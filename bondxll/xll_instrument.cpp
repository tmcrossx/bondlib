// xll_instrument.cpp - Instrument times and cash flows.
#include "../bondlib/tmx_instrument.h"
#include "bondxll.h"

using namespace tmx;
using namespace xll;

AddIn xai_instrument_(
	Function(XLL_HANDLEX, "xll_instrument_", "\\" CATEGORY ".INSTRUMENT")
	.Arguments({
		Arg(XLL_FPX, "time", "is an array of times."),
		Arg(XLL_FPX, "cash", "is an array of cash flow amounts."),
		})
	.Uncalced()
	.Category(CATEGORY)
	.FunctionHelp("Return a handle to times and cash flows.")
);
HANDLEX WINAPI xll_instrument_(const _FPX* pu, const _FPX* pc)
{
#pragma XLLEXPORT
	if (size(*pu) != size(*pc)) {
		XLL_ERROR(__FUNCTION__ ": time and cash must have same size");

		return INVALID_HANDLEX;
	}

	handle<instrument<>> h_(new instrument_vector(size(*pu), pu->array, pc->array));

	return h_ ? h_.get() : INVALID_HANDLEX;
}

AddIn xai_instrument(
	Function(XLL_FPX, "xll_instrument", CATEGORY ".INSTRUMENT")
	.Arguments({
		Arg(XLL_HANDLEX, "instrument", "is a handle to an instrument."),
		})
	.Category(CATEGORY)
	.FunctionHelp("Return times and cash flows of instrument.")
);
_FPX* WINAPI xll_instrument(HANDLEX i)
{
#pragma XLLEXPORT
	static FPX result;

	try {
		result.resize(0, 0);
		handle<instrument<>> i_(i);
		ensure(i_);
		int m = (int)i_->size();
		result.resize(2, m);
		std::copy(i_->time(), i_->time() + m, result.array());
		std::copy(i_->cash(), i_->cash() + m, result.array() + m);
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return result.get();
}