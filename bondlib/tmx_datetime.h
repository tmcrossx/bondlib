// tms_datetime.h - date and time functions
#pragma once

#include <chrono>

namespace tmx::datetime {

	using clock = std::chrono::system_clock;
	using years = std::chrono::years;

	// duration as double in years
	constexpr double days_per_year = 146097./400;
	constexpr double seconds_per_day = 24 * 60 * 60;

	inline auto diffseconds(clock::time_point tp1, clock::time_point tp0)
	{
		return std::chrono::duration_cast<std::chrono::seconds>(tp1 - tp0);
	}

	inline double diffyears(time_t t1, time_t t0)
	{
		auto tp1 = clock::from_time_t(t1);
		auto tp0 = clock::from_time_t(t0);

		return diffseconds(tp1, tp0).count() / (seconds_per_day * days_per_year);
	}

} // namespace tmx::datetime