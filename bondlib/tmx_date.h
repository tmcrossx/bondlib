// tmx_date.h - Date routines
#pragma once
#include <chrono>

namespace tmx::date {

	using ymd = std::chrono::year_month_day;
	using std::chrono::sys_seconds;

	// days per year conversion convention
	constexpr double dpy = 365.25;

	constexpr sys_seconds add_years(const sys_seconds& t, double y)
	{
		return t + std::chrono::seconds{ static_cast<int>(y * dpy * 86400) };
	}
	constexpr double diff_years(const sys_seconds& t0, const sys_seconds& t1, double dpy = date::dpy)
	{
		return (t1 - t0).count() / (dpy * 86400);
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

		constexpr auto abs = [](double d) {
			return d >= 0 ? d : -d;
		};
		{
			// ymd to time_point
			constexpr std::chrono::sys_days t0{ 2023y / 1 / 1 };
			constexpr std::chrono::sys_days t1{ 2023y / 1 / 2 };

			static_assert(diff_years(t0, t0) == 0);
			static_assert(diff_years(t0, t1) == 1 / dpy);

			//!!! more tests

			// diff_years can be negative
			static_assert(diff_years(t1, t0) == -1 / 365.25);

			constexpr double y = diff_years(t0, t1);
			static_assert(t1 == add_years(t0, y));
		}

		return 0;
	}
#pragma warning(pop)

#endif // _DEBUG


	constexpr double dcf_years(const ymd& d0, const ymd& d1, double dpy = date::dpy)
	{
		std::chrono::sys_days t0{d0};
		std::chrono::sys_days t1{d1};

		return diff_years(t0, t1, dpy);
	}
	constexpr double dcf_actual_360(const ymd& d0, const ymd& d1)
	{
		return dcf_years(d0, d1, 360);
	}
	constexpr double dcf_actual_365(const ymd& d0, const ymd& d1)
	{
		return dcf_years(d0, d1, 365);
	}

	// 2006-isda-definitions.pdf
	constexpr double dcf_actual_actual(const ymd& d0, const ymd& d1)
	{
		double dcf = 0;

		auto t0 = std::chrono::sys_days(d0);
		auto t1 = std::chrono::sys_days(d1);
		auto y0 = d0.year();
		auto y1 = d1.year();
		int ld = 0; // leap days
		for (auto yi = y0; yi <= y1; ++yi) {
			if (d0 <= yi / 2 / 29 and yi / 2 / 29 <= d1) {
				ld += yi.is_leap();
				//!!! days/366
			}
			else {
				//!!! days/365
			}
		}

		return (ld + (t1 - t0).count()) / 365.;
	}

	//!!! tests
	// check with y/2/29 endpoints

#ifdef _DEBUG

#pragma warning(push)
#pragma warning(disable: 4455)
	inline int dcf_actual_actual_test()
	{
		using std::literals::chrono_literals::operator""y;
		{
			static_assert(dcf_actual_actual(2023y / 1 / 1, 2023y / 1 / 1) == 0);
			static_assert(dcf_actual_actual(2023y / 1 / 1, 2023y / 1 / 2) == 1 / 365.);

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
			static_assert(dcf_actual_actual(2023y / 2 / 28, 2024y / 3 / 1) == 368 / 365.);
			static_assert(dcf_actual_actual(2024y / 2 / 28, 2024y / 2 / 29) == 2 / 365.);
			//!!! more tests
		}

		return 0;
	}
#pragma warning(pop)

#endif // _DEBUG

	constexpr double dcf_30_360(const ymd& d0, const ymd& d1)
	{
		auto dy = d1.year() - d0.year();
		auto dm = d1.month() - d0.month();
		auto dd = d1.day() - d0.day();

		// months are computed modulo 12
		int md = d1.month() >= d0.month() ? dm.count() : dm.count() - 12;

		return dy.count() + md / 12. + dd.count() / 360.;
	}

#ifdef _DEBUG

#pragma warning(push)
#pragma warning(disable: 4455)
	inline int dcf_30_360_test()
	{
		using std::literals::chrono_literals::operator""y;
		{
			static_assert(dcf_30_360(2023y / 1 / 1, 2023y / 1 / 1) == 0);
			static_assert(dcf_30_360(2023y / 1 / 1, 2023y / 1 / 2) == 1 / 360.);
			static_assert(dcf_30_360(2022y / 12 / 2, 2023y / 1 / 2) == 1 - 11 / 12.);
			//!!! more tests
		}

		return 0;
	}
#pragma warning(pop)

#endif // _DEBUG

}