// tmx_math_limits.h - Short names for numeric limits
#pragma once

namespace tmx::math {

	template<class X>
	constexpr X NaN = std::numeric_limits<X>::quiet_NaN();
	template<class X>
	constexpr X epsilon = std::numeric_limits<X>::epsilon();
	template<class X>
	constexpr X infinity = std::numeric_limits<X>::infinity();

} // namespace tmx::math
