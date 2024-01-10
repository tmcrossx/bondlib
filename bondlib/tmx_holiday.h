// tmx_holiday.h - Holidays
#pragma once
#include <map>
#include <string>
#include "tmx_date.h"

namespace tmx::holiday {

	// Return true if date is a holiday.
	using holiday = bool(*)(const date::ymd&);

	inline std::map<std::string, holiday> name;

	// inline namespace {
	// Yearly holiday on month and day
	constexpr bool month_day(const date::ymd& d, const std::chrono::month& month, const std::chrono::day& day)
	{
		return (d.month() == month) and (d.day() == day);
	}
	// } // inline namespace

	bool new_year_day(const date::ymd& d);

	// ...

	constexpr bool christmas_day(const date::ymd& d)
	{
		return month_day(d, std::chrono::December, std::chrono::day(25));
	}

} // namespace tmx::holiday
