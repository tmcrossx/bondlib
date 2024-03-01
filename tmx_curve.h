// tmx_curve.h - Interest rate curve interface.
// Values depend on the discount over the interval [t, u]. Default is t = 0.
// D(u, t) = exp(-int_t^u f(s) ds) = exp(-r(u, t)(u - t)).
#pragma once
#include <cmath>
#include <utility>

namespace tmx::curve {

	template<class T = double, class F = double>
	struct base {
		constexpr base() {}
		virtual ~base() {}

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
		// Forward over [t, u].
		F forward(T u, T t = 0) const
		{
			return _forward(u, t);
		}
		// Spot over [t, u]
		F spot(T u, T t) const
		{
			// TODO: u ~= t???
			return -std::log(discount(u, t)) / (u - t);
		}

	private:
		virtual F _forward(T u, T t) const = 0;
		virtual F _integral(T u, T t) const = 0;
	};

	// Constant curve.
	template<class T = double, class F = double>
	class constant : public base<T, F> {
		F f;
	public:
		constexpr constant(F f = math::NaN<F>)
			: f(f)
		{ }
		constexpr constant(const constant& c) = default;
		constexpr constant& operator=(const constant& c) = default;
		constexpr ~constant() = default;

		constexpr F _forward(T, T) const override
		{
			return f;
		}
		constexpr F _integral(T u, T t) const override
		{
			return f * (u - t);
		}
#ifdef _DEBUG
		static int test()
		{
			constant c(1);
			constant c2{ c };
			c = c2;

			assert(1 == c.value(0));
			assert(0 == c.integral(0));
			//c.extrapolate(1);
			//c.back();

			return 0;
		}
#endif // _DEBUG
	
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
		plus(const base<T, F>& f, F s)
			: f(f), g(constant(s))
		{ }
		plus(const plus& p) = default;
		plus& operator=(const plus& p) = default;
		~plus() = default;

		F _forward(T u, T t) const override
		{
			return f.forward(u, t) + g.forward(u, t);
		}
		F _integral(T u, T t) const override
		{
			return f.integral(u, t) + g.integral(u, t);
		}
	};

}// namespace tmx::curve

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

#ifdef _DEBUG
inline int curve_operator_test()
{	
	double f = 2.0;
	tmx::curve::constant c1(1.0);
	tmx::curve::constant c2(3.0);

	tmx::curve::plus a1 = c1 + c2;
	tmx::curve::plus a2 = c1 + f;
	assert(a1.forward(0) == 4.0);
	assert(a2.forward(0) == 3.0);

	return 0;
}
#endif // _DEBUG
