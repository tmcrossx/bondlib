// bondxll.h
#pragma once
#include <chrono>
#include "../bondlib/tmx_date.h"
#include "xll/xll/xll.h"

#ifndef CATEGORY
#define CATEGORY "TMX"
#endif

namespace xll {

	//using ymd = std::chrono::year_month_day;
	using sys_days = std::chrono::sys_days;
	constexpr auto epoch{ sys_days(std::chrono::year(1900) / 1 / 0) };

	inline double days_to_excel(const sys_days& d)
	{
		return (d - epoch).count();
	}
	// Excel time in days since epoch.
	inline auto excel_to_days(double t)
	{
		return epoch + std::chrono::seconds(static_cast<int>(t * 86400));
	}

}