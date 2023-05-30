// fms_secant.h - 1-d root using secant method
#pragma once
#include <cmath>
#include <limits>

namespace tmx::secant {

	template<class X>
	constexpr X epsilon = std::numeric_limits<X>::epsilon();
	//inline static constexpr double tolerance = 1e-8; // sqrt(epsilon);

	template<class F, class X = double>
	constexpr X solve(const F& f, X x0, X x1,
		double tol = 100 * epsilon<X>, int iter = 100)
	{
		auto y0 = f(x0);

		while (iter-- and y0 > tol or y0 < -tol) {
			auto y1 = f(x1);
			x0 = (x0 * y1 - x1 * y0) / (y1 - y0);
			std::swap(x0, x1);
			std::swap(y0, y1);
		}

		return iter ? x0 : std::numeric_limits<X>::quiet_NaN();
	}

} // namespace fms::secant