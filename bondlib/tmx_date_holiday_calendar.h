// tmx_date_holiday_calendar.h - Holiday calendars
#pragma once
#include "tmx_date_holiday.h"

namespace tmx::date::holiday::calendar {

	// Return true on non-trading days.
	using calendar_t = bool(*)(const date::ymd&);

	constexpr bool weekend(const date::ymd& d)
	{
		auto wd = std::chrono::year_month_weekday(d).weekday();

		return (wd == std::chrono::Saturday) || (wd == std::chrono::Sunday);
	}

	constexpr bool NYSE(const ymd& d)
	{
		return weekend(d)
			|| holiday::new_year_day(d)
			|| holiday::martin_luther_king_day(d)
			// TODO: Add more holidays
			|| holiday::christmas_day(d);
	}

} // namespace tmx::date::holiday::calendar
