// tmx_curve.h - Forward curve interface.
// Values depend on the discount over the interval [t, u]. Default is t = 0.
// D(u, t) = exp(-int_t^u f(s) ds) = exp(-r(u, t)(u - t)).
#pragma once
#ifdef _DEBUG
#include<cassert>
#endif // _DEBUG
#include <utility>
#include "tmx_math_hypergeometric.h"

namespace tmx::curve {

	template<class T = double, class F = double>
	struct base {
		constexpr base() noexcept = default;
		constexpr virtual ~base() {}

		// Forward over [t, u].
		F forward(T u, T t = 0) const noexcept
		{
			return u >= t ? _forward(u, t) : math::NaN<F>;
		}
		// Integral from t to u of forward. int_t^u f(s) ds.
		F integral(T u, T t = 0) const noexcept
		{
			return u >= t ? _integral(u, t) : math::NaN<F>;
		}
		// Price at time t of one unit received at time u.
		F discount(T u, T t = 0) const noexcept
		{
			return u >= t ? math::exp(-integral(u, t)) : math::NaN<F>;
		}
		// Spot over [t, u]
		F spot(T u, T t = 0) const noexcept
		{
			return u >= t ? (u > t + math::sqrt_epsilon<T> ? -_integral(u, t) / (u - t) : _forward(u, t)) : math::NaN<F>;
		}

	private:
		virtual F _forward(T u, T t) const noexcept = 0;
		virtual F _integral(T u, T t) const noexcept = 0;
	};

	// Constant curve.
	template<class T = double, class F = double>
	class constant : public base<T, F> {
		F f;
	public:
		constexpr constant(F f = math::NaN<F>) noexcept
			: f(f)
		{ }

		constexpr F _forward(T u, T t = 0) const noexcept override
		{
			return (u >= t and t >= 0) ? f : math::NaN<F>;
		}
		constexpr F _integral(T u, T t = 0) const noexcept override
		{
			return (u >= t and t >= 0) ? f * (u - t) : math::NaN<F>;
		}
	};
#ifdef _DEBUG
	static_assert(math::isnan(constant(1.)._forward(-1., 0.)));
	static_assert(constant(1.)._forward(0., 0.) == 1);
	static_assert(constant(1.)._integral(0., 0.) == 0);
	static_assert(constant(1.)._integral(2., 0.) == 2.);
//	static_assert(constant(1.).spot(0., 0.) == 1);
#endif // _DEBUG

	// Linear curve.
	template<class T = double, class F = double>
	class linear : public base<T, F> {
		F f, df;
	public:
		constexpr linear(F f = math::NaN<F>, F df = 0) noexcept
			: f(f), df(df)
		{ }

		constexpr F _forward(T u, T t = 0) const noexcept override
		{
			return (u >= t and t >= 0) ? f + df*(u - t) : math::NaN<F>;
		}
		constexpr F _integral(T u, T t = 0) const noexcept override
		{
			return (u >= t and t >= 0) ? f * (u - t) + df * (u - t)*(u - t)/2 : math::NaN<F>;
		}
	};
#ifdef _DEBUG
	/*
	static_assert(math::isnan(linear(1.)._forward(-1., 0.)));
	static_assert(linear(1.)._forward(0., 0.) == 1);
	static_assert(linear(1.)._integral(0., 0.) == 0);
	static_assert(linear(1.)._integral(2., 0.) == 2.);
	//	static_assert(linear(1.).spot(0., 0.) == 1);
	*/
#endif // _DEBUG

	// Add two curves.
	template<class T = double, class F = double>
	class plus : public base<T, F> {
		const base<T, F>& f;
		const base<T, F>& g;
	public:
		plus(const base<T, F>& f, const base<T, F>& g)
			: f(f), g(g)
		{ }
		plus(const base<T, F>& f, F s)
			: f(f), g(constant(s))
		{ }
		plus(const plus& p) = default;
		plus& operator=(const plus& p) = default;
		~plus() = default;

		F _forward(T u, T t = 0) const override
		{
			return f.forward(u, t) + g.forward(u, t);
		}
		F _integral(T u, T t = 0) const override
		{
			return f.integral(u, t) + g.integral(u, t);
		}
	};

} // namespace tmx::curve

 // Add two curves.
template<class T, class F>
inline tmx::curve::plus<T, F> operator+(const tmx::curve::base<T, F>& f, const tmx::curve::base<T, F>& g)
{
	return tmx::curve::plus<T, F>(f, g);
}
// Add a constant spread.
template<class T, class F>
inline tmx::curve::plus<T, F> operator+(tmx::curve::base<T, F>& f, F s)
{
	return tmx::curve::plus<T, F>(f, s);
}
