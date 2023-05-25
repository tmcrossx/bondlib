// bondlib.cpp - test
#include "tmx_date.h"
#include "tmx_pwflat_curve.h"
#include "tmx_value.h"
#include "tmx_bond.h"

using namespace tmx;

int pwflat_integral_test = pwflat::test_pwflat_integral();
int value_yield_test_d = value::test_value_yield<double>();
int value_yield_test_f = value::test_value_yield<float>();
int bond_basic_test = bond::test_bond_basic();

int main()
{

	return 0;
}