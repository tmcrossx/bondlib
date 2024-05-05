// tmx_date_holiday_calendar.h - Holiday calendars
#pragma once
#include "tmx_date_holiday.h"

namespace tmx::date::holiday::calendar {

	// Return true on non-trading days.
	using calendar_t = bool(*)(const date::ymd&);

	constexpr bool none(const date::ymd&)
	{
		return false;
	}

	constexpr bool weekend(const date::ymd& d)
	{
		return holiday::weekend(d);
	}

	// TODO: Add more calendars
	// US Bond market holidays
	// https://www.sifma.org/resources/general/holiday-schedule/#us
	constexpr bool SIFMA(const ymd& d)
	{
		return weekend(d)
			|| holiday::new_year_day(d)
			|| holiday::martin_luther_king_day(d)
			|| holiday::presidents_day(d)
			|| holiday::good_friday(d)
			|| holiday::memorial_day(d)
			|| holiday::juneteenth(d)
			|| holiday::independence_day(d)
			|| holiday::labor_day(d)
			|| holiday::columbus_day(d)
			|| holiday::veterans_day(d)
			|| holiday::thanksgiving(d)
			|| holiday::christmas_day(d);
	}

	// TODO: Add more calendars
	// https://www.sifma.org/resources/general/holiday-schedule/#us
	constexpr bool NYSE(const ymd& d)
	{	
		return weekend(d)
			|| holiday::new_year_day(d)
			|| holiday::martin_luther_king_day(d)
			|| holiday::memorial_day(d)
			|| holiday::juneteenth(d)
			|| holiday::independence_day(d)
			|| holiday::labor_day(d)
			|| holiday::columbus_day(d)
			|| holiday::veterans_day(d)
			|| holiday::thanksgiving(d)
			|| holiday::christmas_day(d);
	}

} // namespace tmx::date::holiday::calendar
