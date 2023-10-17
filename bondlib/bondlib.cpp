// bondlib.cpp - test
#include <cassert>
#include "fms_iterable.h"
//#include "tmx_monoid.h"
#include "tmx_view.h"
#include "tmx_date.h"
#include "tmx_pwflat_curve.h"
#include "tmx_instrument_value.h"
#include "tmx_value.h"
#include "tmx_bond.h"
#include "tmx_bootstrap.h"
//#include "tmx_muni.h"

using namespace fms;
using namespace tmx;

#ifdef _DEBUG
int test_iterable_ptr = iterable_ptr<int>::test();
int test_iterator = iterable_iterator<std::vector<int>::iterator>::test();
//int test_monoid_i = monoid_test<int>();
//int test_monoid_d = monoid_test<double>();
//int test_mean_monoid_d = mean_monoid_test<double>();
//int test_bound = root1d::bound_test<double>();
int test_years = date::years_test();
int test_dcf_30_360 = date::dcf_30_360_test();
int test_view_d = view<double>::test();
int test_view_iterable_d = view_iterable<double>::test();
int test_tmx_monotonic = tmx::monotonic_test();
int test_pwflat_curve_view = pwflat::curve_view<>::test();
int test_pwflat_curve_value = pwflat::curve<>::test();
int test_instrument_view = instrument_view<>::test();
int test_instrument_value = instrument_value<>::test();
int test_value_yield_d = value::yield_test<double>();
int test_value_yield_f = value::yield_test<float>();
int test_bond_basic = bond::bond_basic_test();
int test_bootstrap_instrument = bootstrap::instrument_test();
//int test_muni_fit = muni::fit_test();
#endif // _DEBUG

int main()
{

	return 0;
}
