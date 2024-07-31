// tmx_date.h - Convert year/month/day to time_t and back.
#pragma once
#ifdef _DEBUG
#include <cassert>
#endif // _DEBUG
#include <tuple>
#include <chrono>

namespace tmx::date {

	using std::literals::chrono_literals::operator""y;
	using ymd = std::chrono::year_month_day; // E.g., ymd d = 2024y / 5 / 6

	constexpr void swap(ymd& lhs, ymd& rhs) {
		ymd tmp = lhs;
		lhs = rhs;
		rhs = tmp;
	}

	// Use std::chrono constants.
	constexpr time_t seconds_per_year = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::years{ 1 }).count();
	constexpr int seconds_per_day = 24 * 60 * 60;

	// Time in seconds between d0 and d1
	constexpr time_t operator-(const ymd& d1, const ymd& d0)
	{
		using namespace std::chrono;

		return duration_cast<seconds>(sys_days(d1) - sys_days(d0)).count();
	}
#ifdef _DEBUG
	static_assert(2023y / 4 / 5 - 2023y / 4 / 5 == 0);
	static_assert(2023y / 4 / 6 - 2023y / 4 / 5 == seconds_per_day);
#endif // _DEBUG

	// TODO: use sys_days instead of ymd?
	constexpr double diffyears(const ymd& d1, const ymd& d0)
	{
		return static_cast<double>(d1 - d0) / seconds_per_year;
	}

	// d0 + (d1 - d0) = d1
	constexpr ymd operator+(const ymd& d, time_t s)
	{
		using namespace std::chrono;

		return ymd{ floor<days>(sys_days(d) + seconds(s)) };
	}
#ifdef _DEBUG
	static_assert(2023y / 4 / 5 + (2023y / 4 / 6 - 2023y / 4 / 5) == 2023y / 4 / 6);
	static_assert(2023y / 4 / 5 + (2023y / 5 / 6 - 2023y / 4 / 5) == 2023y / 5 / 6);
	static_assert(2023y / 4 / 5 + (2024y / 5 / 6 - 2023y / 4 / 5) == 2024y / 5 / 6);
#endif // _DEBUG

	// Break down ymd to tuple.
	constexpr auto split(const ymd& d) // spread?
	{
		return std::tuple<int, unsigned, unsigned>(d.year(), d.month(), d.day());
	}

	// Difference in days from d0 to d1.
	constexpr int diffdays(const ymd& d1, const ymd& d0)
	{
		using namespace std::chrono;

		return (sys_days(d1) - sys_days(d0)).count();
	}

} // namespace tmx::date
