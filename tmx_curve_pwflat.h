// tmx_curve_pwflat.h - Piecewise flat forward curve
#pragma once
#include "tmx_math.h"
#include "tmx_pwflat.h"
#include "tmx_curve.h"

namespace tmx::curve {

	template<class T = double, class F = double>
	class pwflat : public base<T, F> {
		std::vector<T> t;
		std::vector<F> f;
		F _f; // extrapolation value
	public:
		// constant curve
		constexpr pwflat(F _f = math::NaN<F>)
			: _f(_f)
		{ }
		pwflat(size_t n, const T* t_, const F* f_, F _f = math::NaN<F>)
			: t(t_, t_ + n), f(f_, f_ + n), _f(_f)
		{ }
		pwflat(const pwflat&) = default;
		pwflat& operator=(const pwflat&) = default;
		pwflat(pwflat&&) = default;
		pwflat& operator=(pwflat&&) = default;
		~pwflat() = default;

		F _value(T u) const override
		{
			return pwflat::value(u, t.size(), t.data(), f.data(), _f);
		}
		F _integral(T u, T t) const override
		{
			return pwflat::integral(u, t.size(), t.data(), f.data(), _f)
				 - pwflat::integral(t, t.size(), t.data(), f.data(), _f);
		}
		pwflat& _extrapolate(F f) override
		{
			_f = f;
		}

		// Last point on the curve.
		std::pair<T, F> _back() const override
		{
			return { t.back(), f.back() };
		}
#ifdef _DEBUG
		static int test()
		{
			{
				// TODO: (Tianxin)
				pwflat<> c; // default constructor
				auto c2(c); // copy constructor
				c2 = c;     // copy assignment
			}
			{
				// test one case for _value, _integral, _extrapolate, and _back.
			}

			return 0;
		}
#endif // _DEBUG

	};

} // namespace tms::curve