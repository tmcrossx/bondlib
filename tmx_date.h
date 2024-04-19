// tmx_date.h - Convert year/month/day to time_t and back.
#pragma once
#ifdef _DEBUG
#include <cassert>
#endif // _DEBUG
#include <tuple>
#include <chrono>

namespace tmx::date {

	// Use calendar year/month/day from <chrono> for dates.
	using ymd = std::chrono::year_month_day;

	// Short names for constants from <chrono>	
	constexpr time_t seconds_per_year = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::years{ 1 }).count();
	constexpr time_t seconds_per_day = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::days{ 1 }).count();
	constexpr double days_per_year = static_cast<double>(seconds_per_year) / seconds_per_day;

	// Broken down date to ymd.
	constexpr ymd to_ymd(int y, unsigned int m, unsigned int d)
	{
		return ymd(std::chrono::year(y), std::chrono::month(m), std::chrono::day(d));
	}
	static_assert(to_ymd(2023, 4, 5) == std::chrono::year(2023) / 4 / 5);

	// Break down ymd to tuple.
	constexpr auto from_ymd(const ymd& d)
	{
		return std::tuple<int, unsigned, unsigned>(d.year(), d.month(), d.day());
	}
	static_assert(from_ymd(to_ymd(2023, 4, 5)) == std::tuple(2023, 4u, 5u));

	// year/month/day to UTC time_t
	constexpr time_t to_time_t(const ymd& ymd)
	{
		return std::chrono::sys_days(ymd).time_since_epoch().count() * seconds_per_day;
	}
	// Not guaranteed to be true.
	static_assert(to_time_t(to_ymd(1970, 1, 1)) == 0);

	// UTC time_t to year/month/day
	inline ymd from_time_t(time_t t)
	{
		return ymd{ std::chrono::floor<std::chrono::days>(std::chrono::system_clock::from_time_t(t)) };
	}
	// static_assert(from_time_t(0) == to_ymd(1970, 1, 1)); // not constexpr
#ifdef _DEBUG
	inline int from_time_t_test()
	{
		assert(from_time_t(0) == to_ymd(1970, 1, 1));
		assert(from_time_t(seconds_per_day) == to_ymd(1970, 1, 2));
		assert(from_time_t(-seconds_per_day) == to_ymd(1969, 12, 31));

		return 0;
	}
#endif // _DEBUG

	// Time in seconds from d0 to d1.
	constexpr time_t diffseconds(ymd d1, ymd d0)
	{
		return to_time_t(d1) - to_time_t(d0);
	}
#ifdef _DEBUG
	static_assert(diffseconds(to_ymd(2023, 4, 5), to_ymd(2023, 4, 4)) == seconds_per_day);
	static_assert(diffseconds(to_ymd(2023, 4, 5), to_ymd(2023, 4, 6)) == -seconds_per_day);
#endif // _DEBUG

	// Time in days from d0 to d1.
	constexpr double diffdays(ymd d1, ymd d0)
	{
		return static_cast<double>(diffseconds(d1, d0)) / seconds_per_day;
	}
	// Time in years from d0 to d1.
	constexpr double diffyears(ymd d1, ymd d0)
	{
		return static_cast<double>(diffseconds(d1, d0)) / seconds_per_year;
	}

	inline ymd adddays(ymd d, double ds)
	{
		return from_time_t(to_time_t(d) + static_cast<time_t>(ds * seconds_per_day));
	}
	inline ymd addyears(ymd d, double ys)
	{
		return from_time_t(to_time_t(d) + static_cast<time_t>(ys * seconds_per_year));
	}

#ifdef _DEBUG
	int test()
	{
		using namespace std::chrono_literals;
		double day = 1 / days_per_year;
		{
			ymd d0 = to_ymd(2023, 4, 5);
			ymd d1 = 2024y / 4 / 5;
			double dy = diffyears(d1, d0);
			assert(fabs(dy - 1) <= day);
		}
		{
			ymd d0 = 2023y / 4 / 5;
			ymd d1 = addyears(d0, 1);
			double dy = diffyears(d1, d0);
			assert(fabs(dy - 1) <= day);
			ymd d2 = addyears(d0, dy);
			assert(d1 == d2);
		}
		// TODO: Add more tests

		return 0;
	}
#endif // _DEBUG

#define TMX_DATE_FREQUENCY(X) \
	X(annually,     1) \
	X(semiannually, 2) \
	X(quarterly,    4) \
	X(monthly,     12) \

#define TMX_DATE_FREQUENCY_ENUM(E, N) E = N,
	enum class frequency {
		TMX_DATE_FREQUENCY(TMX_DATE_FREQUENCY_ENUM)
	};
#undef TMX_DATE_FREQUENCY_ENUM

	// Convert frequency to period in months.
	constexpr std::chrono::months period(frequency f)
	{
		return std::chrono::months(12 / static_cast<int>(f));
	}

	// iterable periodic dates
	class periodic {
		ymd d;
		std::chrono::months m;
	public:
		constexpr periodic(const ymd& d, const frequency& f)
			: d(d), m(period(f))
		{ }

		constexpr explicit operator bool() const
		{
			return true;
		}
		constexpr ymd operator*() const
		{
			return d;
		}
		constexpr periodic& operator++()
		{
			d += m;

			return *this;
		}
		constexpr periodic& operator--()
		{
			d -= m;

			return *this;
		}
#ifdef _DEBUG
		static int test()
		{
			using std::literals::chrono_literals::operator""y;
			{
				ymd d = 2024y / 5 / 6;
				periodic p(d, frequency::annually);
				assert(p);
				assert(*p == 2024y / 5 / 6);
				++p;
				assert(*p == 2025y / 5 / 6);
				assert(p);
			}
			{
				ymd d = 2024y / 11 / 6;
				periodic p(d, frequency::monthly);
				assert(p);
				assert(*p == 2024y / 11 / 6);
				++p;
				assert(*p == 2024y / 12 / 6);
				++p;
				assert(*p == 2025y / 1 / 6);
				++p;
				assert(*p == 2025y / 2 / 6);
				assert(p);
			}

			return 0;
		}
#endif // _DEBUG
	};

	// Work backward from termination/maturity to first calculation/coupon date after effective/issue date.
	constexpr ymd first_calculation_date(ymd effective, ymd termination, frequency f)
	{
		periodic p(termination, f);

		while (*p > effective) {
			--p;
		}

		return *++p;
	}

} // namespace tmx::date

// Date difference in years
constexpr double operator-(const tmx::date::ymd& d1, const tmx::date::ymd& d0)
{
	return tmx::date::diffyears(d1, d0);
}
inline tmx::date::ymd operator+(const tmx::date::ymd& d, double y)
{
	return tmx::date::addyears(d, y);
}
