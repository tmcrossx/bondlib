// tmx_calendar.h - Holiday calendars
#pragma once
#include "tmx_holiday.h"

namespace tmx::calendar {

	// Return true on non-trading days.
	using calendar = bool(*)(const date::ymd&);
		constexpr bool weekday(const date::ymd& d)
		{
			auto w = std::chrono::year_month_weekday(d);
			auto wd = w.weekday();

			return (wd == std::chrono::Saturday) or (wd == std::chrono::Sunday);
		}
		constexpr bool example(const date::ymd& d)
		{
			return weekday(d) or holiday::new_year_day(d);
		}

		// constexpr bool nyse(const ymd& d) ...


} // namespace tmx::calendar
