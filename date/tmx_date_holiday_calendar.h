// tmx_date_holiday_calendar.h - Holiday calendars
#pragma once
#include "date/tmx_date_holiday.h"

#define TMX_DATE_HOLIDAY_CALENDAR_DEFAULT SIFMA

#define TMX_DATE_HOLIDAY_CALENDAR(X) \
	X(NONE, none, "No weekends or holidays") \
	X(WEEKEND, weekend, "Saturday and Sunday") \
	X(SIFMA, SIFMA, "US bond market") \
	X(NYSE, NYSE, "New York Stock Exchange") \
	X(FED, FED, "Federal Reserve calendar") \

namespace tmx::date::holiday {

	// Return true on non-trading days.
	using calendar_t = bool(*)(const date::ymd&);

	namespace calendar {
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

		// Federal Reserve
		// https://www.stlouisfed.org/about-us/resources/legal-holiday-schedule
		constexpr bool FED(const ymd& d)
		{
			return SIFMA(d)
				|| holiday::presidents_day(d);
		}

		// New York Stock Exchange holidays
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
	}
} // namespace tmx::date::holiday
