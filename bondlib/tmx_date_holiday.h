// tmx_date_holiday.h - Holidays
#pragma once
#include "tmx_date.h"

namespace tmx::holiday {

	// Return true if date is a holiday.
	using holiday_t = bool(*)(const date::ymd&);

	constexpr bool month_day(const date::ymd& d, const std::chrono::month& month, const std::chrono::day& day)
	{
		return (d.month() == month) and (d.day() == day);
	}
	static_assert(month_day(date::to_ymd(2021, 1, 1), std::chrono::January, std::chrono::day(1)));
	static_assert(month_day(std::chrono::year(2021)/1/1, std::chrono::January, std::chrono::day(1)));

	constexpr bool nth_weekday(const date::ymd& d, const std::chrono::month& m, const std::chrono::weekday& wd, unsigned nth)
	{
		std::chrono::weekday_indexed wdi(wd);

		return (d.month() == m) && (d.weekday() == wdi.weekday()) && (d.index() == wdi.index());
	}

	constexpr bool tmx::holiday::new_year_day(const date::ymd& d)
	{
		return month_day(d, std::chrono::January, std::chrono::day(1));
	}

	// TODO: Add more holidays

	constexpr bool tmx::holiday::christmas_day(const date::ymd& d)
	{
		return month_day(d, std::chrono::December, std::chrono::day(25));
	}

#ifdef _DEBUG
	static_assert(new_year_day(date::to_ymd(2021, 1, 1)));
	// TODO: Add more holiday tests
	static_assert(christmas_day(date::to_ymd(2021, 12, 25)));
#endif // _DEBUG

} // namespace tmx::holiday
