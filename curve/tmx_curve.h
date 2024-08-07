// tmx_curve.h - Curve interface implmented by forward and integral.
#pragma once
#ifdef _DEBUG
#include<cassert>
#endif // _DEBUG
#include <cmath>
#include <limits>

namespace tmx::curve {

	// NVI idiom compiles to non-virtual function calls.
	template<class T = double, class F = double>
	class interface {
	public:
		using time_type = T;
        using rate_type = F;

		virtual ~interface() {}

		// Forward at u: f(u).
		constexpr F forward(T u) const
		{
			return u >= 0 ? _forward(u) : std::numeric_limits<F>::quiet_NaN();
		}
		constexpr F operator()(T u) const
		{
			return forward(u);
		}

		// Integral from 0 to u of forward: int_0^u f(s) ds.
		constexpr F integral(T u) const
		{
			return u >= 0 ? _integral(u) : std::numeric_limits<F>::quiet_NaN();
		}

		// Price of one unit received at time u.
		constexpr F discount(T u) const
		{
			return u >= 0 ? std::exp(-integral(u)) : std::numeric_limits<F>::quiet_NaN();
		}

		// Spot/yield is the average of the forward over [0, u].
		// If u is small, use the forward.
		constexpr F spot(T u) const
		{
			if (u < 0) {
				return std::numeric_limits<F>::quiet_NaN();
			}
			if (u + 1 == 1) {
				return _forward(u);
			}

			return _integral(u) / u;
		}

	private:
		constexpr virtual F _forward(T u) const = 0;
		constexpr virtual F _integral(T u) const = 0;
	};

	// Constant curve.
	template<class T = double, class F = double>
	class constant : public interface<T, F> {
		F f;
	public:
		constexpr constant(F f = 0) 
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
			assert(std::isnan(constant(1.).forward(-1)));
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
	class exponential : public interface<T, F> {
		F r;
	public:
		constexpr exponential(F r = 0)
			: r(r)
		{ }

		constexpr F _forward(T u) const override
		{
			return std::exp(r * u);
		}
		constexpr F _integral(T u) const override
		{
			return (r == 0) ? u : std::expm1(r * u) / r;
		}
	};
#ifdef _DEBUG
	inline int exponential_test()
	{
		{
			exponential c(1.);
			assert(std::isnan(exponential(1.).forward(-1.)));
			assert(c.forward(0.) == 1);
			assert(c.integral(0.) == 0);
			assert(c.integral(1.) == std::exp(1.) - 1);
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
		constexpr translate(const interface<T, F>& f, T t)
			: f(f), t(t)
		{
		}
		constexpr translate(const translate& c) = default;
		constexpr translate& operator=(const translate& c) = default;
		constexpr ~translate() = default;

		constexpr F _forward(T u) const override
		{
			return f.forward(u + t);
		}
		constexpr F _integral(T u) const override
		{
			return f.integral(u + t) - f.integral(t);
		}
	};
#ifdef _DEBUG
	inline int translate_test()
	{
		double r = 0.1;
		{
			exponential e(r);
			translate t(e, 1.);
			assert(t.forward(0.9) == std::exp(r*(0.9 + 1)));
			assert(t.integral(0.9) == e.integral(0.9 + 1) - e.integral(1.));
		}

		return 0;
	}
#endif // _DEBUG

	// Left continuous extrapolate _f after _t.
	template<class T = double, class F = double>
	class extrapolate : public interface<T, F> {
		const interface<T, F>& f;
		T _t;
		F _f;
	public:
		constexpr extrapolate(const interface<T, F>& f, T _t, F _f)
			: f(f), _t(_t), _f(_f)
		{ }
		constexpr extrapolate(const extrapolate& p) = default;
		constexpr extrapolate& operator=(const extrapolate& p) = default;
		constexpr ~extrapolate() = default;

		// left continuous forward
		constexpr F _forward(T u) const override
		{
			return u <= _t ? f.forward(u) : _f;
		}
		constexpr F _integral(T u) const override
		{
			return f.integral(u) + u > _t ? _f * (u - _t) : 0;
		}
	};

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

		constexpr F _forward(T u) const override
		{
			return f.forward(u) + g.forward(u);
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
