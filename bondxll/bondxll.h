// bondxll.h
#pragma once
#include <chrono>
#include "../bondlib/tmx_date.h"
#include "xll/xll/xll.h"

#ifndef CATEGORY
#define CATEGORY "TMX"
#endif

namespace xll {

	using ymd = std::chrono::year_month_day;
	constexpr ymd epoch = std::chrono::year(1900) / 1 / 0;

	inline double ymd_to_excel(ymd d)
	{
		return (std::chrono::sys_days(d) - std::chrono::sys_days(epoch)).count();
	}
	constexpr ymd excel_to_ymd(double t)
	{
		return std::chrono::sys_days(epoch) + std::chrono::days((int)t);
	}

}