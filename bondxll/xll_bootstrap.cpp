// xll_bootstrap.cpp - Bootstrap a curve from bonds
#include "../bondlib/tmx_pwflat_curve.h"
#include "../bondlib/tmx_instrument.h"
#include "../bondlib/tmx_bootstrap.h"
#include "bondxll.h"

using namespace tmx;
using namespace xll;

AddIn xai_tmx_bootstrap(
	Function(XLL_HANDLEX, "xll_tmx_bootstrap", "\\" CATEGORY ".BOOTSTRAP")
	.Arguments({
		Arg(XLL_FPX, "instruments", "is an array of fixed income instrument handles."),
		Arg(XLL_FPX, "prices", "is an array of instrument prices."),
		})
	.Uncalced()
	.Category(CATEGORY)
	.FunctionHelp("Return a handle to a curve repricing instruments.")
);
HANDLEX WINAPI xll_tms_bootstrap(_FPX* pi, _FPX* pp)
{
#pragma XLLEXPORT
	HANDLEX result = INVALID_HANDLEX;

	try {
		ensure(size(*pi) == size(*pp));
		handle<pwflat::curve<>> c_(new pwflat::curve<>{});
		ensure(c_);
		for (size_t i = 0; i < size(*pi); ++i) {
			handle<instrument<>> ii(pi->array[i]);
			ensure(ii);
			c_->push_back(bootstrap::instrument<>(
				ii->size(), ii->time(), ii->cash(),
				c_->size(), c_->time(), c_->rate(), pp->array[i]));
		}
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return result;
}