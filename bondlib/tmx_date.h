#pragma once
#include <chrono>

namespace tmx::date {

	using ymd = std::chrono::year_month_day;

	// days per year
	constexpr double dpy = 365.25;

	constexpr ymd add_years(ymd d, double y)
	{
		double dy = y * dpy;
		int iy = static_cast<int>(dy);
		iy += dy - iy > 0.5; // round

		return std::chrono::sys_days(d) + std::chrono::days{iy};
	}

	constexpr double dcf_years(const ymd& d0, const ymd& d1, double dpy = date::dpy)
	{
		auto t0 = std::chrono::sys_days(d0);
		auto t1 = std::chrono::sys_days(d1);

		return (t1 - t0).count() / dpy;
	}

#ifdef _DEBUG

#pragma warning(push)
#pragma warning(disable: 4455)
	inline int test_dcf_years()
	{
		using std::literals::chrono_literals::operator""y;
		constexpr auto abs = [](double d) {
			return d >= 0 ? d : -d;
		};
		{
			static_assert(dcf_years(2023y / 1 / 1, 2023y / 1 / 1) == 0);
			static_assert(dcf_years(2023y / 1 / 1, 2023y / 1 / 2) == 1 / 365.25);
			//!!! more tests
		}
		{
			constexpr auto d0 = 2023y / 1 / 1;
			constexpr double y = 0;
			constexpr auto d1 = add_years(d0, y);
			static_assert(dcf_years(d1, d0) == y);
		}
		{
			constexpr auto d0 = 2023y / 1 / 1;
			constexpr double y = 1.2;
			constexpr auto d1 = add_years(d0, y);
			static_assert(abs(dcf_years(d0, d1) - y) < .5/dpy);
		}
		{
			constexpr auto d0 = 2023y / 1 / 1;
			constexpr double y = -1.2;
			constexpr auto d1 = add_years(d0, y);
			static_assert(abs(dcf_years(d0, d1) - y) < .5/dpy);
		}

		return 0;
	}
#pragma warning(pop)

#endif // _DEBUG

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

	constexpr double dcf_30_360(const ymd& d0, const ymd& d1)
	{
		auto dy = d1.year() - d0.year();
		auto dm = d1.month() - d0.month();
		auto dd = d1.day() - d0.day();

		// months are computed modulo 12
		int md = d1.month() >= d0.month() ? dm.count() : dm.count() - 12;

		return dy.count() + md / 12. + dd.count() / 360. ;
	}

#ifdef _DEBUG

#pragma warning(push)
#pragma warning(disable: 4455)
	inline int test_dcf_30_360()
	{
		using std::literals::chrono_literals::operator""y;
		{
			static_assert(dcf_30_360(2023y/1/1, 2023y/1/1) == 0);
			static_assert(dcf_30_360(2023y/1/1, 2023y/1/2) == 1/360.);
			//!!! more tests
		}

		return 0;
	}
#pragma warning(pop)

#endif // _DEBUG

}