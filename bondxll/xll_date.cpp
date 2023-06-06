#include "../bondlib/tmx_date.h"
#include "bondxll.h"

using namespace tmx;
using namespace xll;

XLL_CONST(HANDLEX, TMX_DAY_COUNT_30_360, safe_handle(date::dcf_30_360), "30 days per month, 360 days per year.", CATEGORY, "")
XLL_CONST(HANDLEX, TMX_DAY_COUNT_ACTUAL_360, safe_handle(date::dcf_actual_360), "Actual days divided by 360.", CATEGORY, "")
XLL_CONST(HANDLEX, TMX_DAY_COUNT_ACTUAL_365, safe_handle(date::dcf_actual_365), "Actual days divided by 365.", CATEGORY, "")

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
	auto t = excel_to_days(d);
	t = date::add_years(t, y);

	return days_to_excel(t);
}

AddIn xai_date_sub_years(
	Function(XLL_DOUBLE, "xll_date_sub_years", CATEGORY ".DATE.SUB.YEARS")
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
	auto t0 = excel_to_days(d0);
	auto t1 = excel_to_days(d1);

	return date::sub_years(t0, t1);
}

AddIn xai_date_ymd(
	Function(XLL_DOUBLE, "xll_date_ymd", CATEGORY ".DATE.YMD")
	.Arguments({
		Arg(XLL_DOUBLE, "date", "is an Excel date."),
		})
	.Category(CATEGORY)
	.FunctionHelp("Return Excel date as yyyymmdd.")
);
double WINAPI xll_date_ymd(double d)
{
#pragma XLLEXPORT
	auto y = std::chrono::year_month_day(excel_to_days(d));

	return (int)y.year() * 10000 + (unsigned)y.month() * 100 + (unsigned)y.day();
}

AddIn xai_date_excel(
	Function(XLL_DOUBLE, "xll_date_excel", CATEGORY ".DATE.EXCEL")
	.Arguments({
		Arg(XLL_DOUBLE, "date", "is a yyyymmdd date."),
		})
		.Category(CATEGORY)
	.FunctionHelp("Return a yyyymmdd date as an Excel date.")
);
double WINAPI xll_date_excel(double date)
{
#pragma XLLEXPORT
	int y = (int)date / 10000;
	date -= y * 10000;
	unsigned m = (unsigned)date / 100;
	date -= m * 100;
	unsigned d = (unsigned)date;

	auto t = std::chrono::sys_days(std::chrono::year_month_day(std::chrono::year(y), std::chrono::month(m), std::chrono::day(d)));

	return days_to_excel(t);
}

AddIn xai_date_dcf(
	Function(XLL_DOUBLE, "xll_date_dcf", CATEGORY ".DATE.DCF")
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
	using dcf_t = double(*)(const date::ymd&, const date::ymd&);
	auto _dcf = *safe_pointer<dcf_t>(dcf);
	if (!_dcf) {
		XLL_ERROR(__FUNCTION__ ": invalid day count fraction");

		return INVALID_HANDLEX;
	}

	auto y0 = excel_to_days(d0);
	auto y1 = excel_to_days(d1);

	return _dcf(y0, y1);
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
	auto y0 = excel_to_days(d0);
	auto y1 = excel_to_days(d1);

	return date::dcf_years(y0, y1);
}
