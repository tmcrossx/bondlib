// tmx_date.h - Date routines
#pragma once
#include <chrono>

namespace tmx::date {

	using ymd = std::chrono::year_month_day;
	using std::chrono::sys_days;

	// days per year conversion convention
	using std::chrono::years;
	constexpr double dpy = years::period::num / (86400. * years::period::den);

	namespace frequency {
		constexpr auto annually = std::chrono::months(12);
		constexpr auto semiannually = std::chrono::months(6);
		constexpr auto quarterly = std::chrono::months(3);
		constexpr auto monthly = std::chrono::months(1);
	}

	template<class Dur>
	constexpr std::chrono::sys_days as_days(const std::chrono::sys_time<Dur>& t)
	{
		return std::chrono::time_point_cast<std::chrono::days>(t);
	}
	template<class Dur>
	constexpr ymd as_ymd(const std::chrono::sys_time<Dur>& t)
	{
		return std::chrono::time_point_cast<ymd>(t);
	}

	template<class Clk, class Dur>
	constexpr auto add_years(const std::chrono::time_point<Clk, Dur>& t, double y)
	{
		return t + std::chrono::seconds{ static_cast<long long>(y * dpy * 86400) };
	}
	template<class Clk, class Dur>
	constexpr double sub_years(const std::chrono::time_point<Clk, Dur>& t1, const std::chrono::time_point<Clk, Dur>& t0, double dpy = date::dpy)
	{
		return std::chrono::round<std::chrono::seconds>(t1 - t0).count()/(dpy*86400);
	}

#ifdef _DEBUG

#pragma warning(push)
#pragma warning(disable: 4455)
	inline int years_test()
	{
		using std::literals::chrono_literals::operator""y;
		using std::chrono::years;
		using std::chrono::months;
		using std::chrono::days;
		using std::chrono::sys_days;
		using std::chrono::sys_time;

		{
			// ymd to time_point
			constexpr sys_days t0{ 2023y / 1 / 1 };
			constexpr sys_days t1{ 2023y / 1 / 2 };

			static_assert(sub_years(t0, t0) == 0);
			static_assert(sub_years(t1, t0) == 1 / dpy);

			//!!! more tests

			// sub_years can be negative
			static_assert(sub_years(t0, t1) == -1 / dpy);
		}
		{
			constexpr sys_days t{ 2023y / 1 / 1 };
			constexpr auto t0 = add_years(t, 1.1);
			constexpr auto t1 = add_years(t0, -1.1);
			static_assert(0 == (t - t1).count());

			/*
			constexpr auto t2 = t + std::chrono::years(1);
			constexpr auto t2_ = sys_days{ 2024y / 1 / 1 };
			constexpr auto dt = t2 - sys_days{2024y / 1 / 1};
			//static_assert(sys_days{ t2 } == sys_days{2024y / 1 / 1});
			*/
		}

		return 0;
	}
#pragma warning(pop)

#endif // _DEBUG

	using dcf_t = double(const ymd&, const ymd&);

	constexpr double dcf_years(const ymd& d0, const ymd& d1)
	{
		return sub_years(sys_days{ d1 }, sys_days{ d0 }, dpy);
	}
	constexpr double dcf_actual_360(const ymd& d0, const ymd& d1)
	{
		return sub_years(sys_days{ d1 }, sys_days{ d0 }, 360);
	}
	constexpr double dcf_actual_365(const ymd& d0, const ymd& d1)
	{
		return sub_years(sys_days{ d1 }, sys_days{ d0 }, 365);
	}

	// 2006-isda-definitions.pdf
	constexpr double dcf_actual_actual(const ymd& d0, const ymd& d1)
	{
		return sub_years(sys_days{ d1 }, sys_days{ d0 }); //!!! not correct
	}

	//!!! tests
	// check with y/2/29 endpoints

#ifdef _DEBUG

#pragma warning(push)
#pragma warning(disable: 4455)
	inline int dcf_actual_actual_test()
	{
		using std::literals::chrono_literals::operator""y;
		using std::chrono::sys_days;
		{
			constexpr sys_days t0{ 2023y / 1 / 1 };
			constexpr sys_days t1{ 2023y / 1 / 2 };
			static_assert(dcf_actual_actual(t0, t0) == 0);
			static_assert(dcf_actual_actual(t0, t1) == 1 / dpy);

			/*
			logical error? there are 367 days between the two dates but adding the leap day changes that to 368
			Question: shouldn't sys_days .count() already factor in leap days?
			and instead shouldn't we split the years as given in the 2006-isda pdf :

			if “Actual/Actual”, “Actual/Actual (ISDA)”, “Act/Act” or “Act/Act (ISDA)” is specified,
			the actual number of days in the Calculation Period or Compounding Period in respect of which payment
			is being made divided by 365 (or, if any portion of that Calculation Period or Compounding Period falls
			in a leap year, the sum of (i) the actual number of days in that portion of the Calculation Period or
			Compounding Period falling in a leap year divided by 366 and (ii) the actual number of days in that
			portion of the Calculation Period or Compounding Period falling in a non-leap year divided by 365)

			*/
			//static_assert(dcf_actual_actual(sys_days{ 2023y / 2 / 28 }, sys_days{ 2024y / 3 / 1 }) == 368 / 365.);
			//static_assert(dcf_actual_actual(sys_days{2024y / 2 / 28}, sys_days{2024y / 2 / 29}) == 2 / 365.);
			//!!! more tests
		}

		return 0;
	}
#pragma warning(pop)

#endif // _DEBUG

	// 2006-isda-definitions.pdf 
	constexpr double dcf_30_360(const ymd& d0, const ymd& d1)
	{
		std::chrono::year_month_day t0{d0};
		std::chrono::year_month_day t1{d1};

		int dy = (int)t1.year() - (int)t0.year();
		int dm = (unsigned)t1.month() - (unsigned)t0.month();
		int dd0 = (unsigned)t0.day();
		int dd1 = (unsigned)t1.day();
		if (dd0 == 31) {
			dd0 = 30;
		}
		if (dd1 == 31 and dd0 > 29) {
			dd1 = 30;
		}
		int dd = dd1 - dd0;

		return dy + dm / 12. + dd / 360.;
	}

#ifdef _DEBUG

#pragma warning(push)
#pragma warning(disable: 4455)
	inline int dcf_30_360_test()
	{
		using std::chrono::sys_days;
		using std::literals::chrono_literals::operator""y;
		{
			constexpr auto t0{ 2023y / 1 / 1 };
			constexpr auto t1{ 2023y / 1 / 2 };
			static_assert(dcf_30_360(t0, t0) == 0);
			static_assert(dcf_30_360(t0, t1) == 1 / 360.);
			static_assert(dcf_30_360(2023y / 1 / 1, 2023y / 2 / 1 ) == 1 / 12.);
			static_assert(dcf_30_360(2023y / 1 / 31, 2023y / 2 / 1 ) == 1 / 12. - 29 / 360.);
			static_assert(dcf_30_360(1985y / 1 / 31, 1986y / 3 / 16) == 406 / 360.);
			//!!! more tests
		}

		return 0;
	}
#pragma warning(pop)

#endif // _DEBUG

}
