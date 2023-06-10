// bondxll.h
#pragma once
#include <chrono>
//#include "../bondlib/tmx_date.h"
#include "xll/xll/xll.h"

#ifndef CATEGORY
#define CATEGORY "TMX"
#endif

namespace xll {

	// https://stackoverflow.com/questions/33964461/handling-julian-days-in-c11-14
	struct excel_clock;

	template <class Duration>
	using excel_time = std::chrono::time_point<excel_clock, Duration>;

	// Excel clock represented as days since 1900-01-00.
	struct excel_clock
	{
		using rep = double;
		using period = std::chrono::days::period;
		using duration = std::chrono::duration<rep, period>;
		using time_point = std::chrono::time_point<excel_clock>;

		static constexpr bool is_steady = false;

		static time_point now() noexcept;

		template <class Duration>
		static auto from_sys(std::chrono::sys_time<Duration> const& tp) noexcept
		{
			return excel_time{ tp - (std::chrono::sys_days{std::chrono::year(1900) / 1 / 0}) };
		}

		template <class Duration>
		static auto to_sys(excel_time<Duration> const& tp) noexcept
		{
			return std::chrono::sys_time{ tp - std::chrono::clock_cast<excel_clock>(std::chrono::sys_days{}) };
		}
	};

	inline excel_clock::time_point excel_clock::now() noexcept
	{
		using namespace std::chrono;
		return clock_cast<excel_clock>(system_clock::now());
	}

	inline auto as_time(double d)
	{
		return excel_time{ excel_clock::duration{d} };
	}
	inline std::chrono::sys_days as_days(double d)
	{
		return std::chrono::time_point_cast<std::chrono::days>(excel_clock::to_sys(as_time(d)));
	}
}