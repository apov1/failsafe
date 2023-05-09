#include <catch2/catch.hpp>

#include "Utils/ThreadPool.hpp"
#include <thread>

TEST_CASE( "ThreadPool: Bad construction", "" )
{
    REQUIRE_THROWS( ThreadPool( 0 ) );
}

TEST_CASE( "ThreadPool: Execution of single function", "" )
{
    auto pool = ThreadPool( 1 );

    int testVar = 0;
    pool.QueueTask( [ &testVar ]() { testVar = 1; } );

    // Wait to get the function executed
    std::this_thread::sleep_for( std::chrono::milliseconds{ 100 } );

    REQUIRE( testVar == 1 );
}

TEST_CASE( "ThreadPool: Sequential execution of functions", "" )
{
    auto pool = ThreadPool( 1 );

    int testVar = 0;
    pool.QueueTask( [ &testVar ]() { testVar = 1; } );
    pool.QueueTask( [ &testVar ]() {
        REQUIRE( testVar == 1 );
        testVar = 2;
    } );
    pool.QueueTask( [ &testVar ]() {
        REQUIRE( testVar == 2 );
        testVar = 3;
    } );
    pool.QueueTask( [ &testVar ]() { REQUIRE( testVar == 3 ); } );
}
