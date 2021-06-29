#ifndef WOTH_WGE_DEBUG_ASSERT_HPP
#define WOTH_WGE_DEBUG_ASSERT_HPP

#include <cassert>

#define WGE_CONTRACT_CHECK(str, x) assert(str && (x))

#define wge_expects(x) WGE_CONTRACT_CHECK("WGE: Precondition failure", x)
#define wge_ensures(x) WGE_CONTRACT_CHECK("WGE: Postcondition failure", x)
#endif  // WOTH_WGE_DEBUG_ASSERT_HPP
