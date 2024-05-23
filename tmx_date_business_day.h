// tmx_date_business_day.h - Business day conventions.
#pragma once
#include "tmx_date_holiday_calendar.h"

// Business day rolling conventions.
#define TMX_DATE_BUSSINESS_DAY_CONVENTION(X) \
	X(MISSING, missing, "Missing. ") \
	X(NONE, none, "No roll.") \
	X(FOLLOWING, following, "Following business day.") \
	X(PREVIOUS, previous, "Previous business day.") \
	X(MODIFIED_FOLLOWING, modified_following, "Following business day unless different month.") \
	X(MODIFIED_PREVIOUS, modified_previous, "Previous business day unless different month.") \

namespace tmx::date::business_day {

#define TMX_DATE_BUSINESS_DAY_CONVENTION_ENUM(a, b, c) b,

	enum class roll {
		TMX_DATE_BUSSINESS_DAY_CONVENTION(TMX_DATE_BUSINESS_DAY_CONVENTION_ENUM)
	};

#undef TMX_DATE_BUSINESS_DAY_CONVENTION_ENUM

	// Move date to business day using roll convention and calendar.
	constexpr std::chrono::sys_days adjust(std::chrono::sys_days d, business_day::roll roll, holiday::calendar::calendar_t cal = holiday::weekend)
	{
		if (cal(d)) {
			if (roll == roll::previous) {
				while (cal(--d))
					;
			}
			else if (roll == roll::following) {
				while (cal(++d))
					;
			}
			else if (roll == roll::modified_following) {
				const auto d_ = adjust(d, roll::following, cal);
				d = date::ymd(d_).month() == date::ymd(d).month()
					? d_ : adjust(d, roll::previous, cal);
			}
			else if (roll == roll::modified_previous) {
				const auto d_ = adjust(d, roll::previous, cal);
				d = date::ymd(d_).month() == date::ymd(d).month()
					? d_ : adjust(d, roll::following, cal);
			}
			else {
				// next non-holiday
				while (cal(++d))
					;
			}
		}

		return d;
	}
	constexpr date::ymd adjust(const date::ymd& date, roll convention, holiday::calendar::calendar_t cal = holiday::weekend)
	{
		return date::ymd{ adjust(std::chrono::sys_days{ date }, convention, cal) };
	}
#ifdef _DEBUG

	static_assert(adjust(2023y / 1 / 1, roll::none) == 2023y / 1 / 2); // Sunday -> Monday
	static_assert(adjust(2024y / 1 / 1, roll::none) == 2024y / 1 / 1);
	static_assert(adjust(2023y / 1 / 1, roll::previous) == 2022y / 12 / 30);
	static_assert(adjust(2023y / 1 / 1, roll::modified_previous) == 2023y / 1 / 2);
	static_assert(adjust(2023y / 12 / 31, roll::previous) == 2023y / 12 / 29);
	static_assert(adjust(2023y / 1 / 1, roll::following) == 2023y / 1 / 2);
	static_assert(adjust(2022y / 12 / 31, roll::following) == 2023y / 1 / 2);
	static_assert(adjust(2022y / 12 / 31, roll::modified_following) == 2022y / 12 / 30);

#endif // _DEBUG
} // namespace tmx::date::business_day

namespace tmx::date {

	// Move date to business day using roll convention and calendar.
	class adjust {
		business_day::roll roll;
		holiday::calendar::calendar_t cal;
	public:
		adjust(business_day::roll roll, holiday::calendar::calendar_t cal)
			: roll(roll), cal(cal)
		{ }
		constexpr date::ymd operator()(ymd d) const
		{
			return business_day::adjust(d, roll, cal) ;
		}
	};

} // namespace tmx::date