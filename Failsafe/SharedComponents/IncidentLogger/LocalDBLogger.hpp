#pragma once

#include <memory>
#include <string>

#include "SQLiteCpp/Database.h"
#include "SharedComponents/IncidentLogger/ILogger.hpp"
#include "Utils/ThreadPool.hpp"

class LocalDBLogger : public ILogger {
public:
    LocalDBLogger( const std::string &databasePath );

    virtual void SaveLog( ActionType takenAction, std::shared_ptr< DLPOperation > op ) override;
    virtual void SaveLog( ActionType takenAction, std::shared_ptr< DCOperation > op ) override;

private:
    int FindOrInsertDevice( const DCOperation &op );
    void MaybeInsertUser( uid_t uid );

    ThreadPool threadPool_;
    SQLite::Database db_;

    std::unique_ptr< SQLite::Statement > insertDeviceStatement_;
    std::unique_ptr< SQLite::Statement > findDeviceStatement_;
    std::unique_ptr< SQLite::Statement > insertDCStatement_;

    std::unique_ptr< SQLite::Statement > insertUserStatement_;
    std::unique_ptr< SQLite::Statement > findUserStatement_;
    std::unique_ptr< SQLite::Statement > insertDLPStatement_;
};