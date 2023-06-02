// xll_bond.cpp - Bonds
#include "../bondlib/tmx_bond.h"
#include "bondxll.h"

using namespace tmx;
using namespace xll;

XLL_CONST(WORD, BONDLIB_FREQUENCY_ANNUALLY, 12 / tmx::bond::frequency::annually.count(), "Yearly payments.", CATEGORY, "")
XLL_CONST(WORD, BONDLIB_FREQUENCY_SEMIANNUALLY, 12 / tmx::bond::frequency::semiannually.count(), "2 payments per year.", CATEGORY, "")
XLL_CONST(WORD, BONDLIB_FREQUENCY_QUARTERLY, 12 / tmx::bond::frequency::quarterly.count(), "4 payments per year.", CATEGORY, "")
XLL_CONST(WORD, BONDLIB_FREQUENCY_MONTHLY, 12 / tmx::bond::frequency::monthly.count(), "12 payments per year.", CATEGORY, "")

XLL_CONST(HANDLEX, BONDLIB_DAY_COUNT_30_360, safe_handle(date::dcf_30_360), "30 days per month, 360 days per year.", CATEGORY, "")

AddIn xai_bond_simple_(
	Function(XLL_HANDLEX, "xll_bond_simple_", "\\BOND.SIMPLE")
	.Arguments({
		Arg(XLL_WORD, "maturity", "is the bond maturity in years."),
		Arg(XLL_DOUBLE, "coupon", "is the bond coupon."),
		Arg(XLL_WORD, "frequency", "is the yearly payment frequency from the BONDLIB_FREQUENCY_* enumeration. Default is semiannually"),
		Arg(XLL_HANDLEX, "dcf", "is the day count basis from the BONDLIB_DAY_COUNT_* enumeration. Default is 30/360."),
		})
	.Uncalced()
	.Category(CATEGORY)
	.FunctionHelp("Return a handle to a simple bond.")
);
HANDLEX WINAPI xll_bond_simple_(WORD maturity, double coupon, WORD freq, HANDLEX dcf)
{
#pragma XLLEXPORT
	HANDLEX result = INVALID_HANDLEX;

	try {
		using std::chrono::years;
		using std::chrono::months;
		using dcf_t = double(*)(const date::ymd&, const date::ymd&);

		dcf_t _dcf = nullptr;
		if (freq == 0) {
			freq = 2;
		}
		if (dcf == 0) {
			_dcf = date::dcf_30_360;
		}
		else {
			_dcf = *safe_pointer<dcf_t>(dcf);
		}

		handle<bond::simple<>> h(new bond::simple(years(maturity), coupon, months(12 / freq), _dcf));
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return result;
}