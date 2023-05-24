#pragma once
#include <chrono>

namespace tmx {

	using ymd = std::chrono::year_month_day;

	constexpr auto dcf_30_360(const ymd& d0, const ymd& d1)
	{
		auto dy = d1.year() - d0.year();
		auto dm = d1.month() - d0.month();
		auto dd = d1.day() - d0.day();

		return dy.count() + dm.count() / 30. + dd.count() / 360.;
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

	constexpr double dcf_actual_360(const ymd& d0, const ymd& d1)
	{
		return (std::chrono::sys_days(d1) - std::chrono::sys_days(d0)).count() / 360.;
	}

#ifdef _DEBUG

#pragma warning(push)
#pragma warning(disable: 4455)
	inline int test_dcf_actual_360()
	{
		using std::literals::chrono_literals::operator""y;
		{
			static_assert(dcf_actual_360(2023y / 1 / 1, 2023y / 1 / 1) == 0);
			static_assert(dcf_actual_360(2023y / 1 / 1, 2023y / 1 / 2) == 1 / 360.);
			//!!! more tests
		}

		return 0;
	}
#pragma warning(pop)

#endif // _DEBUG


	constexpr double dcf_actual_365(const ymd& d0, const ymd& d1)
	{
		return (std::chrono::sys_days(d1) - std::chrono::sys_days(d0)).count() / 365.;
	}

#ifdef _DEBUG

#pragma warning(push)
#pragma warning(disable: 4455)
	inline int test_dcf_actual_365()
	{
		using std::literals::chrono_literals::operator""y;
		{
			static_assert(dcf_actual_365(2023y / 1 / 1, 2023y / 1 / 1) == 0);
			static_assert(dcf_actual_365(2023y / 1 / 1, 2023y / 1 / 2) == 1 / 365.);
			//!!! more tests
		}

		return 0;
	}
#pragma warning(pop)

#endif // _DEBUG

}