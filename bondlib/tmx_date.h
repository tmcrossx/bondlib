// tmx_date.h - Date and time calculation
#pragma once
#ifdef _DEBUG
#include <cassert>
#endif // _DEBUG
#include <tuple>
#include <chrono>

namespace tmx::date {

	// Calendar year/month/day date.
	using ymd = std::chrono::year_month_day;
	using clock = std::chrono::system_clock; // Default clock.
	using years = std::chrono::years;

	constexpr double seconds_per_year = static_cast<double>(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::years{ 1 }).count());
	constexpr double seconds_per_day = static_cast<double>(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::days{ 1 }).count());
	constexpr double days_per_year = seconds_per_year / seconds_per_day;

	constexpr ymd to_ymd(int y, unsigned int m, unsigned int d)
	{
		return ymd(std::chrono::year(y), std::chrono::month(m), std::chrono::day(d));
	}
	static_assert(to_ymd(2021, 1, 1) == ymd(std::chrono::year(2021), std::chrono::January, std::chrono::day(1)));
	static_assert(to_ymd(2021, 1, 1) == std::chrono::year(2021)/1/1);

	constexpr std::tuple<int, unsigned, unsigned> from_ymd(const ymd& d)
	{
		return { d.year().operator int(), d.month().operator unsigned int(), d.day().operator unsigned int() };
	}
	static_assert(from_ymd(to_ymd(2021, 1, 1)) == std::tuple(2021, 1u, 1u));

	// time_t cannot be constexpr
	inline time_t to_time_t(const ymd& ymd)
	{
		auto [y, m, d] = from_ymd(ymd);
		tm tm;
		memset(&tm, 0, sizeof(tm));
		tm.tm_year = y - 1900;
		tm.tm_mon = m - 1;
		tm.tm_mday = d;
		tm.tm_isdst = -1; // let gmtime figure it out
#ifdef _WIN32
		time_t t = _mkgmtime(&tm); // user local time to UTC
#else
		time_t t = mkgmtime(&tm); // user local time to UTC
#endif
		if (errno != 0) {
			char err[256];
			strerror_s(err, errno);

			throw std::runtime_error(err);
		}

		return t;
	}
	inline ymd from_time_t(time_t t)
	{
		tm tm;
		errno = 0;
#ifdef _WIN32
		errno = localtime_s(&tm, &t); // UTC to user local time
#else
		localtime_s(&t, &tm); // UTC to user local time
#endif // WIN32
		if (errno != 0) {
			char err[256];
			strerror_s(err, errno);

			throw std::runtime_error(err);
		}

		return to_ymd(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
	}

	// Time in years from d0 to d1.
	inline double diffyears(ymd d1, ymd d0)
	{
		time_t t1 = to_time_t(d1);
		time_t t0 = to_time_t(d0);

		return static_cast<double>(t1 - t0)/seconds_per_year;
	}
	inline ymd addyears(ymd d, double y)
	{
		time_t t = to_time_t(d);
		
		t += static_cast<time_t>(std::round(y * seconds_per_year));
		
		return from_time_t(t);
	}
	

#ifdef _DEBUG
	int test()
	{
		using namespace std::chrono_literals;
		double day = 1 / days_per_year;
		{
			ymd d0 = to_ymd(2023, 4, 5);
			ymd d1 = to_ymd(2024, 4, 5);
			double dy = diffyears(d1, d0);
			assert(fabs(dy - 1) <= day);
		}
		{
			ymd d0 = 2023y / 4 / 5;
			ymd d1 = addyears(d0, 1);
			double dy = diffyears(d1, d0);
			assert(fabs(dy - 1) <= day);
			ymd d2 = addyears(d0, dy);
			assert(d1 == addyears(d0, dy));
		}
		// TODO: Add more tests

		return 0;
	}
#endif // _DEBUG

	enum class frequency {
		annually = 1,
		semiannually = 2,
		quarterly = 4,
		monthly = 12,
	};
	constexpr /*std::chrono::months*/int tenor(frequency f)
	{
		return 12 / (int)f;
	}

} // namespace tmx::date
