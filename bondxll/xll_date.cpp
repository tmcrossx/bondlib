#include "../bondlib/tmx_date.h"
#include "bondxll.h"

using namespace tmx;
using namespace xll;

AddIn xai_date_add_years(
	Function(XLL_DOUBLE, "xll_date_add_years", CATEGORY ".DATE.ADD.YEARS")
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
	auto ymd = excel_to_ymd(d);
	ymd = date::add_years(ymd, y);

	return ymd_to_excel(ymd);
}

AddIn xai_date_dcf_years(
	Function(XLL_DOUBLE, "xll_date_dcf_years", CATEGORY ".DATE.DCF.YEARS")
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
	auto y0 = excel_to_ymd(d0);
	auto y1 = excel_to_ymd(d1);

	return date::dcf_years(y0, y1);
}