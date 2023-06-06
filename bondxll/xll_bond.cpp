// xll_bond.cpp - Bonds
#include "../bondlib/tmx_bond.h"
#include "bondxll.h"

using namespace tmx;
using namespace xll;

XLL_CONST(WORD, TMX_FREQUENCY_ANNUALLY, 12 / tmx::bond::frequency::annually.count(), "Yearly payments.", CATEGORY, "")
XLL_CONST(WORD, TMX_FREQUENCY_SEMIANNUALLY, 12 / tmx::bond::frequency::semiannually.count(), "2 payments per year.", CATEGORY, "")
XLL_CONST(WORD, TMX_FREQUENCY_QUARTERLY, 12 / tmx::bond::frequency::quarterly.count(), "4 payments per year.", CATEGORY, "")
XLL_CONST(WORD, TMX_FREQUENCY_MONTHLY, 12 / tmx::bond::frequency::monthly.count(), "12 payments per year.", CATEGORY, "")

/*
AddIn xai_bond_simple_(
	Function(XLL_HANDLEX, "xll_bond_simple_", "\\" CATEGORY ".BOND.SIMPLE")
	.Arguments({
		Arg(XLL_WORD, "maturity", "is the bond maturity in years."),
		Arg(XLL_DOUBLE, "coupon", "is the bond coupon."),
		Arg(XLL_WORD, "frequency", "is the yearly payment frequency from the TMX_FREQUENCY_* enumeration. Default is semiannually"),
		Arg(XLL_HANDLEX, "dcf", "is the day count basis from the TMX_DAY_COUNT_* enumeration. Default is 30/360."),
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

		if (freq == 0) {
			freq = 2;
		}

		dcf_t _dcf = nullptr;
		if (dcf == 0) {
			_dcf = date::dcf_30_360;
		}
		else {
			_dcf = *safe_pointer<dcf_t>(dcf);
		}

		handle<bond::simple<>> h(new bond::simple<>{ years(maturity), coupon, months(12 / freq), _dcf });
		ensure(h);

		result = h.get();
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return result;
}

AddIn xai_bond_cash_flow_(
	Function(XLL_HANDLEX, "xll_bond_cash_flow_", "\\" CATEGORY ".BOND.INSTRUMENT")
	.Arguments({
		Arg(XLL_HANDLEX, "bond", "is a handle to a bond."),
		Arg(XLL_DOUBLE, "dated", "is the dated date of the bond."),
		})
	.Uncalced()
	.Category(CATEGORY)
	.FunctionHelp("Return a handle to bond cash flows.")
);
_FPX* WINAPI xll_bond_cash_flow_(HANDLEX b, double dated)
{
#pragma XLLEXPORT
	static FPX result;

	try {
		handle<bond::simple<>> b_(b);
		ensure(b_);

		handle<instrument<>> i_(new instrument_vector<>(bond::instrument<>(*b_, excel_to_ymd(dated))));
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return result.get();
}
*/