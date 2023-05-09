#include "SharedComponents/IncidentLogger/LocalDBLogger.hpp"

#include <chrono>
#include <pwd.h>

#include "Modules/DLP/Operations/DLPOperationOnePath.hpp"
#include "Modules/DLP/Operations/DLPOperationTwoPath.hpp"

#include "spdlog/spdlog.h"

LocalDBLogger::LocalDBLogger( const std::string &databasePath )
    : threadPool_( 1 ),
      db_( databasePath, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE | SQLite::OPEN_FULLMUTEX )
{
    db_.exec( "CREATE TABLE IF NOT EXISTS Users("
              "uid INTEGER PRIMARY KEY,"
              "name TEXT,"
              "UNIQUE(uid, name)"
              ");" );

    db_.exec( "CREATE TABLE IF NOT EXISTS DLPIncidents("
              "id INTEGER PRIMARY KEY AUTOINCREMENT,"
              "incidentTime INTEGER,"
              "user INTEGER,"
              "executable TEXT,"
              "actionTaken TEXT,"
              "operationType TEXT,"
              "sourcePath TEXT,"
              "sourceType TEXT,"
              "destinationPath TEXT,"
              "destinationType TEXT,"
              "FOREIGN KEY(user) REFERENCES Users(uid)"
              ");" );

    db_.exec( "CREATE TABLE IF NOT EXISTS Devices("
              "id INTEGER PRIMARY KEY AUTOINCREMENT,"
              "vendorId INTEGER,"
              "productId INTEGER,"
              "serial TEXT,"
              "UNIQUE(vendorId, productId, serial)"
              ");" );

    db_.exec( "CREATE TABLE IF NOT EXISTS DCIncidents("
              "id INTEGER PRIMARY KEY AUTOINCREMENT,"
              "incidentTime INTEGER,"
              "actionTaken TEXT,"
              "operationType TEXT,"
              "device INTEGER,"
              "FOREIGN KEY(device) REFERENCES Devices(id)"
              ");" );

    insertDeviceStatement_ =
        std::make_unique< SQLite::Statement >( db_, "INSERT OR IGNORE INTO Devices"
                                                    "(vendorId, productId, serial) "
                                                    "VALUES (:vendorId, :productId, :serial);" );

    findDeviceStatement_ = std::make_unique< SQLite::Statement >(
        db_, "SELECT id FROM Devices WHERE "
             "vendorId = :vendorId AND productId = :productId AND serial = :serial;" );

    insertDCStatement_ = std::make_unique< SQLite::Statement >(
        db_, "INSERT INTO DCIncidents"
             "(incidentTime, actionTaken, operationType, device) "
             "VALUES (:time, :action, :operation, :device);" );

    insertUserStatement_ = std::make_unique< SQLite::Statement >( db_, "INSERT OR IGNORE INTO Users"
                                                                       "(uid, name) "
                                                                       "VALUES (:uid, :name);" );

    findUserStatement_ =
        std::make_unique< SQLite::Statement >( db_, "SELECT uid FROM Users WHERE uid = :uid;" );

    insertDLPStatement_ = std::make_unique< SQLite::Statement >(
        db_, "INSERT INTO DLPIncidents ("
             "incidentTime, user, executable, actionTaken, operationType, "
             "sourcePath, sourceType, destinationPath, destinationType) "
             "VALUES (:time, :user, :executable, :action, :operation, :sourcePath, "
             ":sourceType, :destinationPath, :destinationType);" );
}

/* virtual */ void LocalDBLogger::SaveLog( ActionType takenAction,
                                           std::shared_ptr< DLPOperation > op )
{
    threadPool_.QueueTask( [ this, takenAction, op = std::move( op ) ]() {
        MaybeInsertUser( op->UserId() );

        insertDLPStatement_->reset();

        auto unixTime =
            std::chrono::duration_cast< std::chrono::seconds >( op->TimePoint().time_since_epoch() )
                .count();

        insertDLPStatement_->bind( insertDLPStatement_->getIndex( ":time" ), unixTime );
        insertDLPStatement_->bind( insertDLPStatement_->getIndex( ":user" ), op->UserId() );
        insertDLPStatement_->bindNoCopy( insertDLPStatement_->getIndex( ":executable" ),
                                         op->ExecutablePath().data() );
        insertDLPStatement_->bind( insertDLPStatement_->getIndex( ":action" ),
                                   ActionToString( takenAction ) );
        insertDLPStatement_->bind( insertDLPStatement_->getIndex( ":operation" ),
                                   DLPOperationToString( op->Operation() ) );

        if ( op->Operation() == DLPOperationType::FileTransfer ) {
            const auto &castedOp = static_pointer_cast< DLPOperationTwoPath >( op );

            const auto &srcPath = castedOp->SourcePath();
            insertDLPStatement_->bindNoCopy( insertDLPStatement_->getIndex( ":sourcePath" ),
                                             srcPath.Path().c_str() );
            insertDLPStatement_->bind( insertDLPStatement_->getIndex( ":sourceType" ),
                                       PathTypeToString( srcPath.Type() ) );

            const auto &dstPath = castedOp->DestinationPath();
            insertDLPStatement_->bindNoCopy( insertDLPStatement_->getIndex( ":destinationPath" ),
                                             dstPath.Path().c_str() );
            insertDLPStatement_->bind( insertDLPStatement_->getIndex( ":destinationType" ),
                                       PathTypeToString( dstPath.Type() ) );
        }
        else {
            const auto &castedOp = static_pointer_cast< DLPOperationOnePath >( op );

            const auto &srcPath = castedOp->Path();
            insertDLPStatement_->bindNoCopy( insertDLPStatement_->getIndex( ":sourcePath" ),
                                             srcPath.Path().c_str() );
            insertDLPStatement_->bind( insertDLPStatement_->getIndex( ":sourceType" ),
                                       PathTypeToString( srcPath.Type() ) );
        }

        insertDLPStatement_->exec();
    } );
}

/* virtual */ void LocalDBLogger::SaveLog( ActionType takenAction,
                                           std::shared_ptr< DCOperation > op )
{
    threadPool_.QueueTask( [ this, takenAction, op = std::move( op ) ]() {
        int deviceId = FindOrInsertDevice( *op );

        insertDCStatement_->reset();

        auto currentTime = std::chrono::system_clock::now();
        auto currentTimeUnix =
            std::chrono::duration_cast< std::chrono::seconds >( currentTime.time_since_epoch() )
                .count();

        insertDCStatement_->bind( insertDCStatement_->getIndex( ":time" ), currentTimeUnix );
        insertDCStatement_->bind( insertDCStatement_->getIndex( ":action" ),
                                  ActionToString( takenAction ) );
        insertDCStatement_->bind( insertDCStatement_->getIndex( ":operation" ),
                                  DCOperationToString( op->Operation() ) );
        insertDCStatement_->bind( insertDCStatement_->getIndex( ":device" ), deviceId );

        insertDCStatement_->exec();
    } );
}

int LocalDBLogger::FindOrInsertDevice( const DCOperation &op )
{
    insertDeviceStatement_->reset();
    insertDeviceStatement_->bind( insertDeviceStatement_->getIndex( ":vendorId" ),
                                  op.Device().VendorID() );
    insertDeviceStatement_->bind( insertDeviceStatement_->getIndex( ":productId" ),
                                  op.Device().ProductID() );
    insertDeviceStatement_->bindNoCopy( insertDeviceStatement_->getIndex( ":serial" ),
                                        op.Device().Serial().data() );
    insertDeviceStatement_->exec();

    findDeviceStatement_->reset();
    findDeviceStatement_->bind( findDeviceStatement_->getIndex( ":vendorId" ),
                                op.Device().VendorID() );
    findDeviceStatement_->bind( findDeviceStatement_->getIndex( ":productId" ),
                                op.Device().ProductID() );
    findDeviceStatement_->bindNoCopy( findDeviceStatement_->getIndex( ":serial" ),
                                      op.Device().Serial().data() );

    if ( !findDeviceStatement_->executeStep() )
        throw std::runtime_error( "Failed to find device in database!" );

    return findDeviceStatement_->getColumn( 0 );
}

void LocalDBLogger::MaybeInsertUser( uid_t uid )
{
    findUserStatement_->reset();

    findUserStatement_->bind( findUserStatement_->getIndex( ":uid" ), uid );
    if ( findUserStatement_->executeStep() )
        return;

    struct passwd pw;
    struct passwd *pwp;
    char buffer[ 1024 ];
    int rc = getpwuid_r( uid, &pw, buffer, 1024, &pwp );
    if ( rc != 0 )
        throw std::runtime_error( "Username resolution failed" );

    insertUserStatement_->reset();
    insertUserStatement_->bind( insertUserStatement_->getIndex( ":uid" ), uid );
    insertUserStatement_->bind( insertUserStatement_->getIndex( ":name" ), pwp->pw_name );

    insertUserStatement_->exec();
}