// bondlib.cpp - test
#include <cassert>
#include "tmx_view.h"
#include "tmx_date.h"
#include "tmx_pwflat_curve.h"
#include "tmx_value.h"
#include "tmx_bond.h"
#include "tmx_bootstrap.h"
//#include "tmx_muni.h"

using namespace tmx;

#ifdef _DEBUG
int test_value_yield_d = value::yield_test<double>();
int test_value_yield_f = value::yield_test<float>();
int test_pwflat_integral = pwflat::pwflat_integral_test();
int test_span_translate_d = view_test<double>();
int test_tmx_translate_d = translate_<double>::test();
int test_pwflat_curve_d = pwflat::pwflat_curve_test<double>();
int test_bond_basic = bond::bond_basic_test();
//int test_muni_fit = muni::fit_test();
#endif // _DEBUG

int main()
{

	return 0;
}
