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
		bool valid(T u, T t) const
		{
			return 0 <= t && t <= u;
		}
	public:
		virtual ~interface() {}

		// Forward over [t, u].
		F forward(T u, T t = 0) const
		{
			return valid(u, t) ? _forward(u, t) : math::NaN<F>;
		}

		// Integral from t to u of forward. int_t^u f(s) ds.
		F integral(T u, T t = 0) const
		{
			return valid(u, t) ? _integral(u, t) : math::NaN<F>;
		}

		// Price at time t of one unit received at time u.
		F discount(T u, T t = 0) const
		{
			return valid(u, t) ? math::exp(-integral(u, t)) : math::NaN<F>;
		}

		// Spot/yield is the average of the forward over [t, u]
		F spot(T u, T t = 0) const
		{
			return valid(u, t)
				? (u > t + math::sqrt_epsilon<T>
					? _integral(u, t) / (u - t)
					: _forward(u, t))
				: math::NaN<F>;
		}

	private:
		virtual F _forward(T u, T t) const = 0;
		virtual F _integral(T u, T t) const = 0;
	};

	// Constant curve.
	template<class T = double, class F = double>
	class constant : public interface<T, F> {
		F f;
	public:
		constexpr constant(F f = math::NaN<F>) 
			: f(f)
		{ }

		constexpr F _forward(T, T) const override
		{
			return f;
		}
		constexpr F _integral(T u, T t = 0) const override
		{
			return f * (u - t);
		}
	};
#ifdef _DEBUG
	inline int constant_test()
	{
		{
			constant c(1.);
			assert(math::isnan(constant(1.).forward(-1., 0.)));
			assert(c.forward(0., 0.) == 1);
			assert(c.forward(2., 1.) == 1);
			assert(c.integral(0., 0.) == 0);
			assert(c.integral(2., 0.) == 2.);
			assert(c.spot(0., 0.) == 1);
			assert(c.spot(1., 0.) == 1);
			assert(c.spot(2., 1.) == 1);
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

		constexpr F _forward(T u, T t = 0) const override
		{
			return math::exp(r * (u - t));
		}
		constexpr F _integral(T u, T t = 0) const override
		{
			T dt = u - t;

			return math::fabs(r * dt) < math::sqrt_epsilon<F> ? dt * (1 + r * dt) / 2 : (math::exp(r * u) - math::exp(r * t)) / r;
		}
	};
#ifdef _DEBUG
	inline int exp_test()
	{
		{
			exp c(1.);
			assert(math::isnan(exp(1.).forward(-1., 0.)));
			assert(c.forward(0., 0.) == 1);
			assert(c.forward(2., 1.) == math::exp(1.));
			assert(c.integral(0., 0.) == 0);
			assert(c.integral(2., 1.) == 2.);
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

		constexpr F _forward(T u, T t = 0) const override
		{
			return s * (t0 <= u - t) * (u - t <= t1);
		}
		constexpr F _integral(T u, T t = 0) const override
		{
			return s * (std::min(u, t1) - std::max(t, t0)) * (u >= t0) * (t <= t1);
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
inline tmx::curve::plus<T, F> operator+(const tmx::curve::interface<T, F>& f, const tmx::curve::interface<T, F>& g)
{
	return tmx::curve::plus<T, F>(f, g);
}
