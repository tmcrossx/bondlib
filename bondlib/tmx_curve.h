// tmx_curve.h - interest rate curve
#pragma once
#ifdef _DEBUG
#include <cassert>
#endif
#include <cmath>
#include <algorithm>
#include <vector>
#include "ensure.h"
#include "tmx_view.h"

namespace tmx {

	// NVI base class for piecewise curves.
	template<class T = double, class F = double>
	struct curve {
		virtual ~curve() = default;

		// Last point on the curve.
		std::pair<T, F> back() const
		{
			return _back();
		}

		// f_t(u) is the forward at u seen at time t
		F forward(T u, T t = 0) const
		{
			return _forward(u, t);
		}
		// D_t(u) is the price at time t of a zero coupon bond maturing at time u.
		// D_t(u) = exp(-int_t^u f(s) ds).
		F discount(T u, T t = 0) const
		{
			return _discount(u, t);
		}
		// y_t(u) is the yield at time t over the interval [t, u].
		// D_t(u) = exp(-(u - t) y_t(u)).
		F yield(T u, T t = 0) const
		{
			return _yield(u, t);
		}
		// parallel shift by s
		curve& shift(F s)
		{
			return _shift(s);
		}
	private:
		virtual	std::pair<T, F> _back() const = 0;
		virtual F _forward(T u, T t) const = 0;
		virtual F _discount(T u, T t) const = 0;
		virtual F _yield(T u, T t) const = 0;
		virtual curve& _shift(F s) = 0;
	};

}