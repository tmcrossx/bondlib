// bondxll.h
#pragma once
#include <chrono>
#include "../bondlib/tmx_date.h"
#include "xll/xll/xll.h"

#ifndef CATEGORY
#define CATEGORY "TMX"
#endif

namespace xll {

	using sys_seconds = std::chrono::sys_seconds;
	using sys_days = std::chrono::sys_days;
	constexpr auto epoch{ std::chrono::sys_days(std::chrono::year(1900) / 1 / 0) };

	inline double days_to_excel(const sys_days& d)
	{
		return std::chrono::duration_cast<std::chrono::days>(d - epoch).count();
	}
	// Excel time in days since epoch.
	inline sys_days excel_to_days(double t)
	{
		return epoch + std::chrono::days((int)t);
	}

}