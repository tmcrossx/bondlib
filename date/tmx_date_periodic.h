// tmx_date_periodic.h - periodic dates
#pragma once
#include "fms_iterable/fms_iterable.h"
#include "tmx_date.h"

// NAME, name, months, description
#define TMX_DATE_FREQUENCY(X) \
	X(NONE, none, 0, "No payments.") \
	X(ANNUALLY, annually, 1, "Yearly payments.") \
	X(SEMIANNUALLY, semiannually, 2, "biannual payments.") \
	X(QUARTERLY, quarterly, 4, "quarterly payments.") \
	X(MONTHLY, monthly, 12, "monthly payments.") \

namespace tmx::date {

#define TMX_DATE_FREQUENCY_ENUM(a, b, c, d) b = c,
	enum frequency {
		TMX_DATE_FREQUENCY(TMX_DATE_FREQUENCY_ENUM)
	};
#undef TMX_DATE_FREQUENCY_ENUM
#define TMX_DATE_FREQUENCY_ENUM(a, b, c, d) if (freq == c) return true;
	constexpr bool is_frequency(int freq)
	{
		TMX_DATE_FREQUENCY(TMX_DATE_FREQUENCY_ENUM)
		return false;
	}
#undef TMX_DATE_FREQUENCY_ENUM
#ifdef _DEBUG
	static_assert(is_frequency(12));
	static_assert(!is_frequency(3));
#endif // _DEBUG

	// Period in months.
	constexpr int period(frequency f)
	{
		return f ? 12 / f : std::numeric_limits<int>::max();
	}

	// Sequence of dates after b at frequency f working backwards from e.
	constexpr auto periodic(frequency f, ymd b, ymd e)
	{
		using months = std::chrono::months;
		using namespace fms::iterable;

		int n = 0;
		int p = period(f);
		while (b < e) {
			e -= months(p); // work backwards from e
			++n;
		}
		const auto fpd = e + months(p); // first payment date after b

		return take(sequence(fpd, months(p)), n);
	}
#ifdef _DEBUG
	constexpr int periodic_test()
	{
		using namespace fms::iterable;
		{
			ymd d0 = 2024y / 5 / 6;
			ymd d1 = 2025y / 5 / 6;

			auto p = periodic(frequency::quarterly, d0, d1);
			assert(equal(p, {2024y / 8 / 6, 2024y / 11 / 6, 2025y / 2 / 6, 2025y / 5 / 6 }));
		}
		{
			ymd d0 = 2024y / 5 / 7;
			ymd d1 = 2025y / 5 / 6;

			auto p = periodic(frequency::quarterly, d0, d1);
			assert(equal(p, { 2024y / 8 / 6, 2024y / 11 / 6, 2025y / 2 / 6, 2025y / 5 / 6 }));
		}
		{
			ymd d0 = 2024y / 5 / 5;
			ymd d1 = 2025y / 5 / 6;

			auto p = periodic(frequency::quarterly, d0, d1);
			assert(equal(p, { 2024y / 5/ 6, 2024y / 8 / 6, 2024y / 11 / 6, 2025y / 2 / 6, 2025y / 5 / 6 }));
		}

		{
			ymd d0 = 2024y / 5 / 6;
			ymd d1 = 2025y / 5 / 6;

			auto p = periodic(frequency::semiannually, d0, d1);
			assert(equal(p, { 2024y / 11 / 6, 2025y / 5 / 6 }));
		}
		{
			ymd d0 = 2024y / 5 / 7;
			ymd d1 = 2025y / 5 / 6;

			auto p = periodic(frequency::semiannually, d0, d1);
			assert(equal(p, { 2024y / 11 / 6, 2025y / 5 / 6 }));
		}
		{
			ymd d0 = 2024y / 5 / 5;
			ymd d1 = 2025y / 5 / 6;

			auto p = periodic(frequency::semiannually, d0, d1);
			assert(equal(p, { 2024y / 5 / 6, 2024y / 11 / 6, 2025y / 5 / 6 }));
		}

		{
			ymd d0 = 2024y / 5 / 6;
			ymd d1 = 2025y / 5 / 6;

			auto p = periodic(frequency::annually, d0, d1);
			assert(equal(p, { 2025y / 5 / 6 }));
		}
		{
			ymd d0 = 2024y / 5 / 7;
			ymd d1 = 2025y / 5 / 6;

			auto p = periodic(frequency::annually, d0, d1);
			assert(equal(p, { 2025y / 5 / 6 }));
		}
		{
			ymd d0 = 2024y / 5 / 5;
			ymd d1 = 2025y / 5 / 6;

			auto p = periodic(frequency::annually, d0, d1);
			assert(equal(p, { 2024y / 5 / 6, 2025y / 5 / 6 }));
		}

		return 0;
	}
#endif // _DEBUG

} // namespace tmx::date
