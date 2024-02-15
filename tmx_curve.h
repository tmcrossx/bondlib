// tmx_curve.h - Interest rate curve interface.
#pragma once
#include <cmath>
#include <utility>
#include <vector>
#include "tmx_pwflat.h"
#include <iostream>
namespace tmx::curve {

	template<class T = double, class F = double>
	struct base {
		constexpr base() {}
		virtual ~base() {}

		// Forward value at time u.
		F value(T u) const
		{
			return _value(u);
		}
		// Integral from t to u of forward. int_t^u f(s) ds.
		F integral(T u, T t = 0) const
		{
			return _integral(u, t);
		}
		// Extend curve by _f.
		base& extrapolate(F _f)
		{
			return _extrapolate(_f);
		}
		// Current extrapolated value.
		F extrapolate() const
		{
			return _extrapolate();
		}
		// Return last (non-extrapolated) point on the curve.
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
		virtual F _extrapolate() const = 0;
		virtual std::pair<T, F> _back() const = 0;
	};

	// Constant curve.
	template<class T = double, class F = double>
	class constant : public base<T, F> {
		F f;
	public:
		constexpr constant(F f = math::NaN<F>)
			: f(f)
		{ }
		constexpr F _value([[maybe_unused]] T u) const override
		{
			return f;
		}
		constexpr F _integral(T u, T t) const override
		{
			return f * (u - t);
		}
		constexpr constant& _extrapolate(F _f) override
		{
			f = _f;

			return *this;
		}
		constexpr F _extrapolate() const override
		{
			return f;
		}
		constexpr std::pair<T, F> _back() const override
		{
			return { math::infinity<T>, f };
		}
#ifdef _DEBUG
		static int test()
		{
			constant c(1);
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
		base<T, F>& f;
		base<T, F>& g;
	public:
		plus(base<T, F>& f, base<T, F>& g)
			: f(f), g(g)
		{ }
		F _value(T u) const override
		{
			return f.value(u) + g.value(u);
		}
		F _integral(T u, [[maybe_unused]] T t) const override
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

			return { std::min(fb.first, gb.first), fb.second + gb.second };
		}
	};


// Add two curves.
template<class T, class F>
inline tmx::curve::plus<T, F> operator+(tmx::curve::base<T, F>& f, tmx::curve::base<T, F>& g)
{
	tmx::curve::plus<T, F> c(f, g);
	//return tmx::curve::plus<T, F>(f, g);
	return c;

}
// Add a constant spread.
// TODO: (Tianxin) put in non-contexpr test using assert.
template<class T, class F>
inline tmx::curve::plus<T, F> operator+(tmx::curve::base<T, F>& f, F s)
{
	tmx::curve::constant<T, F> g(s);
	std::cout << "value:" << std::endl;
	std::cout << g.value(0) << std::endl;
	tmx::curve::plus<T, F> p(f, g);
	std::cout << p.value(0) << std::endl;
	return p;
	//return tmx::curve::plus<T, F> (f, c);
}



#ifdef _DEBUG
inline int operator_test()
{	
	std::cout << "haha" << std::endl;
	double f = 2.0;
	tmx::curve::constant c1(1.0);
	tmx::curve::constant c2(3.0);

	tmx::curve::plus a1 = c1 + c2;
	tmx::curve::plus a2 = c1 + f;
	std::cout << "before" << std::endl;
	assert(a1.value(0) == 4.0);
	//assert(a2.value(0) == 3.0);
	std::cout << a1.value(0) << std::endl;
	std::cout << "after" << std::endl;

	return 0;
}
#endif // _DEBUG


}// namespace tmx::curve