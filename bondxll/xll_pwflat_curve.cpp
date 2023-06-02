// xll_pwflat_curve.cpp - Piecewise flat curve
#include "../bondlib/tmx_pwflat_curve.h"
#include "bondxll.h"

using namespace tmx;
using namespace xll;

AddIn xai_pwflat_curve(
	Function(XLL_HANDLEX, "xll_pwflat_curve", "\\PWFLAT.CURVE")
	.Arguments({
		Arg(XLL_FPX, "t", "is an array of positive increasing times."),
		Arg(XLL_FPX, "f", "is an array of corresponding rates."),
		Arg(XLL_LPOPER, "_f", "is an optional extrapolation rate."),
		})
	.Uncalced()
	.Category(CATEGORY)
	.FunctionHelp("Return a handle to a piecewise flat curve.")
);
HANDLEX WINAPI xll_pwflat_curve(const _FPX* pt, const _FPX* pf, LPOPER p_f)
{
#pragma XLLEXPORT

	HANDLEX h = INVALID_HANDLEX;

	try {
		ensure(size(*pt) == size(*pf));
		double _f = std::numeric_limits<double>::quiet_NaN();
		if (!p_f->is_missing()) {
			ensure(p_f->is_num());
			_f = p_f->as_num();
		}
		handle<pwflat::curve<>> h_(new pwflat::curve(size(*pt), pt->array, pf->array, _f));
		ensure(h_);
		h = h_.get();
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return h;
}

AddIn xai_pwflat_curve_value(
	Function(XLL_DOUBLE, "xll_pwflat_curve_value", "PWFLAT.CURVE.VALUE")
	.Arguments({
		Arg(XLL_HANDLEX, "c", "is a handle to a piecewise flat curve."),
		Arg(XLL_DOUBLE, "t", "is the time at which to value the curve."),
		})
	.Category(CATEGORY)
	.FunctionHelp("Return the value of a piecewise flat curve.")
);
HANDLEX WINAPI xll_pwflat_curve_value(HANDLEX c, double t)
{
#pragma XLLEXPORT

	double v = std::numeric_limits<double>::quiet_NaN();

	try {
		handle<pwflat::curve<>> c_(c);
		ensure(c_);

		v = c_->value(t);
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return v;
}

AddIn xai_pwflat_curve_spot(
	Function(XLL_DOUBLE, "xll_pwflat_curve_spot", "PWFLAT.CURVE.SPOT")
	.Arguments({
		Arg(XLL_HANDLEX, "c", "is a handle to a piecewise flat curve."),
		Arg(XLL_DOUBLE, "t", "is the time at which to value the spot rate of the curve."),
		})
	.Category(CATEGORY)
	.FunctionHelp("Return the spot value of a piecewise flat curve.")
);
HANDLEX WINAPI xll_pwflat_curve_spot(HANDLEX c, double t)
{
#pragma XLLEXPORT

	double v = std::numeric_limits<double>::quiet_NaN();

	try {
		handle<pwflat::curve<>> c_(c);
		ensure(c_);

		v = c_->spot(t);
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return v;
}

AddIn xai_pwflat_curve_discount(
	Function(XLL_DOUBLE, "xll_pwflat_curve_discount", "PWFLAT.CURVE.discount")
	.Arguments({
		Arg(XLL_HANDLEX, "c", "is a handle to a piecewise flat curve."),
		Arg(XLL_DOUBLE, "t", "is the time at which to value the discount of the curve."),
		})
	.Category(CATEGORY)
	.FunctionHelp("Return the discount value of a piecewise flat curve.")
);
HANDLEX WINAPI xll_pwflat_curve_discount(HANDLEX c, double t)
{
#pragma XLLEXPORT

	double v = std::numeric_limits<double>::quiet_NaN();

	try {
		handle<pwflat::curve<>> c_(c);
		ensure(c_);

		v = c_->discount(t);
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return v;
}
