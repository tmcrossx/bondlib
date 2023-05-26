// bondlib.cpp - test
#include "tmx_date.h"
#include "tmx_pwflat_curve.h"
#include "tmx_value.h"
#include "tmx_bond.h"

using namespace tmx;

int test_value_yield_d = value::value_yield_test<double>();
int test_value_yield_f = value::value_yield_test<float>();
int test_pwflat_integral = pwflat::pwflat_integral_test();
int test_pwflat_curve_d = pwflat::pwflat_curve_test<double>();
int test_bond_basic = bond::bond_basic_test();

int main()
{

	return 0;
}