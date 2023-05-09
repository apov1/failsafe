#pragma once

#include <functional>

template < typename T >
class AtExit {
public:
    AtExit( T func )
        : atExit_( std::move( func ) )
    {
    }

    ~AtExit()
    {
        atExit_();
    }

private:
    T atExit_;
};