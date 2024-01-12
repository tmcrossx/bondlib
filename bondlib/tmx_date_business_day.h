// tmx_date_business_day.h - Business day conventions.
#pragma once
#include "tmx_date_holiday_calendar.h"

namespace tmx::date::business_day {

	// Roll to business day conventions.
	enum class roll {
		none,
		following, // following business day
		previous,  // previous business day
		modified_following, // following business day unless different month
		modified_previous,  // previous business day unless different month
	};

	constexpr ymd adjust(const ymd& date, roll convention, holiday::calendar::calendar_t cal = holiday::calendar::weekday)
	{
		if (!cal(date)) {
			return date;
		}

		auto d = std::chrono::sys_days(date);

		switch (convention) {
		case roll::none:
			return date;
		case roll::previous:
			return adjust(d - std::chrono::days(1), convention, cal);
		case roll::following:
			return adjust(d + std::chrono::days(1), convention, cal);
		case roll::modified_following:
		{
			const auto date_ = adjust(date, roll::following, cal);
			return date_.month() == date.month()
				? date_
				: adjust(date, roll::previous, cal);
		}
		case roll::modified_previous:
		{
			const auto date_ = adjust(date, roll::previous, cal);
			return date_.month() == date.month()
				? date_
				: adjust(date, roll::following, cal);
		}
		default:
			return ymd{};
		}

		return date;
	}
#ifdef _DEBUG
#define TMX_DATE_BUSINESS_DAY_ADJUST(X) \
	X(2024, 1, 1, roll::none, 2024, 1, 1) \
	X(2023, 12, 31, roll::previous, 2023, 12, 29) \
	X(2023, 12, 31, roll::following, 2024, 1, 1) \

	// TODO: Add more tests

#define BUSINESS_DAY_TEST(Y, M, D, R, Y_, M_, D_) static_assert(adjust(to_ymd(Y, M, D), R) == to_ymd(Y_, M_, D_));
	TMX_DATE_BUSINESS_DAY_ADJUST(BUSINESS_DAY_TEST)
#undef BUSINESS_DAY_TEST
#undef TMX_DATE_BUSINESS_DAY_ADJUST
#endif // _DEBUG

} // namespace tmx::date::business_day
