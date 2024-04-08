// tmx_curve_pwflat.h - Piecewise flat forward curve
#pragma once
#include "tmx_math.h"
#include "tmx_pwflat.h"
#include "tmx_curve.h"
#include <iostream>

namespace tmx::curve {

	template<class T = double, class F = double>
	class pwflat : public base<T, F> {
		std::vector<T> t_;
		std::vector<F> f_;
		F _f; // extrapolation value
	public:
		// constant curve
		constexpr pwflat(F _f = math::NaN<F>)
			: _f(_f)
		{ }
		pwflat(size_t n, const T* t_, const F* f_, F _f = math::NaN<F>)
			: t_(t_, t_ + n), f_(f_, f_ + n), _f(_f)
		{ }
		pwflat(const pwflat&) = default;
		pwflat& operator=(const pwflat&) = default;
		pwflat(pwflat&&) = default;
		pwflat& operator=(pwflat&&) = default;
		~pwflat() = default;

		F _forward(T u, T t) const override
		{
			return tmx::pwflat::forward(u - t, t_.size(), t_.data(), f_.data(), _f);
		}
		F _integral(T u, T t0) const override
		{
			return tmx::pwflat::integral(u, t_.size(), t_.data(), f_.data(), _f)
				 - tmx::pwflat::integral(t0, t_.size(), t_.data(), f_.data(), _f);
		}
	};

#ifdef _DEBUG
	inline int pwflat_test()
	{
		{
			// TODO: (Tianxin)
			std::cout << "pwflat" << std::endl;
			pwflat<> c; // default constructor
			auto c2(c); // copy constructor
			c2 = c;     // copy assignment
			std::cout << "constructor" << std::endl;
		}
		{
			// test one case for _value, _integral, _extrapolate, and _back.
			pwflat<> p1(3);
			assert(3.0 == p1.forward(0));
			assert(15 == p1.integral(5, 0));
			//p1.extrapolate(7);
			//assert(7.0 == p1.value(0));
			//assert(21 == p1.integral(3, 0));

			//auto back = p1.back();
			//assert(back.first);
			//assert(3.0 == back.second);
			//std::cout << back.second << std::endl;
			//std::cout << p.integral(5, 0) << std::endl;
		}

		return 0;
	}
#endif // _DEBUG
} // namespace tms::curve1