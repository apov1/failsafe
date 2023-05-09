#pragma once

#include <atomic>
#include <condition_variable>
#include <deque>
#include <future>
#include <mutex>
#include <thread>
#include <vector>

class ThreadPool {
public:
    ThreadPool( std::size_t numThreads )
    {
        if ( numThreads < 1 )
            throw std::runtime_error( "numThreads must be larger than 0!" );

        for ( std::size_t i = 0; i < numThreads; ++i )
            threads_.emplace_back( [ this ]() { workerThreadFunc(); } );
    }

    ~ThreadPool()
    {
        shouldStop_ = true;

        {
            std::scoped_lock lock( mtx_ );
            cv_.notify_all();
        }

        for ( auto &thread : threads_ )
            if ( thread.joinable() )
                thread.join();
    }

    template < typename F >
    void QueueTask( F task )
    {
        if ( shouldStop_ )
            return;

        std::scoped_lock lock( mtx_ );

        tasks_.emplace_back( std::move( task ) );
        cv_.notify_one();
    }

private:
    mutable std::mutex mtx_;
    std::condition_variable cv_;
    std::atomic< bool > shouldStop_ = false;

    std::vector< std::thread > threads_;
    std::deque< std::packaged_task< void() > > tasks_;

    void workerThreadFunc()
    {
        while ( true ) {

            if ( shouldStop_ )
                return;

            std::packaged_task< void() > workerTask;

            {
                std::unique_lock lock( mtx_ );
                cv_.wait( lock, [ this ]() { return shouldStop_ || !tasks_.empty(); } );

                if ( shouldStop_ )
                    return;

                workerTask = std::move( tasks_.front() );
                tasks_.pop_front();
            }

            workerTask();
        }
    }
};