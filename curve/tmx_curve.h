// tmx_curve.h - Curve interface implmented by forward and integral.
#pragma once
#ifdef _DEBUG
#include<cassert>
#endif // _DEBUG
#include <cmath>
#include <limits>
#include "math/tmx_math_limits.h"

namespace tmx::curve {

	template<class X>
	constexpr X NaN = std::numeric_limits<X>::quiet_NaN();
	template<class X>
	constexpr X infinity = std::numeric_limits<X>::infinity();

	// Subclasses implement _value and _integral.
	// Handle extrapolation by f after t at the interface level.
	template<class T = double, class F = double>
	class interface {
	public:
		using time_type = T;
		using rate_type = F;

		virtual ~interface() {}

		// Forward at u.
		constexpr F value(T u, T t = infinity<T>, F f = NaN<F>) const
		{
			return u < 0 ? NaN<F> : u <= t ? _value(u) : f;
		}
		constexpr F operator()(T u, T t = infinity<T>, F f = NaN<F>) const
		{
			return value(u, t, f);
		}

		// Integral from 0 to u of forward: int_0^u f(s) ds.
		constexpr F integral(T u, T t = infinity<T>, F f = NaN<F>) const
		{
			return u < 0 ? NaN<F> : u == 0 ? 0 : u <= t ? _integral(u) : _integral(t) + f * (u - t);
		}

		// Price of one unit received at time u.
		constexpr F discount(T u, T t = infinity<T>, F f = NaN<F>) const
		{
			return u < 0 ? NaN<F> : std::exp(-integral(u, t, f));
		}

		// Spot/yield is the average of the forward over [0, u].
		// If u is small, use the forward.
		constexpr F spot(T u, T t = infinity<T>, F f = NaN<F>) const
		{
			return u < 0 ? NaN<F> : u < math::sqrt_epsilon<T> ? value(u, t, f) : integral(u, t, f) / u;
		}

	private:
		constexpr virtual F _value(T u) const = 0;
		constexpr virtual F _integral(T u) const = 0;
	};

	// Provide t and f where forward(u) = f for u > t.
	// Assumes lifetime of f.
	template<class T = double, class F = double>
	class extrapolate : public interface<T, F> {
		const interface<T, F>& f;
		T _t;
		F _f;
	public:
		extrapolate(const interface<T, F>& f, T _t = infinity<T>, F _f = NaN<F>)
			: f(f), _t(_t), _f(_f)
		{ }
		constexpr F _value(T u) const override
		{
			return f.value(u, _t, _f);
		}
		constexpr F _integral(T u) const override
		{
			return f.integral(u, _t, _f);
		}
	};

	// Constant curve.
	template<class T = double, class F = double>
	class constant : public interface<T, F> {
		F f;
	public:
		constexpr constant(F f = 0)
			: f(f)
		{ }

		constexpr F _value(T) const override
		{
			return f;
		}
		constexpr F _integral(T u) const override
		{
			return f * u;
		}
	};
#ifdef _DEBUG
	inline int constant_test()
	{
		{
			constant c(1.);
			assert(std::isnan(constant(1.).value(-1)));
			assert(c.value(0.) == 1);
			assert(c.integral(0.) == 0);
			assert(c.integral(2.) == 2.);
			assert(c.spot(0.) == 1);
			assert(c.spot(1.) == 1);
		}

		return 0;
	}

#endif // _DEBUG

	// s on [t0, t1], 0 elsewhere.
	template<class T = double, class F = double>
	class bump : public interface<T, F> {
		F s;
		T t0, t1;
	public:
		constexpr bump(F s, T t0 = 0, T t1 = math::infinity<T>)
			: s(s), t0(t0), t1(t1)
		{ }
		constexpr bump(const bump& c) = default;
		constexpr bump& operator=(const bump& c) = default;
		constexpr ~bump() = default;

		constexpr F _value(T u) const override
		{
			return s * (t0 <= u) * (u <= t1);
		}
		constexpr F _integral(T u) const override
		{
			return s * (std::min(u, t1) - t0) * (u >= t0);
		}
	};
#ifdef _DEBUG
	inline int bump_test()
	{
		{
			bump b(0.5, 1., 2.);
			assert(b.value(0.9) == 0);
			assert(b.value(1) == 0.5);
			assert(b.value(2) == 0.5);
			assert(b.value(2.1) == 0);
			assert(b.integral(0) == 0);
			assert(b.integral(1) == 0);
			assert(b.integral(2) == 0.5);
		}

		return 0;
	}
#endif // _DEBUG

	// Shift curve forward by t.
	template<class T = double, class F = double>
	class translate : public interface<T, F> {
		const interface<T, F>& f;
		T t;
	public:
		constexpr translate(const interface<T, F>& f, T t)
			: f(f), t(t)
		{
		}
		constexpr translate(const translate& c) = default;
		constexpr translate& operator=(const translate& c) = default;
		constexpr ~translate() = default;

		constexpr F _value(T u) const override
		{
			return f.value(u + t);
		}
		constexpr F _integral(T u) const override
		{
			return f.integral(u + t) - f.integral(t);
		}
	};
#ifdef _DEBUG
	inline int translate_test()
	{
		{
		}

		return 0;
	}
#endif // _DEBUG


	// Add two curves. Assumes lifetime of f and g.
	template<class T = double, class F = double>
	class plus : public interface<T, F> {
		const interface<T, F>& f;
		const interface<T, F>& g;
	public:
		constexpr plus(const interface<T, F>& f, const interface<T, F>& g)
			: f(f), g(g)
		{ }
		constexpr plus(const plus& p) = default;
		constexpr plus& operator=(const plus& p) = default;
		constexpr ~plus() = default;

		constexpr F _value(T u) const override
		{
			return f.value(u) + g.value(u);
		}
		constexpr F _integral(T u) const override
		{
			return f.integral(u) + g.integral(u);
		}
	};

} // namespace tmx::curve

// Add two curves.
template<class T, class F>
constexpr  tmx::curve::plus<T, F> operator+(const tmx::curve::interface<T, F>& f, const tmx::curve::interface<T, F>& g)
{
	return tmx::curve::plus<T, F>(f, g);
}
