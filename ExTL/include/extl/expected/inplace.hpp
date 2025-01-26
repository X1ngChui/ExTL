#pragma once

namespace extl {
    /**
     * Tag type used to indicate in-place construction.
     */
    struct in_place_t { explicit in_place_t() = default; };
    
    /**
     * Tag object used to indicate in-place construction.
     */
    inline constexpr in_place_t in_place{};
}