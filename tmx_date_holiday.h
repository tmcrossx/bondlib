// tmx_date_holiday.h - Holidays
#pragma once
#include "tmx_date.h"

namespace tmx::date::holiday {

	// Return true if date is a holiday.
	using holiday_t = bool(*)(const date::ymd&);

	constexpr bool month_day(const date::ymd& d, const std::chrono::month& month, const std::chrono::day& day)
	{
		return (d.month() == month) and (d.day() == day);
	}
	static_assert(month_day(date::to_ymd(2021, 1, 1), std::chrono::January, std::chrono::day(1)));
	static_assert(month_day(std::chrono::year(2021) / 1 / 1, std::chrono::January, std::chrono::day(1)));

	// Return true if date is the nth weekday of the month.
	constexpr bool nth_weekday(const date::ymd& d, const std::chrono::month& m, const std::chrono::weekday& wd, unsigned nth)
	{
		return std::chrono::year_month_weekday(d) == d.year() / m / std::chrono::weekday_indexed(wd, nth);
	}
	static_assert(nth_weekday(to_ymd(2024, 1, 11), std::chrono::January, std::chrono::Thursday, 2));

	// First day of the year.
	constexpr bool new_year_day(const date::ymd& d)
	{
		return month_day(d, std::chrono::January, std::chrono::day(1));
	}

	// Third Monday in January.
	constexpr bool martin_luther_king_day(const date::ymd& d)
	{
		return nth_weekday(d, std::chrono::January, std::chrono::Monday, 3);
	}

	// TODO: Add more holidays

	constexpr bool christmas_day(const date::ymd& d)
	{
		return month_day(d, std::chrono::December, std::chrono::day(25));
	}

#ifdef _DEBUG
	static_assert(new_year_day(to_ymd(2021, 1, 1)));
	static_assert(martin_luther_king_day(to_ymd(2024, 1, 15)));
	// TODO: Add more holiday tests
	static_assert(christmas_day(to_ymd(2021, 12, 25)));
#endif // _DEBUG

} // namespace tmx::date::holiday
