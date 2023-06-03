// xll_value.h - Value functions
#include "../bondlib/tmx_value.h"
#include "../bondlib/tmx_pwflat_curve.h"
#include "bondxll.h"

using namespace tmx;
using namespace xll;

AddIn xai_value_present(
	Function(XLL_DOUBLE, "xll_value_present", CATEGORY "VALUE.PRESENT")
	.Arguments({
		Arg(XLL_FPX, "time_cash", "is a two row array of times and cash flows."),
		Arg(XLL_HANDLEX, "curve", "is a handle to a curve."),
		})
	.Category(CATEGORY)
	.FunctionHelp("Return present value of cash flows using curve.")
);
double WINAPI xll_value_present(const _FPX* puc, HANDLEX curve)
{
#pragma XLLEXPORT
	double result = std::numeric_limits<double>::quiet_NaN();

	try {
		ensure(puc->rows == 2);
		handle<pwflat::curve<>> c_(curve);
		ensure(c_);

		auto m = puc->columns;
		result = value::present(m, puc->array, puc->array + m, c_->size(), c_->time(), c_->rate(), c_->extrapolate());
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return result;
}
