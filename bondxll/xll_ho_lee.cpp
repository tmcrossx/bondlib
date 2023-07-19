// xll_ho_lee.cpp - Ho-Lee model
#include "../bondlib/tmx_ho_lee.h"
#include "bondxll.h"

using namespace xll;
using namespace tmx;

AddIn xai_ho_lee_ED(
	Function(XLL_DOUBLE, "xll_ho_lee_ED", CATEGORY ".HO_LEE.ED")
	.Arguments({
		Arg(XLL_DOUBLE, "Dt", "is the discount to time t."),
		Arg(XLL_DOUBLE, "Du", "is the discount to time u."),
		Arg(XLL_DOUBLE, "t", "is the start time of the interval."),
		Arg(XLL_DOUBLE, "u", "is the end time for the interval."),
		Arg(XLL_DOUBLE, "σ", "is the Ho-Lee volatility."),
		})
	.Category(CATEGORY)
	.FunctionHelp("Return the Ho-Lee expected discount E_t[D_u].")
);
double WINAPI xll_ho_lee_ED(double Dt, double Du, double t, double u, double σ)
{
#pragma XLLEXPORT
	double result = std::numeric_limits<double>::quiet_NaN();

	try {
		result = ho_lee::ED(Dt, Du, t, u, σ);
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return result;
}

AddIn xai_ho_lee_ELogD(
	Function(XLL_DOUBLE, "xll_ho_lee_ELogD", CATEGORY ".HO_LEE.ELogD")
	.Arguments({
		Arg(XLL_DOUBLE, "Dt", "is the discount to time t."),
		Arg(XLL_DOUBLE, "Du", "is the discount to time u."),
		Arg(XLL_DOUBLE, "t", "is the start time of the interval."),
		Arg(XLL_DOUBLE, "u", "is the end time for the interval."),
		Arg(XLL_DOUBLE, "σ", "is the Ho-Lee volatility."),
		})
		.Category(CATEGORY)
	.FunctionHelp("Return the Ho-Lee expected value of log zero price E[log D_t(u)]].")
);
double WINAPI xll_ho_lee_ELogD(double Dt, double Du, double t, double u, double σ)
{
#pragma XLLEXPORT
	double result = std::numeric_limits<double>::quiet_NaN();

	try {
		result = ho_lee::ELogD(Dt, Du, t, u, σ);
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return result;
}

AddIn xai_ho_lee_VarLogD(
	Function(XLL_DOUBLE, "xll_ho_lee_VarLogD", CATEGORY ".HO_LEE.VarLogD")
	.Arguments({
		Arg(XLL_DOUBLE, "t", "is the start time of the interval."),
		Arg(XLL_DOUBLE, "u", "is the end time for the interval."),
		Arg(XLL_DOUBLE, "σ", "is the Ho-Lee volatility."),
		})
		.Category(CATEGORY)
	.FunctionHelp("Return the Ho-Lee expected variance of log zero price Var(log D_t(u)).")
);
double WINAPI xll_ho_lee_VarLogD(double t, double u, double σ)
{
#pragma XLLEXPORT
	double result = std::numeric_limits<double>::quiet_NaN();

	try {
		result = ho_lee::VarLogD(t, u, σ);
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return result;
}

AddIn xai_value_ho_lee_moments(
	Function(XLL_FPX, "xll_value_ho_lee_moments", CATEGORY ".HO_LEE.Moments")
	.Arguments({
		Arg(XLL_HANDLEX, "instrument", "is a handle to an instrument."),
		Arg(XLL_HANDLEX, "curve", "is a handle to a curve."),
		Arg(XLL_DOUBLE, "t", "is the time at which to compute the moments."),
		Arg(XLL_DOUBLE, "σ", "is the Ho-Lee volatility."),
		})
		.Category(CATEGORY)
	.FunctionHelp("Return Ho-Lee mean, forward mean, and log variance of instrument at time t.")
);
_FPX* WINAPI xll_value_ho_lee_moments(HANDLEX i, HANDLEX c, double t, double σ)
{
#pragma XLLEXPORT
	static FPX mv(3,1);

	try {
		handle<instrument<>> i_(i);
		ensure(i_);
		handle<curve<>> c_(c);
		ensure(c_);
		auto [m, m_, v] = ho_lee::moments(*i_, *c_, t, σ);
		mv[0] = m;
		mv[1] = m_;
		mv[2] = v;
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
		return nullptr;
	}

	return mv.get();
}


AddIn xai_value_ho_lee_option(
	Function(XLL_DOUBLE, "xll_value_ho_lee_option", CATEGORY ".HO_LEE.Option")
	.Arguments({
		Arg(XLL_HANDLEX, "instrument", "is a handle to an instrument."),
		Arg(XLL_HANDLEX, "curve", "is a handle to a curve."),
		Arg(XLL_DOUBLE, "t", "is the call time."),
		Arg(XLL_DOUBLE, "σ", "is the Ho-Lee volatility."),
		Arg(XLL_DOUBLE, "p", "is the optional call price. Default is 1."),
		})
		.Category(CATEGORY)
	.FunctionHelp("Return Ho-Lee option value with call date u and call price p at time t.")
);
double WINAPI xll_value_ho_lee_option(HANDLEX i, HANDLEX c, double t, double σ, double p)
{
#pragma XLLEXPORT
	double result = std::numeric_limits<double>::quiet_NaN();

	try {
		if (p == 0) {
			p = 1;
		}
		handle<instrument<>> i_(i);
		ensure(i_);
		handle<curve<>> c_(c);
		ensure(c_);

		result = ho_lee::option(*i_, *c_, t, σ, p);
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return result;
}