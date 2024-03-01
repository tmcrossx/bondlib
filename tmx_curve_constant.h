#pragma once
#include "tmx_curve.h"

namespace tmx::curve {

	// Constant curve.
	template<class T = double, class F = double>
	class constant : public base<T, F> {
		F f;
	public:
		constexpr constant(F f = math::NaN<F>)
			: f(f)
		{ }

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
			{
				constant c(1);
				constant c2{ c };
				c = c2;

				assert(1 == c.forward(0));
				assert(0 == c.integral(0));
				//c.extrapolate(1);
				//c.back();
			}
			/*
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
			*/

			return 0;
		}
#endif // _DEBUG
	};

} // namespace tmx::curve
