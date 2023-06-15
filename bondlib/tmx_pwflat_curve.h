// tmx_pwflat_curve.h - piecewise flat curve value type
#pragma once
#include <vector>
#include "ensure.h"
#include "tmx_pwflat.h"
#include "tmx_view.h"

namespace tmx::pwflat {

	// Pwflat curve value type.
	template<class T = double, class F = double>
	class curve : public curve_view<T,F> {
		std::vector<T> t;
		std::vector<F> f;
	public:
		using curve_view<T,F>::size;

		// constant curve
		constexpr curve(F _f = NaN<F>)
			: curve_view<T,F>(_f)
		{ }
		curve(size_t n, const T* t_, const F* f_, F _f = NaN<F>)
			: curve_view<T, F>(_f), t(t_, t_ + n), f(f_, f_ + n)
		{
			curve_view<T,F>::t = t.data();
			curve_view<T,F>::f = f.data();
			curve_view<T, F>::n = n;

			ensure(ok());
		}
		curve(const curve&) = default;
		curve& operator=(const curve&) = default;
		curve(curve&&) = default;
		curve& operator=(curve&&) = default;
		~curve()
		{ }

		bool ok() const
		{
			return t.size() == f.size() and curve_view<T, F>::ok();
		}

		// add point
		curve& extend(T t_, F f_)
		{
			ensure(size() == 0 || t_ > t.back());

			t.push_back(t_);
			f.push_back(f_);
			curve_view<T, F>::t = t.data();
			curve_view<T, F>::f = f.data();
			++curve_view<T, F>::n;

			return *this;
		}
		curve& push_back(const std::pair<T, F>& tf)
		{
			return extend(tf.first, tf.second);
		}

	};

#ifdef _DEBUG

	template<class X>
	inline int pwflat_curve_test()
	{
		X t[] = { 1, 2, 3 };
		X f[] = { .1, .2, .3 };
		/*
		{
			curve c(.1);
			ensure(0 == c.size());
			ensure(.1 == c.extrapolate());
			curve c2{ c };
			ensure(c2 == c);
			c = c2;
			ensure(!(c != c2));
			ensure(.1 == c(.2));
		}
		{
			curve c(3, t, f);
			
			ensure(3 == c.size());
			ensure(f[0] == c.value(1));
			ensure(std::isnan(c.extrapolate()));
			curve c2{ c };
			ensure(c2 == c);
			c = c2;
			ensure(!(c != c2));			

			ensure(std::isnan(c(-1)));
			for (size_t i = 0; i < 3; ++i) {
				ensure(f[i] == c(t[i]));
			}
			ensure(f[0] == c(0.5));
			ensure(f[1] == c(1.5));
			ensure(f[2] == c(2.5));
			ensure(std::isnan(c(3.5)));
		}
		{
			curve c(3, t, f);
			c.translate(0.5);
			ensure(f[0] == c(0.5));
			ensure(f[1] == c(1.5));
		}
		*/
		return 0;
	}

#endif // _DEBUG

}