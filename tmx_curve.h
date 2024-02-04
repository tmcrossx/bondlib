// tmx_curve.h - Interest rate curve interface.
#pragma once
#include <cmath>
#include <utility>
#include <vector>
#include "tmx_pwflat.h"

namespace tmx::curve {

	template<class T = double, class F = double>
	struct base {
		virtual ~base() {}

		// Forward value at time u.
		F value(T u) const
		{
			return _value(u);
		}
		// Integral from t to u of forward.
		F integral(T u, T t = 0) const
		{
			return _integral(u, t);
		}
		// Extend curve by _f.
		base& extrapolate(F _f)
		{
			return _extrapolate(_f);
		}
		// Return last point on the curve.
		std::pair<T, F> back() const
		{
			return _back();
		}

		// Forward at u as seen from time t.
		F forward(T u, T t = 0) const
		{
			return forward(u + t);
		}
		// Discount at u as seen from time t.
		F discount(T u, T t = 0) const
		{
			return std::exp(-integral(u,t));
		}
		// Spot r(u,t) satisfies D(u, t) = exp(-r(u, t)(u - t)).
		F spot(T u, T t) const
		{
			// TODO: u ~= t???
			return -std::log(discount(u, t)) / (u - t);
		}

	private:
		virtual F _value(T u) const = 0;
		virtual F _integral(T u, T t) const = 0;
		virtual base& _extrapolate(F _f) = 0;
		virtual std::pair<T, F> _back() const = 0;
	};

	// Constant curve.
	template<class T = double, class F = double>
	class constant : public base<T, F> {
		F f;
	public:
		constant(F f = math::NaN<F>)
			: f(f)
		{ }
		F _value([[maybe_unused]]T u) const override
		{
			return f;
		}
		F _integral(T u, T t) const override
		{
			return f * (u - t);
		}
		constant& _extrapolate(F _f) override
		{
			f = _f;

			return *this;
		}
		std::pair<T, F> _back() const override
		{
			return { math::infinity<T>, f };
		}
	};

	// Add two curves.
	template<class T = double, class F = double>
	class plus : public base<T,F> {
		const base<T, F>& f;
		const base<T, F>& g;
	public:
		plus(const base<T, F>& f, const base<T, F>& g)
			: f(f), g(g)
		{ }
		F _forward(T u) const override
		{
			return f.value(u) + g.value(u);
		}
		F _integral(T u) const override
		{
			return f.integral(u) + g.integral(u);
		}
		plus& _extrapolate(F _f) override
		{
			f.extrapolate(_f);
			g.extrapolate(_f);

			return *this;
		}	
		// Smallest last point on both curves.
		std::pair<T, F> _back() const override
		{
			const auto fb = f.back();
			const auto gb = g.back();
			T ub = std::min(fb.first, gb.first);

			return { ub, _forward(ub) };
		}
	};
}

// Add two curves.
template<class T, class F>
inline tmx::curve::plus<T, F> operator+(const tmx::curve::base<T, F>& f, const tmx::curve::base<T, F>& g)
{
	return tmx::curve::plus<T, F>(f, g);
}
// Add a constant spread.
template<class T, class F>
inline tmx::curve::plus<T, F> operator+(const tmx::curve::base<T, F>& f, F s)
{
	return tmx::curve::plus<T, F>(f, tmx::curve::constant<T, F>(s));
}