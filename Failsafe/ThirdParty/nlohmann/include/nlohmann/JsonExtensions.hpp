#pragma once

#include "nlohmann.hpp"

#include <memory>
#include <optional>

namespace nlohmann {
template < typename T >
struct adl_serializer< std::unique_ptr< T > > {
    static void from_json( const json &j, std::unique_ptr< T > &out ) {
        if ( j.is_null() )
            out = nullptr;
        else
            out = std::make_unique< T >( j.get< T >() );
    }
};

template < typename T >
struct adl_serializer< std::optional< T > > {
    static void from_json( const json &j, std::optional< T > &out ) {
        if ( j.is_null() )
            out = std::nullopt;
        else
            out = std::make_optional< T >( j.get< T >() );
    }
};

} // namespace nlohmann