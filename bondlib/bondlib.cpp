// bondlib.cpp - test
#include <cassert>
#include "tmx_iterable.h"
#include "tmx_monoid.h"
#include "tmx_view.h"
#include "tmx_date.h"
#include "tmx_pwflat_view.h"
#include "tmx_pwflat_value.h"
#include "tmx_value.h"
#include "tmx_bond.h"
#include "tmx_bootstrap.h"
//#include "tmx_muni.h"

using namespace tmx;

#ifdef _DEBUG
int test_monoid_i = monoid_test<int>();
int test_monoid_d = monoid_test<double>();
int test_mean_monoid_d = mean_monoid_test<double>();
int test_years = date::years_test();
int test_dcf_30_360 = date::dcf_30_360_test();
int test_view_d = view<double>::test();
int test_pwflat_monotonic = pwflat::monotonic_test();
int test_pwflat_integral = pwflat::integral_test();
int test_pwflat_discount = pwflat::discount_test();
int test_pwflat_curve_value_d = pwflat::curve_value<double,double>::test();
int test_pwflat_curve_view = pwflat::curve_view<double,double>::test();
int test_instrument_value = instrument_value<>::test();
int test_value_yield_d = value::yield_test<double>();
int test_value_yield_f = value::yield_test<float>();
int test_bond_basic = bond::bond_basic_test();
//int test_muni_fit = muni::fit_test();
#endif // _DEBUG

int main()
{

	return 0;
}
