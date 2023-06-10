#include "../bondlib/tmx_date.h"
#include "bondxll.h"

using namespace tmx;
using namespace xll;

XLL_CONST(HANDLEX, TMX_DAY_COUNT_YEARS, safe_handle(date::dcf_years), "Approximate days per year.", CATEGORY " Enum", "")
XLL_CONST(HANDLEX, TMX_DAY_COUNT_ACTUAL_ACTUAL, safe_handle(date::dcf_actual_actual), "Actual days per year.", CATEGORY " Enum", "")
XLL_CONST(HANDLEX, TMX_DAY_COUNT_30_360, safe_handle(date::dcf_30_360), "30 days per month, 360 days per year.", CATEGORY " Enum", "")
XLL_CONST(HANDLEX, TMX_DAY_COUNT_ACTUAL_360, safe_handle(date::dcf_actual_360), "Actual days divided by 360.", CATEGORY " Enum", "")
XLL_CONST(HANDLEX, TMX_DAY_COUNT_ACTUAL_365, safe_handle(date::dcf_actual_365), "Actual days divided by 365.", CATEGORY " Enum", "")

XLL_CONST(WORD, TMX_FREQUENCY_ANNUALLY, 12 / tmx::date::frequency::annually.count(), "Yearly payments.", CATEGORY " Enum", "")
XLL_CONST(WORD, TMX_FREQUENCY_SEMIANNUALLY, 12 / tmx::date::frequency::semiannually.count(), "2 payments per year.", CATEGORY " Enum", "")
XLL_CONST(WORD, TMX_FREQUENCY_QUARTERLY, 12 / tmx::date::frequency::quarterly.count(), "4 payments per year.", CATEGORY " Enum", "")
XLL_CONST(WORD, TMX_FREQUENCY_MONTHLY, 12 / tmx::date::frequency::monthly.count(), "12 payments per year.", CATEGORY " Enum", "")

AddIn xai_date_add_years(
	Function(XLL_DOUBLE, "xll_date_add_years", CATEGORY ".DATE.ADD_YEARS")
	.Arguments({
		Arg(XLL_DOUBLE, "date", "is an Excel date."),
		Arg(XLL_DOUBLE, "years", "is the number of years to add."),
		})
		.Category(CATEGORY)
	.FunctionHelp("Add years to date.")
);
double WINAPI xll_date_add_years(double d, double y)
{
#pragma XLLEXPORT

	return d + y*date::dpy;
}

AddIn xai_date_sub_years(
	Function(XLL_DOUBLE, "xll_date_sub_years", CATEGORY ".DATE.SUB_YEARS")
	.Arguments({
		Arg(XLL_DOUBLE, "d0", "is an Excel date."),
		Arg(XLL_DOUBLE, "d1", "is an Excel date."),
		})
		.Category(CATEGORY)
	.FunctionHelp("Return d0 - d1 in years.")
);
double WINAPI xll_date_sub_years(double d0, double d1)
{
#pragma XLLEXPORT

	return (d0 - d1)/date::dpy;
}

AddIn xai_date_dcf(
	Function(XLL_DOUBLE, "xll_date_dcf", CATEGORY ".DATE_DCF")
	.Arguments({
		Arg(XLL_HANDLEX, "dcf", "is a day count fraction."),
		Arg(XLL_DOUBLE, "d0", "is an Excel date."),
		Arg(XLL_DOUBLE, "d1", "is an Excel date."),
		})
		.Category(CATEGORY)
	.FunctionHelp("Return day count fraction from d0 to d1.")
);
double WINAPI xll_date_dcf(HANDLEX dcf, double d0, double d1)
{
#pragma XLLEXPORT
	double result = INVALID_HANDLEX;

	try {
		date::dcf_t* _dcf = safe_pointer<date::dcf_t>(dcf);
		if (!_dcf) {
			XLL_ERROR(__FUNCTION__ ": invalid day count fraction");

			return INVALID_HANDLEX;
		}

		result = (*_dcf)(as_days(d0), as_days(d1));
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return result;
}

AddIn xai_date_dcf_years(
	Function(XLL_DOUBLE, "xll_date_dcf_years", CATEGORY ".DATE.DCF_YEARS")
	.Arguments({
		Arg(XLL_DOUBLE, "d0", "is an Excel date."),
		Arg(XLL_DOUBLE, "d1", "is an Excel date."),
		})
		.Category(CATEGORY)
	.FunctionHelp("Return time in years from d0 to d1.")
);

double WINAPI xll_date_dcf_years(double d0, double d1)
{
#pragma XLLEXPORT
	return xll_date_dcf(xll_TMX_DAY_COUNT_YEARS(), d0, d1);
}

AddIn xai_date_dcf_actual_360(
	Function(XLL_DOUBLE, "xll_date_dcf_actual_360", CATEGORY ".DATE.DCF_ACTUAL_360")
	.Arguments({
		Arg(XLL_DOUBLE, "d0", "is an Excel date."),
		Arg(XLL_DOUBLE, "d1", "is an Excel date."),
		})
		.Category(CATEGORY)
	.FunctionHelp("Return Actual/360 day count fraction from d0 to d1.")
);
double WINAPI xll_date_dcf_actual_360(double d0, double d1)
{
#pragma XLLEXPORT
	return xll_date_dcf(xll_TMX_DAY_COUNT_ACTUAL_360(), d0, d1);
}
AddIn xai_date_dcf_actual_365(
	Function(XLL_DOUBLE, "xll_date_dcf_actual_365", CATEGORY ".DATE.DCF_ACTUAL_365")
	.Arguments({
		Arg(XLL_DOUBLE, "d0", "is an Excel date."),
		Arg(XLL_DOUBLE, "d1", "is an Excel date."),
		})
		.Category(CATEGORY)
	.FunctionHelp("Return Actual/365 day count fraction from d0 to d1.")
);
double WINAPI xll_date_dcf_actual_365(double d0, double d1)
{
#pragma XLLEXPORT
	return xll_date_dcf(xll_TMX_DAY_COUNT_ACTUAL_365(), d0, d1);
}
AddIn xai_date_dcf_actual_actual(
	Function(XLL_DOUBLE, "xll_date_dcf_actual_actual", CATEGORY ".DATE.DCF_ACTUAL_ACTUAL")
	.Arguments({
		Arg(XLL_DOUBLE, "d0", "is an Excel date."),
		Arg(XLL_DOUBLE, "d1", "is an Excel date."),
		})
		.Category(CATEGORY)
	.FunctionHelp("Return time Actual/Actual day count fraction from d0 to d1.")
);
double WINAPI xll_date_dcf_actual_actual(double d0, double d1)
{
#pragma XLLEXPORT
	return xll_date_dcf(xll_TMX_DAY_COUNT_ACTUAL_ACTUAL(), d0, d1);
}
AddIn xai_date_dcf_30_360(
	Function(XLL_DOUBLE, "xll_date_dcf_30_360", CATEGORY ".DATE.DCF_30_360")
	.Arguments({
		Arg(XLL_DOUBLE, "d0", "is an Excel date."),
		Arg(XLL_DOUBLE, "d1", "is an Excel date."),
		})
		.Category(CATEGORY)
	.FunctionHelp("Return 30/360 day count fraction from d0 to d1.")
);
double WINAPI xll_date_dcf_30_360(double d0, double d1)
{
#pragma XLLEXPORT
	return xll_date_dcf(xll_TMX_DAY_COUNT_30_360(), d0, d1);
}

