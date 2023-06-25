// xll_bootstrap.cpp - Bootstrap a curve from bonds
#include "../bondlib/tmx_pwflat.h"
#include "../bondlib/tmx_instrument.h"
#include "../bondlib/tmx_bootstrap.h"
#include "bondxll.h"

using namespace tmx;
using namespace xll;

AddIn xai_tmx_bootstrap_(
	Function(XLL_HANDLEX, "xll_tmx_bootstrap_", "\\" CATEGORY ".BOOTSTRAP")
	.Arguments({
		Arg(XLL_FPX, "instruments", "is an array of fixed income instrument handles."),
		Arg(XLL_FPX, "prices", "is an array of instrument prices."),
		})
		.Uncalced()
	.Category(CATEGORY)
	.FunctionHelp("Return a handle to a curve repricing instruments.")
);
HANDLEX WINAPI xll_tmx_bootstrap_(_FPX* pi, _FPX* pp)
{
#pragma XLLEXPORT
	HANDLEX result = INVALID_HANDLEX;

	try {
		ensure(size(*pi) == size(*pp));

		pwflat::curve_value<> f;
		for (size_t i = 0; i < size(*pi); ++i) {
			handle<instrument<>> ii(pi->array[i]);
			ensure(ii);
			f.push_back(bootstrap::instrument<>(*ii, f, pp->array[i]));
		}

		handle<pwflat::curve<>> c_(new pwflat::curve_value<>(f));
		ensure(c_);
		result = c_.get();
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return result;
}