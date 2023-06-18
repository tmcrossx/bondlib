// tmx_pwflat_value.h - piecewise flat curve value type
#pragma once
#include <vector>
#include "ensure.h"
#include "tmx_pwflat_view.h"

namespace tmx::pwflat {

	// Pwflat curve value type.
	template<class T = double, class F = double>
	class curve_value : public curve_view<T,F> {
		std::vector<T> t;
		std::vector<F> f;
	public:
		using curve_view<T,F>::size;

		// constant curve
		constexpr curve_value(F _f = NaN<F>)
			: curve_view<T,F>(_f)
		{ }
		curve_value(size_t n, const T* t_, const F* f_, F _f = NaN<F>)
			: curve_view<T, F>(_f), t(t_, t_ + n), f(f_, f_ + n)
		{
			curve_view<T,F>::t = t.data();
			curve_view<T,F>::f = f.data();
			curve_view<T, F>::n = n;

			ensure(ok());
		}
		curve_value(const curve_view<T,F>& c)
			: curve_value(c.size(), c.time(), c.rate())
		{ }
		curve_value(const curve_value&) = default;
		curve_value& operator=(const curve_value&) = default;
		curve_value(curve_value&&) = default;
		curve_value& operator=(curve_value&&) = default;
		~curve_value()
		{ }

		bool ok() const
		{
			return t.size() == f.size() and curve_view<T, F>::ok();
		}

		// add point
		curve_value& extend(T t_, F f_)
		{
			ensure(size() == 0 || t_ > t.back());

			t.push_back(t_);
			f.push_back(f_);
			curve_view<T, F>::t = t.data();
			curve_view<T, F>::f = f.data();
			++curve_view<T, F>::n;

			return *this;
		}
		curve_value& push_back(const std::pair<T, F>& tf)
		{
			return extend(tf.first, tf.second);
		}
#ifdef _DEBUG

		static int test()
		{
			T t[] = { 1, 2, 3 };
			F f[] = { .1, .2, .3 };
			/*
			{
				curve_value c(.1);
				ensure(0 == c.size());
				ensure(.1 == c.extrapolate());
				curve_value c2{ c };
				ensure(c2 == c);
				c = c2;
				ensure(!(c != c2));
				ensure(.1 == c(.2));
			}
			{
				curve_value c(3, t, f);

				ensure(3 == c.size());
				ensure(f[0] == c.value(1));
				ensure(std::isnan(c.extrapolate()));
				curve_value c2{ c };
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
				curve_value c(3, t, f);
				c.translate(0.5);
				ensure(f[0] == c(0.5));
				ensure(f[1] == c(1.5));
			}
			*/
			return 0;
		}

#endif // _DEBUG

	};


}