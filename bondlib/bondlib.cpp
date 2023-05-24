// bondlib.cpp - test
#include "tmx_date.h"
#include "tmx_pwflat_curve.h"
#include "tmx_value.h"

int pwflat_integral_test = tmx::pwflat::test_pwflat_integral();
int value_yield_test_d = tmx::value::test_value_yield<double>();
int value_yield_test_f = tmx::value::test_value_yield<float>();

int main()
{
	return 0;
}