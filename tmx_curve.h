// tmx_curve.h - Forward curve interface.
// Values depend on the discount over the interval [t, u]. Default is t = 0.
// Forward f and spot/yield r are related to discount by
// D(u, t) = exp(-int_t^u f(s) ds) = exp(-r(u, t)(u - t)).
// so r(u, t) = 1/(u - t) int_t^u f(s) ds is the average forward rate over [t, u].
#pragma once
#ifdef _DEBUG
#include<cassert>
#endif // _DEBUG
// Use <cmath> once std::exp is constexpr.
#include "tmx_math_hypergeometric.h"

namespace tmx::curve {

	// NVI idiom compiles to non-virtual function calls.
	// Enforce preconditions for all derived curves.
	template<class T = double, class F = double>
	struct base {
		constexpr base() noexcept = default;
		constexpr virtual ~base() {}

		// Forward over [t, u].
		F forward(T u, T t = 0) const noexcept
		{
			return u >= t && t >= 0 ? _forward(u, t) : math::NaN<F>;
		}
		
		// Integral from t to u of forward. int_t^u f(s) ds.
		F integral(T u, T t = 0) const noexcept
		{
			return u >= t && t >= 0? _integral(u, t) : math::NaN<F>;
		}
		
		// Price at time t of one unit received at time u.
		F discount(T u, T t = 0) const noexcept
		{
			return u >= t && t >= 0 ? math::exp(-integral(u, t)) : math::NaN<F>;
		}
		
		// Spot/yield over [t, u]
		F spot(T u, T t = 0) const noexcept
		{
			return u >= t && t >= 0 
				? (u > t + math::sqrt_epsilon<T> 
					? _integral(u, t) / (u - t) 
					: _forward(u, t)) 
				: math::NaN<F>;
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
		constexpr constant(const constant& c) = default;
		constexpr constant& operator=(const constant& c) = default;
		constexpr ~constant() = default;

		constexpr F _forward(T, T) const noexcept override
		{
			return f;
		}
		constexpr F _integral(T u, T t = 0) const noexcept override
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
	
	// s on [t0, t1], 0 elsewhere.
	template<class T = double, class F = double>
	class bump : public base<T, F> {
		F s;
		T t0, t1;
	public:
		constexpr bump(F s, T t0, T t1) noexcept
			: s(s), t0(t0), t1(t1)
		{ }
		constexpr bump(const bump& c) = default;
		constexpr bump& operator=(const bump& c) = default;
		constexpr ~bump() = default;

		constexpr F _forward(T u, T t = 0) const noexcept override
		{
			return s * (t0 <= u - t) * (u - t <= t1);
		}
		constexpr F _integral(T u, T t = 0) const noexcept override
		{
			return s * (std::min(u - t, t1) - std::max(u - t, t0)); // fix!!!
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
		plus(const base<T, F>& f, F s, T t0, T t1)
			: f(f), g(bump(s, t0, t1))
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
