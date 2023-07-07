// xll_pwflat.cpp - Piecewise flat curve view
#include "../bondlib/tmx_pwflat.h"
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
	.FunctionHelp("Return a handle to a piecewise flat forward curve.")
);
HANDLEX WINAPI xll_pwflat_curve_(const _FPX* pt, const _FPX* pf, LPOPER p_f)
{
#pragma XLLEXPORT

	HANDLEX h = INVALID_HANDLEX;

	try {
		auto m = size(*pt);
		ensure(m == size(*pf));
		double _f = std::numeric_limits<double>::quiet_NaN();
		if (*p_f) {
			ensure(p_f->is_num());
			_f = p_f->as_num();
		}
		// lasts 2 times equal use last f to extrapolate
		if (m >= 2 and pt->array[m-1] == pt->array[m-2]) {
			--m;
			_f = pf->array[m];
		}
		handle<pwflat::curve<>> h_(new pwflat::curve_value(m, pt->array, pf->array, _f));
		ensure(h_);
		h = h_.get();
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
	.FunctionHelp("Return a two row array of times and rates. "
	"The last time is duplicated and the last rate is the extrapolation.")
);
_FPX* WINAPI xll_pwflat_curve(HANDLEX c)
{
#pragma XLLEXPORT
	static FPX result;

	try {
		result.resize(0, 0);
		handle<pwflat::curve<>> c_(c);
		ensure(c_);

		auto pc = c_.as<pwflat::curve_view<>>();
		ensure(pc);
		
		int m = (int)pc->size();
		result.resize(2, m + 1);
		view<double> t = pc->t;
		view<double> f = pc->f;
		std::copy(t.begin(), t.end(), result.array());
		std::copy(f.begin(), f.end(), result.array() + m + 1);

		result(0, m) = result(0, m - 1);
		result(1, m) = pc->extrapolate();
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return result.get();
}

AddIn xai_pwflat_curve_shift_(
	Function(XLL_HANDLEX, "xll_pwflat_curve_shift_", "\\" CATEGORY ".PWFLAT.CURVE.SHIFT")
	.Arguments({
		Arg(XLL_HANDLEX, "curve", "is handle to a curve."),
		Arg(XLL_DOUBLE, "spread", "is the spread.")
		})
	.Uncalced()
	.Category(CATEGORY)
	.FunctionHelp("Return a shifted curve.")
);
HANDLEX WINAPI xll_pwflat_curve_shift_(HANDLEX c, double s)
{
#pragma XLLEXPORT
	double result = INVALID_HANDLEX;

	try {
		handle<pwflat::curve<>> c_(c);
		ensure(c_);
		handle<pwflat::curve<>> _c(new pwflat::curve_value(*(c_.as<pwflat::curve_view<>>())));
		ensure(_c);

		_c->shift(s);

		result = _c.get();
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return result;
}

#if 0
AddIn xai_pwflat_curve_translate_(
	Function(XLL_HANDLEX, "xll_pwflat_curve_translate_", "\\" CATEGORY ".PWFLAT.CURVE.TRANSLATE")
	.Arguments({
		Arg(XLL_HANDLEX, "curve", "is handle to a curve."),
		Arg(XLL_DOUBLE, "time", "is the time in years to translate the curve.")
		})
	.Uncalced()
	.Category(CATEGORY)
	.FunctionHelp("Return a two row array of times and rates.")
);
HANDLEX WINAPI xll_pwflat_curve_translate_(HANDLEX c, double u)
{
#pragma XLLEXPORT
	double result = std::numeric_limits<double>::quiet_NaN();

	try {
		handle<pwflat::curve_view<>> c_(c);
		ensure(c_);
		handle<pwflat::curve_view<>> _c(new pwflat::curve_value(*c_));
		ensure(_c);
		_c->translate(u);

		result = _c.get();
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return result;
}
#endif // 0

AddIn xai_pwflat_curve_value(
	Function(XLL_DOUBLE, "xll_pwflat_curve_value", CATEGORY ".PWFLAT.CURVE.FORWARD")
	.Arguments({
		Arg(XLL_HANDLEX, "c", "is a handle to a curve."),
		Arg(XLL_DOUBLE, "t", "is the forward time."),
		Arg(XLL_DOUBLE, "_t", "is the time at which to evaluate the yield. Default is 0."),
		})
	.Category(CATEGORY)
	.FunctionHelp("Return the value of a piecewise flat curve.")
);
HANDLEX WINAPI xll_pwflat_curve_value(HANDLEX c, double t, double _t)
{
#pragma XLLEXPORT

	double v = std::numeric_limits<double>::quiet_NaN();

	try {
		handle<pwflat::curve<>> c_(c);
		ensure(c_);

		v = c_->forward(t + _t);
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return v;
}

AddIn xai_pwflat_curve_yield(
	Function(XLL_DOUBLE, "xll_pwflat_curve_yield", CATEGORY ".PWFLAT.CURVE.YIELD")
	.Arguments({
		Arg(XLL_HANDLEX, "c", "is a handle to a piecewise flat curve."),
		Arg(XLL_DOUBLE, "t", "is the yield time."),
		Arg(XLL_DOUBLE, "_t", "is the time at which to evaluate the yield. Default is 0."),
		})
	.Category(CATEGORY)
	.FunctionHelp("Return the yield value of a piecewise flat curve.")
);
HANDLEX WINAPI xll_pwflat_curve_yield(HANDLEX c, double t, double _t)
{
#pragma XLLEXPORT

	double v = std::numeric_limits<double>::quiet_NaN();

	try {
		handle<pwflat::curve<>> c_(c);
		ensure(c_);

		v = c_->yield(t + _t, _t);
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
		Arg(XLL_DOUBLE, "t", "is the discount time."),
		Arg(XLL_DOUBLE, "_t", "is the time at which to evaluate the discount. Default is 0."),
		})
	.Category(CATEGORY)
	.FunctionHelp("Return the discount value of a piecewise flat curve.")
);
HANDLEX WINAPI xll_pwflat_curve_discount(HANDLEX c, double t, double _t)
{
#pragma XLLEXPORT

	double v = std::numeric_limits<double>::quiet_NaN();

	try {
		handle<pwflat::curve<>> c_(c);
		ensure(c_);

		v = c_->discount(t + _t, _t);
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return v;
}
