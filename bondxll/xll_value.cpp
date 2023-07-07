// xll_value.h - Value functions
#include "../bondlib/tmx_value.h"
#include "bondxll.h"

using namespace tmx;
using namespace xll;

AddIn xai_value_present(
	Function(XLL_DOUBLE, "xll_value_present", CATEGORY ".VALUE.PRESENT")
	.Arguments({
		Arg(XLL_HANDLEX, "instrument", "is a handle to an instrument."),
		Arg(XLL_HANDLEX, "curve", "is a handle to a curve."),
		Arg(XLL_DOUBLE, "t", "is an optional time in years at which to compute the present value. Default is 0."),
		})
	.Category(CATEGORY)
	.FunctionHelp("Return present value of instrument cash flows using curve at time t.")
);
double WINAPI xll_value_present(HANDLEX inst, HANDLEX curve, double t)
{
#pragma XLLEXPORT
	double result = std::numeric_limits<double>::quiet_NaN();

	try {
		handle<instrument<>> i_(inst);
		ensure(i_);
		handle<pwflat::curve<>> c_(curve);
		ensure(c_);
		result = value::present(*i_, *c_, t);
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return result;
}

AddIn xai_value_duration(
	Function(XLL_DOUBLE, "xll_value_duration", CATEGORY ".VALUE.DURATION")
	.Arguments({
		Arg(XLL_HANDLEX, "instrument", "is a handle to an instrument."),
		Arg(XLL_HANDLEX, "curve", "is a handle to a curve."),
		Arg(XLL_DOUBLE, "t", "is an optional time in years at which to compute the duration. Default is 0."),
		})
		.Category(CATEGORY)
	.FunctionHelp("Return duration of cash flows using curve.")
);
double WINAPI xll_value_duration(HANDLEX inst, HANDLEX curve, double t)
{
#pragma XLLEXPORT
	double result = std::numeric_limits<double>::quiet_NaN();

	try {
		handle<instrument<>> i_(inst);
		ensure(i_);
		handle<pwflat::curve<>> c_(curve);
		ensure(c_);
		result = value::duration(*i_, *c_, t);
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return result;
}

AddIn xai_value_convexity(
	Function(XLL_DOUBLE, "xll_value_convexity", CATEGORY ".VALUE.CONVEXITY")
	.Arguments({
		Arg(XLL_HANDLEX, "instrument", "is a handle to an instrument."),
		Arg(XLL_HANDLEX, "curve", "is a handle to a curve."),
		Arg(XLL_DOUBLE, "t", "is an optional time in years at which to compute the convexity. Default is 0."),
		})
		.Category(CATEGORY)
	.FunctionHelp("Return convexity of cash flows using curve.")
);
double WINAPI xll_value_convexity(HANDLEX inst, HANDLEX curve, double t)
{
#pragma XLLEXPORT
	double result = std::numeric_limits<double>::quiet_NaN();

	try {
		handle<instrument<>> i_(inst);
		ensure(i_);
		handle<pwflat::curve<>> c_(curve);
		ensure(c_);
		result = value::convexity(*i_, *c_, t);
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return result;
}

AddIn xai_value_yield(
	Function(XLL_DOUBLE, "xll_value_yield", CATEGORY ".VALUE.YIELD")
	.Arguments({
		Arg(XLL_HANDLEX, "instrument", "is a handle to an instrument."),
		Arg(XLL_DOUBLE, "price", "is the price of the instrument."),
		})
		.Category(CATEGORY)
	.FunctionHelp("Return constant yield repricing the instrument.")
);
double WINAPI xll_value_yield(HANDLEX inst, double p)
{
#pragma XLLEXPORT
	double y = std::numeric_limits<double>::quiet_NaN();

	try {
		handle<instrument<>> i_(inst);
		ensure(i_);

		y = value::yield(*i_, p);
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return y;
}

AddIn xai_value_compound_yield(
	Function(XLL_DOUBLE, "xll_value_compound_yield", CATEGORY ".VALUE.COMPOUND_YIELD")
	.Arguments({
		Arg(XLL_DOUBLE, "yield", "is a continuously compounded yield."),
		Arg(XLL_WORD, "n", "is the number of times to compound per year."),
		})
		.Category(CATEGORY)
	.FunctionHelp("Return the compounded yield.")
);
double WINAPI xll_value_compound_yield(double y, WORD n)
{
#pragma XLLEXPORT
	double r = std::numeric_limits<double>::quiet_NaN();

	try {
		r = value::compound_yield(y, n);
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return r;
}

AddIn xai_value_continuous_yield(
	Function(XLL_DOUBLE, "xll_value_continuous_yield", CATEGORY ".VALUE.CONTINUOUS_YIELD")
	.Arguments({
		Arg(XLL_DOUBLE, "yield", "is compounded yield."),
		Arg(XLL_WORD, "n", "is the number of times the yield is compounded per year."),
		})
		.Category(CATEGORY)
	.FunctionHelp("Return constant continuous_yield repricing the instrument.")
);
double WINAPI xll_value_continuous_yield(double y, WORD n)
{
#pragma XLLEXPORT
	double r = std::numeric_limits<double>::quiet_NaN();

	try {
		r = value::continuous_yield(y, n);
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return r;
}