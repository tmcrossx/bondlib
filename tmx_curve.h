// tmx_curve.h - Forward curve interface.
// Values depend on the discount over the interval [t, u]. Default is t = 0.
// D(u, t) = exp(-int_t^u f(s) ds) = exp(-r(u, t)(u - t)).
#pragma once
#include <cmath>
#include <utility>

namespace tmx::curve {

	template<class T = double, class F = double>
	struct base {
		virtual ~base() {}

		// Forward over [t, u].
		F forward(T u, T t = 0) const
		{
			return _forward(u, t);
		}
		// Integral from t to u of forward. int_t^u f(s) ds.
		F integral(T u, T t = 0) const
		{
			return _integral(u, t);
		}
		// Price at time t of one unit received at time u.
		F discount(T u, T t = 0) const
		{
			return std::exp(-integral(u, t));
		}
		// Spot over [t, u]
		F spot(T u, T t = 0) const
		{
			// TODO: u ~= t???
			return -std::log(discount(u, t)) / (u - t);
		}

	private:
		virtual F _forward(T u, T t) const = 0;
		virtual F _integral(T u, T t) const = 0;
	};
} // namespace tmx::curve