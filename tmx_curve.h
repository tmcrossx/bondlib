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
		F forward(T u) const
		{
			return u >= 0 ? _forward(u) : std::numeric_limits<F>::quiet_NaN();
		}

		// Integral from 0 to u of forward: int_0^u f(s) ds.
		F integral(T u) const
		{
			return u >= 0 ? _integral(u) : std::numeric_limits<F>::quiet_NaN();
		}

		// Price of one unit received at time u.
		F discount(T u) const
		{
			return u >= 0 ? std::exp(-integral(u)) : std::numeric_limits<F>::quiet_NaN();
		}

		// Spot/yield is the average of the forward over [0, u].
		// If u is small, use the forward.
		F spot(T u) const
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
		virtual F _forward(T u) const = 0;
		virtual F _integral(T u) const = 0;
	};

	// Constant curve.
	template<class T = double, class F = double>
	class constant : public interface<T, F> {
		F f;
	public:
		constexpr constant(F f = std::numeric_limits<F>::quiet_NaN()) 
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
	class exponential : public interface<T, F> {
		F r;
	public:
		exponential(F r = 0) 
			: r(r)
		{ }

		F _forward(T u) const override
		{
			return std::exp(r * u);
		}
		F _integral(T u) const override
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
