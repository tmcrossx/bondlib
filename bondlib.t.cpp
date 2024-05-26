// bondlib.t.cpp - test bondlib
#include <cassert>
//#include "tmx_monoid.h"
#include "fms_iterable/fms_iterable.h"
#include "tmx_math_hypergeometric.h"
#include "tmx_date.h"
#include "tmx_variate_normal.h"
#include "tmx_option.h"
#include "tmx_date_business_day.h"
#include "tmx_curve_pwflat.h"
#include "tmx_curve.h"
#include "tmx_instrument.h"
#include "tmx_valuation.h"
#include "tmx_instrument_bond.h"
//#include "tmx_bootstrap.h"
//#include "tmx_muni.h"
 
using namespace fms;
using namespace tmx;

#ifdef _DEBUG

int test_sd = math::sd_test();

int test_hypergeometric = math::hypergeometric_test();

int test_curve_constant = curve::constant_test();
int test_exponential = curve::exponential_test();
int test_date = date::test();
//int test_variate_normal = variate::normal<>::test();
//<<<<<<< main
//int test_black_put = black::put::test();
//int test_curve_operator = curve::constant<>::test();
//int test_pwflat = curve::pwflat_test();
//int test_option_put = option::put::test();
//>>>>>>> main
int test_from_time_t = date::from_time_t_test();
int test_date_periodic = date::periodic::test();
//int test_datetime = datetime::test();
//int test_date_dcf = date::dcf::test();
//int test_tmx_monotonic = tmx::monotonic_test();
//int test_pwflat_curve_view = pwflat::view<>::test();
//int test_pwflat_curve_value = pwflat::interface<>::test();
int test_tmx_curve_constant = curve::constant_test();
int test_tmx_curve_bump = curve::bump_test();
int test_instrument_zcb = instrument::iterable_test();
int test_instrument_iterable = instrument::iterable_test();
//int test_instrument_view = view<>::test();
//int test_instrument_value = value<>::test();
int test_valuation_yield_d = valuation::yield_test<double>();
//int test_value_yield_f = value::yield_test<float>();
int test_bond_basic = bond::basic_test();
//int test_bootstrap_instrument = bootstrap::instrument_test();
//int test_muni_fit = muni::fit_test();
#endif // _DEBUG

int main()
{
	
	return 0;
}
