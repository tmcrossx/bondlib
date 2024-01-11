// tms_datetime.h - Time point to double conversion.
#pragma once

#include <chrono>

namespace tmx::datetime {

	using clock = std::chrono::system_clock; // Default clock.
	using years = std::chrono::years;

	constexpr auto seconds_per_year = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::years{1});
	constexpr auto seconds_per_day = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::days{ 1 });
	constexpr double days_per_year = static_cast<double>(seconds_per_year.count()) / seconds_per_day.count();

	template<class Period = std::chrono::milliseconds>
	constexpr double diffyears(const clock::time_point& t1, const clock::time_point& t0)
	{
		auto dt = std::chrono::duration_cast<Period>(t1 - t0);
		auto dy = std::chrono::duration_cast<Period>(std::chrono::years{ 1 });
		
		return static_cast<double>(dt.count()) / dy.count();
	}
	inline double diffyears(time_t t1, time_t t0)
	{
		return diffyears(clock::from_time_t(t1), clock::from_time_t(t0));
	}

#ifdef _DEBUG
	inline int test()
	{
		auto now = clock::now();
		auto t0 = clock::to_time_t(now);
		auto t1 = clock::to_time_t(now + years(1));
		auto y = diffyears(t1, t0); // time_t
		assert(std::fabs(y - 1) <= 1 / days_per_year);

		double s = diffyears(now + std::chrono::seconds(1), now); // time_point
		double es = 1. / seconds_per_year.count();
		assert(s == es);

		return 0;
	}
#endif // _DEBUG

} // namespace tmx::datetime