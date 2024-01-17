// tmx_date_holiday.h - Holidays
#pragma once
#include "tmx_date.h"

namespace tmx::date::holiday {

	// Return true if date is a holiday.
	using holiday_t = bool(*)(const date::ymd&);

	constexpr bool weekend(const date::ymd& d)
	{
		auto wd = std::chrono::year_month_weekday(d).weekday();

		return (wd == std::chrono::Saturday) || (wd == std::chrono::Sunday);
	}

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

	// Return true if date is the last weekday of the month.
	constexpr bool last_weekday(const date::ymd& d, const std::chrono::weekday_last& wd)
	{
		return d == ymd(std::chrono::year_month_weekday_last(d.year(), d.month(), wd));
	}
	static_assert(last_weekday(to_ymd(2024, 1, 31), std::chrono::weekday_last(std::chrono::Wednesday)));

	// First day of the year. No holiday if weekend.
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
	// Easter Sunday.
	// Friday before Easter.
	//constexpr bool good_friday(const date::ymd& d);

	// Last Monday in May.
	//constexpr bool memorial_day(const date::ymd& d);

	// Juneteenth National Independence Day
	//constexpr bool juneteenth(const date::ymd& d);

	// July 4. Roll to Monday if weekend.
	//constexpr bool independence_day(const date::ymd& d);
	
	// First Monday of September Labor Day
	//constexpr bool memorial_day(const date::ymd& d);
	
	// Second Monday of October
	//constexpr bool columbus_day(const date::ymd& d);
	
	// November 11. If it falls on Saturday, the preceding Friday. If it falls on Sunday, the following Monday.
	//constexpr bool veterans_day(const date::ymd& d);
	
	// Fourth Thursday of November
	//constexpr bool thanksgiving(const date::ymd& d);

	// December 25. Roll to Monday if weekend.
	constexpr bool christmas_day(const date::ymd& d)
	{
		return month_day(d, std::chrono::December, std::chrono::day(25));
	}

#ifdef _DEBUG
	static_assert(new_year_day(to_ymd(2021, 1, 1)));
	static_assert(martin_luther_king_day(to_ymd(2024, 1, 15)));
	//  TODO: Add more holiday tests
	static_assert(christmas_day(to_ymd(2021, 12, 25)));
#endif // _DEBUG

} // namespace tmx::date::holiday
