// tmx_date_holiday.h - Holidays
#pragma once
#include "tmx_date.h"

namespace tmx::date::holiday {

	// Return true if date is a holiday.
	using holiday_t = bool(*)(const date::ymd&);

	// Saturday or Sunday.
	constexpr bool weekend(const date::ymd& d)
	{
		const auto wd = std::chrono::year_month_weekday(d).weekday();

		return (wd == std::chrono::Saturday) || (wd == std::chrono::Sunday);
	}

	// Date has given month and day.
	constexpr bool month_day(const date::ymd& d, const std::chrono::month& month, const std::chrono::day& day)
	{
		return (d.month() == month) and (d.day() == day);
	}

	// Return true if date is the nth weekday of the month.
	constexpr bool nth_weekday(const date::ymd& d, const std::chrono::month& m, const std::chrono::weekday& wd, unsigned nth)
	{
		return std::chrono::year_month_weekday(d) == d.year() / m / std::chrono::weekday_indexed(wd, nth);
	}

	// Return true if date is the last weekday of the month.
	constexpr bool last_weekday(const date::ymd& d, const std::chrono::weekday_last& wd)
	{
		return d == ymd(std::chrono::year_month_weekday_last(d.year(), d.month(), wd));
	}

	// TODO: Add more holidays

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

	// Third Monday in February.
	constexpr bool presidents_day(const date::ymd& d)
	{
		return nth_weekday(d, std::chrono::February, std::chrono::Monday, 3);
	}

	// Easter Sunday.
	constexpr bool easter(const date::ymd& d)
	{
		int y = static_cast<int>(d.year());
		int h = (24 + 19 * (y % 19)) % 30;
		int i = h - h / 28;
		int j = (y + y/4 + i - 13) % 7;
		int l = i - j;

		int month = 3 + (l + 40) / 44;
		int day = l + 28 - 31 * (month / 4);

		return d.month() == std::chrono::month(month) && d.day() == std::chrono::day(day);
	}

	// Friday before Easter.
	constexpr bool good_friday(const date::ymd& d)
	{
		return easter(std::chrono::sys_days{ d } + std::chrono::days(2));
	
	}

	// Last Monday in May.
	constexpr bool memorial_day(const date::ymd& d)
	{
		return last_weekday(d, std::chrono::weekday_last(std::chrono::Monday));
	}
	static_assert(memorial_day(2021y / 5 / 31));

	// Juneteenth National Independence Day
	constexpr bool juneteenth(const date::ymd& d)
	{
		return month_day(d, std::chrono::June, std::chrono::day(19));
	}

	// July 4. Roll to Monday if weekend.
	constexpr bool independence_day(const date::ymd& d)
	{
		const auto wd = std::chrono::year_month_weekday(d.year()/7/4).weekday();

		if (wd == std::chrono::Saturday) {
			return month_day(d, std::chrono::July, std::chrono::day(3));
		}
		else if (wd == std::chrono::Sunday) {
			return month_day(d, std::chrono::July, std::chrono::day(5));
		}
		else{
			return month_day(d, std::chrono::July, std::chrono::day(4));
		}
	}
	
	// First Monday of September Labor Day
	constexpr bool labor_day(const date::ymd& d)
	{
		return nth_weekday(d, std::chrono::September, std::chrono::Monday, 1);
	}
	
	// Second Monday of October
	constexpr bool columbus_day(const date::ymd& d)
	{
		return nth_weekday(d, std::chrono::October, std::chrono::Monday, 2);
	}
	
	// November 11. If it falls on Saturday, the preceding Friday. If it falls on Sunday, the following Monday.
	constexpr bool veterans_day(const date::ymd& d)
	{
		const auto wd = std::chrono::year_month_weekday(d.year() / 11 / 11).weekday();

		if (wd == std::chrono::Saturday) {
			return month_day(d, std::chrono::November, std::chrono::day(10));
		}
		else if (wd == std::chrono::Sunday) {
			return month_day(d, std::chrono::November, std::chrono::day(12));
		}
		else {
			return month_day(d, std::chrono::November, std::chrono::day(11));
		}
	}
	
	// Fourth Thursday of November
	constexpr bool thanksgiving(const date::ymd& d)
	{
		return nth_weekday(d, std::chrono::November, std::chrono::Thursday, 4);
	}

	// December 25. Roll to Monday if weekend.
	constexpr bool christmas_day(const date::ymd& d)
	{
		const auto wd = std::chrono::year_month_weekday(d.year()/ 12 / 25).weekday();
		
		if (wd == std::chrono::Saturday) {
			return month_day(d, std::chrono::December, std::chrono::day(24));
		}
		else if (wd == std::chrono::Sunday) {
			return month_day(d, std::chrono::December, std::chrono::day(26));
		}
		else {
			return month_day(d, std::chrono::December, std::chrono::day(25));
		}
	}

#ifdef _DEBUG
	static_assert(month_day(2021y / 1 / 1, std::chrono::January, std::chrono::day(1)));
	static_assert(nth_weekday(2024y / 1 / 11, std::chrono::January, std::chrono::Thursday, 2));
	static_assert(last_weekday(2024y / 1 / 31, std::chrono::weekday_last(std::chrono::Wednesday)));
	static_assert(new_year_day(2021y / 1 / 1));
	static_assert(martin_luther_king_day(2024y / 1 / 15));
	static_assert(easter(2021y / 4 / 4));
	static_assert(good_friday(2021y / 4 / 2));
	static_assert(memorial_day(2021y / 5 / 31));
	static_assert(juneteenth(2021y / 6 / 19));
	static_assert(independence_day(2023y / 7 / 4));
	static_assert(independence_day(2020y / 7 / 3));
	static_assert(independence_day(2021y / 7 / 5));
	static_assert(labor_day(2021y / 9 / 6));
	static_assert(columbus_day(2021y / 10 / 11));
	static_assert(veterans_day(2021y / 11 / 11));
	static_assert(veterans_day(2023y / 11 / 10));
	static_assert(thanksgiving(2021y / 11 / 25));
	static_assert(christmas_day(2021y / 12 / 24));
	static_assert(christmas_day(2022y / 12 / 26));
	static_assert(christmas_day(2023y / 12 / 25));
#pragma region Easter
	static_assert(easter(1990y / 4 / 15));
	static_assert(easter(1991y / 3 / 31));
	static_assert(easter(1992y / 4 / 19));
	static_assert(easter(1993y / 4 / 11));
	static_assert(easter(1994y / 4 / 3));
	static_assert(easter(1995y / 4 / 16));
	static_assert(easter(1996y / 4 / 7));
	static_assert(easter(1997y / 3 / 30));
	static_assert(easter(1998y / 4 / 12));
	static_assert(easter(1999y / 4 / 4));
	static_assert(easter(2000y / 4 / 23));
	static_assert(easter(2001y / 4 / 15));
	static_assert(easter(2002y / 3 / 31));
	static_assert(easter(2003y / 4 / 20));
	static_assert(easter(2004y / 4 / 11));
	static_assert(easter(2005y / 3 / 27));
	static_assert(easter(2006y / 4 / 16));
	static_assert(easter(2007y / 4 / 8));
	static_assert(easter(2008y / 3 / 23));
	static_assert(easter(2009y / 4 / 12));
	static_assert(easter(2010y / 4 / 4));
	static_assert(easter(2011y / 4 / 24));
	static_assert(easter(2012y / 4 / 8));
	static_assert(easter(2013y / 3 / 31));
	static_assert(easter(2014y / 4 / 20));
	static_assert(easter(2015y / 4 / 5));
	static_assert(easter(2016y / 3 / 27));
	static_assert(easter(2017y / 4 / 16));
	static_assert(easter(2018y / 4 / 1));
	static_assert(easter(2019y / 4 / 21));
	static_assert(easter(2020y / 4 / 12));
	static_assert(easter(2021y / 4 / 4));
	static_assert(easter(2022y / 4 / 17));
	static_assert(easter(2023y / 4 / 9));
	static_assert(easter(2024y / 3 / 31));
	static_assert(easter(2025y / 4 / 20));
	static_assert(easter(2026y / 4 / 5));
	static_assert(easter(2027y / 3 / 28));
	static_assert(easter(2028y / 4 / 16));
	static_assert(easter(2029y / 4 / 1));
	static_assert(easter(2030y / 4 / 21));
	static_assert(easter(2031y / 4 / 13));
	static_assert(easter(2032y / 3 / 28));
	static_assert(easter(2033y / 4 / 17));
	static_assert(easter(2034y / 4 / 9));
	static_assert(easter(2035y / 3 / 25));
	static_assert(easter(2036y / 4 / 13));
	static_assert(easter(2037y / 4 / 5));
	static_assert(easter(2038y / 4 / 25));
	static_assert(easter(2039y / 4 / 10));
	static_assert(easter(2040y / 4 / 1));
	static_assert(easter(2041y / 4 / 21));
	static_assert(easter(2042y / 4 / 6));
	static_assert(easter(2043y / 3 / 29));
	static_assert(easter(2044y / 4 / 17));
	static_assert(easter(2045y / 4 / 9));
	static_assert(easter(2046y / 3 / 25));
	static_assert(easter(2047y / 4 / 14));
	static_assert(easter(2048y / 4 / 5));
	static_assert(easter(2049y / 4 / 18));
#pragma endregion
#endif // _DEBUG

} // namespace tmx::date::holiday
