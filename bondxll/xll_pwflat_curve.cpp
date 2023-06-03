// xll_pwflat_curve.cpp - Piecewise flat curve
#include "../bondlib/tmx_pwflat_curve.h"
#include "bondxll.h"

using namespace tmx;
using namespace xll;

AddIn xai_pwflat_curve_(
	Function(XLL_HANDLEX, "xll_pwflat_curve_", "\\" CATEGORY ".PWFLAT.CURVE")
	.Arguments({
		Arg(XLL_FPX, "t", "is an array of positive increasing times."),
		Arg(XLL_FPX, "f", "is an array of corresponding rates."),
		Arg(XLL_LPOPER, "_f", "is an optional extrapolation rate."),
		})
	.Uncalced()
	.Category(CATEGORY)
	.FunctionHelp("Return a handle to a piecewise flat curve.")
);
HANDLEX WINAPI xll_pwflat_curve_(const _FPX* pt, const _FPX* pf, LPOPER p_f)
{
#pragma XLLEXPORT

	HANDLEX h = INVALID_HANDLEX;

	try {
		if (size(*pt) == 1) { // handle
			handle<pwflat::curve<>> c_(pt->array[0]);
			ensure(c_);
			handle<pwflat::curve<>> h_(new pwflat::curve(*c_));
			ensure(h_);

			h = h_.get();
		}
		else {
			ensure(size(*pt) == size(*pf));
			double _f = std::numeric_limits<double>::quiet_NaN();
			if (*p_f) {
				ensure(p_f->is_num());
				_f = p_f->as_num();
			}
			handle<pwflat::curve<>> h_(new pwflat::curve(size(*pt), pt->array, pf->array, _f));
			ensure(h_);
			h = h_.get();
		}
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return h;
}

AddIn xai_pwflat_curve(
	Function(XLL_FPX, "xll_pwflat_curve", CATEGORY ".PWFLAT.CURVE")
	.Arguments({
		Arg(XLL_HANDLEX, "curve", "is handle to a curve."),
		})
	.Category(CATEGORY)
	.FunctionHelp("Return a two row array of times and rates.")
);
_FPX* WINAPI xll_pwflat_curve(HANDLEX c)
{
#pragma XLLEXPORT
	static FPX result;

	try {
		result.resize(0, 0);
		handle<pwflat::curve<>> c_(c);
		ensure(c_);
		int m = (int)c_->size();
		result.resize(2, m + 1);
		std::copy(c_->time(), c_->time() + m, result.array());
		std::copy(c_->rate(), c_->rate() + m, result.array() + m + 1);
		result(0, m) = result(0, m - 1);
		result(1, m) = c_->extrapolate();
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return result.get();
}

AddIn xai_pwflat_curve_shift(
	Function(XLL_HANDLEX, "xll_pwflat_curve_shift", CATEGORY ".PWFLAT.CURVE.SHIFT")
	.Arguments({
		Arg(XLL_HANDLEX, "curve", "is handle to a curve."),
		Arg(XLL_DOUBLE, "rate", "is the rate to shift the curve.")
		})
	.Category(CATEGORY)
	.FunctionHelp("Return a two row array of times and rates.")
);
HANDLEX WINAPI xll_pwflat_curve_shift(HANDLEX c, double f)
{
#pragma XLLEXPORT
	double result = std::numeric_limits<double>::quiet_NaN();

	try {
		handle<pwflat::curve<>> c_(c);
		ensure(c_);
		c_->shift(f);
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return c;
}

AddIn xai_pwflat_curve_translate(
	Function(XLL_HANDLEX, "xll_pwflat_curve_translate", CATEGORY ".PWFLAT.CURVE.TRANSLATE")
	.Arguments({
		Arg(XLL_HANDLEX, "curve", "is handle to a curve."),
		Arg(XLL_DOUBLE, "time", "is the time in years to translate the curve.")
		})
	.Category(CATEGORY)
	.FunctionHelp("Return a two row array of times and rates.")
);
HANDLEX WINAPI xll_pwflat_curve_translate(HANDLEX c, double u)
{
#pragma XLLEXPORT
	double result = std::numeric_limits<double>::quiet_NaN();

	try {
		handle<pwflat::curve<>> c_(c);
		ensure(c_);
		c_->translate(u);
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return c;
}

AddIn xai_pwflat_curve_value(
	Function(XLL_DOUBLE, "xll_pwflat_curve_value", CATEGORY ".PWFLAT.CURVE.VALUE")
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
	Function(XLL_DOUBLE, "xll_pwflat_curve_discount", CATEGORY ".PWFLAT.CURVE.DISCOUNT")
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
