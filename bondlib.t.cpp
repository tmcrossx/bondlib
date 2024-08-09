// bondlib.t.cpp - test bondlib
#include <cassert>
//#include "tmx_monoid.h"
#include "math/tmx_math_hypergeometric.h"
#include "date/tmx_date_periodic.h"
#include "variate/tmx_variate_normal.h"
#include "valuation/tmx_option.h"
#include "valuation/tmx_valuation.h"
#include "date/tmx_date_business_day.h"
#include "curve/tmx_curve_pwflat.h"
#include "curve/tmx_curve.h"
#include "instrument/tmx_instrument.h"
#include "valuation/tmx_valuation.h"
#include "instrument/tmx_instrument_bond.h"
#include "curve/tmx_curve_bootstrap.h"
//#include "tmx_muni.h"
 
using namespace fms;
using namespace tmx;

#ifdef _DEBUG

int test_hypergeometric = math::hypergeometric_test();

// variate/valuation
int test_variate_normal = variate::normal<>::test();
int test_option_put = option::put::test();

int test_date_periodic = date::periodic_test();

int test_curve_constant = curve::constant_test();
int test_curve_bump = curve::bump_test();
int test_bump = curve::bump_test();
int test_translate = curve::translate_test();

int test_pwflat = curve::pwflat_test();
//int test_tmx_monotonic = tmx::monotonic_test();
//int test_pwflat_curve_view = pwflat::view<>::test();
//int test_pwflat_curve_value = pwflat::interface<>::test();
//int test_instrument_zcb = instrument::iterable_test();
//int test_instrument_iterable = instrument::iterable_test();
//int test_instrument_view = view<>::test();
//int test_instrument_value = value<>::test();
//int test_valuation_yield_d = valuation::yield_test<double>();
//int test_value_yield_f = value::yield_test<float>();
int test_instrument_bond_basic = instrument::bond::basic_test();
//int test_muni_fit = muni::fit_test();
#endif // _DEBUG

int bootstrap_test()
{
	{
		double u[] = { 1, 2, 3 };
		double c[] = { 0.01, 0.02, 0.03 };

		curve::pwflat<> f;
		curve::constant<> c0(0.02);
		auto i1 = instrument::iterable(take(array(u), 1), take(array(c), 1));
		double p1, p2;
		p1 = valuation::present(i1, curve::constant(0.01));
		p2 = valuation::present(i1, curve::constant(0.02));
		double t = 0;
		auto pv = [i1, &f, t](double r) { return valuation::present(i1, curve::extrapolate(f, t, r)); };
		p1 = pv(0.01);
		p2 = pv(0.02);
		auto uc1 = curve::bootstrap0(i1, f, 0., .01, valuation::present(i1, c0));
		f.push_back(uc1);

		auto i2 = instrument::iterable(take(array(u), 2), take(array(c), 2));
		auto uc2 = curve::bootstrap0(i2, f, 1., .01, valuation::present(i2, c0));
		f.push_back(uc2);

		auto i3 = instrument::iterable(take(array(u), 3), take(array(c), 3));
		auto uc3 = curve::bootstrap0(i3, f, 2., .01, valuation::present(i3, c0));
		f.push_back(uc3);

		assert(f.time() == std::vector<double>({ 1,2,3 }));
		for (int i = 0; i < 3; ++i) {
			assert(fabs(f.rate()[i] - 0.02) <= 44*math::sqrt_epsilon<double>);
		}
	}
	return 0;
}
int test_bootstrap = bootstrap_test();

int main()
{
	
	return 0;
}
