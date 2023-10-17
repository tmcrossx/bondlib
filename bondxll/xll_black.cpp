// xll_black.cpp - Black forward model
#include "../bondlib/tmx_black.h"
#include "bondxll.h"

using namespace xll;
using namespace tmx;

AddIn xai_normal_cdf(
	Function(XLL_DOUBLE, "xll_normal_cdf", CATEGORY ".NORMAL.CDF")
	.Arguments({
		Arg(XLL_DOUBLE, "x", "is a number."),
		Arg(XLL_DOUBLE, "s", "is the vol."),
		})
		.Category(CATEGORY)
	.FunctionHelp("Return the normal cumulative distribution function.")
);
double WINAPI xll_normal_cdf(double x, double s)
{
#pragma XLLEXPORT
	return black::normal::cdf(x, s);
}

AddIn xai_black_moneyness(
	Function(XLL_DOUBLE, "xll_black_moneyness", CATEGORY ".BLACK.MONEYNESS")
	.Arguments({
	Arg(XLL_DOUBLE, "f", "is the forward price."),
	Arg(XLL_DOUBLE, "s", "is the vol."),
	Arg(XLL_DOUBLE, "k", "is the strike."),
		})
	.Category(CATEGORY)
	.FunctionHelp("Return the Black moneyness log(k/f)/s + s/2.")
);
double WINAPI xll_black_moneyness(double f, double s, double k)
{
#pragma XLLEXPORT
	return black::moneyness(f, s, k);
}

AddIn xai_black_put_value(
	Function(XLL_DOUBLE, "xll_black_put_value", CATEGORY ".BLACK.PUT.VALUE")
	.Arguments({
	Arg(XLL_DOUBLE, "f", "is the forward price."),
	Arg(XLL_DOUBLE, "s", "is the vol."),
	Arg(XLL_DOUBLE, "k", "is the strike."),
		})
		.Category(CATEGORY)
	.FunctionHelp("Return the Black put value.")
);
double WINAPI xll_black_put_value(double f, double s, double k)
{
#pragma XLLEXPORT
	return black::put::value(f, s, k);
}

AddIn xai_black_put_delta(
	Function(XLL_DOUBLE, "xll_black_put_delta", CATEGORY ".BLACK.PUT.DELTA")
	.Arguments({
	Arg(XLL_DOUBLE, "f", "is the forward price."),
	Arg(XLL_DOUBLE, "s", "is the vol."),
	Arg(XLL_DOUBLE, "k", "is the strike."),
		})
		.Category(CATEGORY)
	.FunctionHelp("Return the Black put delta.")
);
double WINAPI xll_black_put_delta(double f, double s, double k)
{
#pragma XLLEXPORT
	return black::put::delta(f, s, k);
}

AddIn xai_black_put_gamma(
	Function(XLL_DOUBLE, "xll_black_put_gamma", CATEGORY ".BLACK.GAMMA")
	.Arguments({
	Arg(XLL_DOUBLE, "f", "is the forward price."),
	Arg(XLL_DOUBLE, "s", "is the vol."),
	Arg(XLL_DOUBLE, "k", "is the strike."),
		})
		.Category(CATEGORY)
	.FunctionHelp("Return the Black put gamma.")
);
double WINAPI xll_black_put_gamma(double f, double s, double k)
{
#pragma XLLEXPORT
	return black::put::gamma(f, s, k);
}

AddIn xai_black_put_vega(
	Function(XLL_DOUBLE, "xll_black_put_vega", CATEGORY ".BLACK.VEGA")
	.Arguments({
	Arg(XLL_DOUBLE, "f", "is the forward price."),
	Arg(XLL_DOUBLE, "s", "is the vol."),
	Arg(XLL_DOUBLE, "k", "is the strike."),
		})
		.Category(CATEGORY)
	.FunctionHelp("Return the Black put vega.")
);
double WINAPI xll_black_put_vega(double f, double s, double k)
{
#pragma XLLEXPORT
	return black::put::vega(f, s, k);
}

AddIn xai_black_call_value(
	Function(XLL_DOUBLE, "xll_black_call_value", CATEGORY ".BLACK.CALL.VALUE")
	.Arguments({
	Arg(XLL_DOUBLE, "f", "is the forward price."),
	Arg(XLL_DOUBLE, "s", "is the vol."),
	Arg(XLL_DOUBLE, "k", "is the strike."),
		})
		.Category(CATEGORY)
	.FunctionHelp("Return the Black call value.")
);
double WINAPI xll_black_call_value(double f, double s, double k)
{
#pragma XLLEXPORT
	return black::call::value(f, s, k);
}

AddIn xai_black_call_delta(
	Function(XLL_DOUBLE, "xll_black_call_delta", CATEGORY ".BLACK.CALL.DELTA")
	.Arguments({
	Arg(XLL_DOUBLE, "f", "is the forward price."),
	Arg(XLL_DOUBLE, "s", "is the vol."),
	Arg(XLL_DOUBLE, "k", "is the strike."),
		})
		.Category(CATEGORY)
	.FunctionHelp("Return the Black call delta.")
);
double WINAPI xll_black_call_delta(double f, double s, double k)
{
#pragma XLLEXPORT
	return black::call::delta(f, s, k);
}

AddIn xai_black_put_implied(
	Function(XLL_DOUBLE, "xll_black_put_implied", CATEGORY ".BLACK.PUT.IMPLIED")
	.Arguments({
	Arg(XLL_DOUBLE, "f", "is the forward price."),
	Arg(XLL_DOUBLE, "p", "is the put price."),
	Arg(XLL_DOUBLE, "k", "is the strike."),
		})
		.Category(CATEGORY)
	.FunctionHelp("Return the Black implied volatility repricing a put.")
);
double WINAPI xll_black_put_implied(double f, double p, double k)
{
#pragma XLLEXPORT
	return black::put::implied(f, p, k);
}
