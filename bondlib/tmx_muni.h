// tmx_muni.h - Municipal bond curves
#pragma once
#include "tmx_pwflat_curve.h"
#include "tmx_bond.h"
#include "tmx_value.h"

namespace tmx::muni {

	// Forward curve that reprices standard muni curve.
	template<class T, class F>
	inline pwflat::curve<T, F> fit(size_t n, const T* t, const F* y,
		F eps = std::sqrt(std::numeric_limits<F>::quiet_NaN()))
	{
		curve::curve r(n, t, y);
		auto D = [&r](T t) { return r.discount(t); };

		for (unsigned i = 0; i < 10; ++i) {
			bond::basic bi(date::ymd{}, (unsigned)t[i], 0.05);
			F pi = bi.price(r[i]);
			F r_ = r[i] + 2 * eps;
			while (std::fabs(r_ - r[i]) > eps) {
				F p_ = bi.present_value(bi.dated, D) - pi;
				F dp = bi.duration(bi.dated, D);
				r_ = r[i];
				r[i] -= p_ / dp;
			}
		}

		return r;
	}

#ifdef _DEBUG

	inline int fit_test()
	{
		{
			double t[] = { 1,2,3 };
			double y[] = { 0.03, 0.04, 0.05 };
			auto f = fit(3, t, y);
		}

		return 0;
	}

#endif // _DEBUG

}
