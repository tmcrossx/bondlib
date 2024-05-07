// tmx_curve.h - Forward curve interface.
// D(u, t) is the discount over [t, u].
// Forward f and spot/yield r are related to discount by
// D(u, t) = exp(-int_t^u f(s) ds) = exp(-r(u, t)(u - t)).
// Note r(u, t) = int_t^u f(s) ds/(u - t) is the average forward rate over [t, u].
#pragma once
#ifdef _DEBUG
#include<cassert>
#endif // _DEBUG
// TODO: Use <cmath> once it is constexpr.
#include "tmx_math_hypergeometric.h"

namespace tmx::curve {

	// NVI idiom compiles to non-virtual function calls.
	// Enforce preconditions for all derived classes.
	template<class T = double, class F = double>
	class interface {
	public:
		virtual ~interface() {}

		// Forward over [0, u].
		F forward(T u) const
		{
			return u >= 0 ? _forward(u) : math::NaN<F>;
		}

		// Integral from 0 to u of forward. int_0^u f(s) ds.
		F integral(T u) const
		{
			return u >= 0 ? _integral(u) : math::NaN<F>;
		}

		// Price at time t of one unit received at time u.
		F discount(T u) const
		{
			return u >= 0 ? math::exp(-integral(u)) : math::NaN<F>;
		}

		// Spot/yield is the average of the forward over [0, u]
		F spot(T u) const
		{
			return u >= 0
				? (u > math::sqrt_epsilon<T>
					? _integral(u) / u
					: _forward(u))
				: math::NaN<F>;
		}

	private:
		virtual F _forward(T u) const = 0;
		virtual F _integral(T u) const = 0;
	};

	// Constant curve.
	template<class T = double, class F = double>
	class constant : public interface<T, F> {
		F f;
	public:
		constexpr constant(F f = math::NaN<F>) 
			: f(f)
		{ }

		constexpr F _forward(T) const override
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
			assert(math::isnan(constant(1.).forward(-1)));
			assert(c.forward(0.) == 1);
			assert(c.integral(0.) == 0);
			assert(c.integral(2.) == 2.);
			assert(c.spot(0.) == 1);
			assert(c.spot(1.) == 1);
		}

		return 0;
	}

#endif // _DEBUG

	// Exponential curve e^{rt}
	template<class T = double, class F = double>
	class exp : public interface<T, F> {
		F r;
	public:
		constexpr exp(F r = 0) 
			: r(r)
		{ }

		constexpr F _forward(T u) const override
		{
			return math::exp(r * u);
		}
		constexpr F _integral(T u) const override
		{
			return math::fabs(r * u) < math::sqrt_epsilon<F> ? u * (1 + r * u) / 2 : (math::exp(r * u) - 1) / r;
		}
	};
#ifdef _DEBUG
	inline int exp_test()
	{
		{
			exp c(1.);
			assert(math::isnan(exp(1.).forward(-1.)));
			assert(c.forward(0.) == 1);
			assert(c.integral(0.) == 0);
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
		constexpr bump(F s, T t0, T t1) 
			: s(s), t0(t0), t1(t1)
		{ }
		constexpr bump(const bump& c) = default;
		constexpr bump& operator=(const bump& c) = default;
		constexpr ~bump() = default;

		constexpr F _forward(T u) const override
		{
			return s * (t0 <= u) * (u <= t1);
		}
		constexpr F _integral(T u) const override
		{
			return s * (std::min(u, t1) - std::max(T(0), t0)) * (u >= t0) * (0 <= t1);
		}
	};
#ifdef _DEBUG
	inline int bump_test()
	{
		{
			bump b(0.5, 1., 2.);
			assert(b.forward(0.9) == 0);
			assert(b.forward(1) == 0.5);
			assert(b.forward(2) == 0.5);
			assert(b.forward(2.1) == 0);
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
		translate(const interface<T, F>& f, T t)
			: f(f), t(t)
		{
		}
		translate(const translate& c) = default;
		translate& operator=(const translate& c) = default;
		~translate() = default;

		F _forward(T u) const override
		{
			return f.forward(u + t);
		}
		F _integral(T u) const override
		{
			return f.integral(u) - f.integral(t);
		}
	};
		
	// Add two curves. Assumes lifetime of f and g.
	template<class T = double, class F = double>
	class plus : public interface<T, F> {
		const interface<T, F>& f;
		const interface<T, F>& g;
	public:
		plus(const interface<T, F>& f, const interface<T, F>& g)
			: f(f), g(g)
		{ }
		plus(const plus& p) = default;
		plus& operator=(const plus& p) = default;
		~plus() = default;

		F _forward(T u) const override
		{
			return f.forward(u) + g.forward(u);
		}
		F _integral(T u) const override
		{
			return f.integral(u) + g.integral(u);
		}
	};

} // namespace tmx::curve

// Add two curves.
template<class T, class F>
inline tmx::curve::plus<T, F> operator+(const tmx::curve::interface<T, F>& f, const tmx::curve::interface<T, F>& g)
{
	return tmx::curve::plus<T, F>(f, g);
}
