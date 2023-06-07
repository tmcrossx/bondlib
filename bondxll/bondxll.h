// bondxll.h
#pragma once
#include <chrono>
#include "../bondlib/tmx_date.h"
#include "xll/xll/xll.h"

#ifndef CATEGORY
#define CATEGORY "TMX"
#endif

namespace xll {

	// Excel epoch.
	constexpr auto epoch{ std::chrono::sys_days(std::chrono::year(1900) / 1 / 0) };

	inline double days_to_excel(const std::chrono::sys_days& d)
	{
		return std::chrono::duration_cast<std::chrono::days>(d - epoch).count();
	}
	// Excel time in days since epoch.
	inline std::chrono::sys_days excel_to_days(double t)
	{
		return epoch + std::chrono::days((int)t);
	}

}