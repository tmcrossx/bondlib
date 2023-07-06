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
	.FunctionHelp("Return the Ho-Lee expected value of log discount E[log D_t(u)]].")
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
	.FunctionHelp("Return the Ho-Lee expected variance of log discount Var(log D_t(u)).")
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