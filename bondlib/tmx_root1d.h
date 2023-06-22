// fmx_root1d.h - 1-d root using secant method
#pragma once
#include <cmath>
#include <limits>

namespace tmx {

	namespace root1d {
		template<class X, class Y>
		constexpr auto secant(X x0, Y y0, X x1, Y y1)
		{
			return (x0 * y1 - x1 * y0) / (y1 - y0);
		}
	}
	
	namespace secant {

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
				auto x0 = root1d::secant(x0, y0, x1, y1);
				std::swap(x0, x1);
				std::swap(y0, y1);
			}

			return iter ? x0 : std::numeric_limits<X>::quiet_NaN();
		}
	}
} // namespace fms::secant