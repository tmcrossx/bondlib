// tmx_date_periodic.h - periodic dates
#pragma once
#include "../date/tmx_date.h"

// TMX_FREQUENCY_, date::frequency, description
#define TMX_DATE_FREQUENCY(X) \
	X(MISSING, missing, 0, "Missing frequency.") \
	X(ANNUALLY, annually, 1, "Yearly payments.") \
	X(SEMIANNUALLY, semiannually, 2, "biannual payments.") \
	X(QUARTERLY, quarterly, 4, "quarterly payments per year.") \
	X(MONTHLY, monthly, 12, "monthly payments per year.") \

namespace tmx::date {

#define TMX_DATE_FREQUENCY_ENUM(a, b, c, d) b = c,
	enum class frequency {
		TMX_DATE_FREQUENCY(TMX_DATE_FREQUENCY_ENUM)
	};
#undef TMX_DATE_FREQUENCY_ENUM
	// Convert frequency to period in months.
	constexpr std::chrono::months period(frequency f)
	{
		return std::chrono::months(12 / static_cast<int>(f));
	}

	// iterable periodic dates from beginning to end at frequency.
	class periodic {
		std::chrono::months m;
		ymd b, e;
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = ymd;
		using difference_type = std::ptrdiff_t;

		constexpr periodic(const frequency& f, const ymd& b, const ymd& e = ymd{})
			: m(period(f)), b(b), e(e)
		{ }
		static_assert(!ymd{}.ok());
		static_assert(ymd{} == ymd{});

		bool operator==(const periodic& p) const = default;

		constexpr explicit operator bool() const
		{
			return e.ok() ? b <= e : true;
		}
		constexpr value_type operator*() const
		{
			return b;
		}
		constexpr periodic& operator++()
		{
			b += m;

			return *this;
		}
		constexpr periodic& operator--()
		{
			b -= m;

			return *this;
		}
#ifdef _DEBUG
		static int test()
		{
			{
				ymd d = 2024y / 5 / 6;
				periodic p(frequency::annually, d);
				assert(p);
				assert(*p == 2024y / 5 / 6);
				++p;
				assert(*p == 2025y / 5 / 6);
				assert(p);
			}
			{
				ymd d = 2024y / 11 / 6;
				periodic p(frequency::monthly, d);
				assert(p);
				assert(*p == 2024y / 11 / 6);
				++p;
				assert(*p == 2024y / 12 / 6);
				++p;
				assert(*p == 2025y / 1 / 6);
				++p;
				assert(*p == 2025y / 2 / 6);
				assert(p);
				--p;
				assert(*p == 2025y / 1 / 6);
			}

			return 0;
		}
#endif // _DEBUG
	};

	// Return first payment date after effective date and number of payments working backward from termination date.
	constexpr std::pair<ymd, int> first_payment_date(frequency f, ymd effective, ymd termination)
	{
		int n = 0;
		periodic p(f, termination);

		while (*p > effective) {
			--p;
			++n;
		}
		++p;

		return { *p, n };
	}
	static_assert(first_payment_date(frequency::annually, 
		2024y / 5 / 6, 
		2025y / 5 / 6) == std::tuple(
		2025y / 5 / 6, 1));
	static_assert(first_payment_date(frequency::semiannually, 
		2024y / 5 / 6, 
		2025y / 5 / 6) == std::tuple(
		2024y / 11 / 6, 2));
	static_assert(first_payment_date(frequency::quarterly, 
		2024y / 5 / 6, 
		2025y / 5 / 6) == std::tuple(
		2024y / 8 / 6, 4));
	static_assert(first_payment_date(frequency::monthly, 
		2024y / 5 / 6, 
		2025y / 5 / 6) == std::tuple(
		2024y / 6 / 6, 12));

	static_assert(first_payment_date(frequency::annually, 
		2024y / 4 / 6, 
		2025y / 5 / 6) == std::tuple(
		2024y / 5 / 6, 2));
	static_assert(first_payment_date(frequency::semiannually, 
		2024y / 4 / 6, 
		2025y / 5 / 6) == std::tuple(
		2024y / 5 / 6, 3));
	static_assert(first_payment_date(frequency::quarterly, 
		2024y / 4 / 6, 
		2025y / 5 / 6) == std::tuple(
		2024y / 5 / 6, 5));
	static_assert(first_payment_date(frequency::monthly, 
		2024y / 4 / 6, 
		2025y / 5 / 6) == std::tuple(
		2024y / 5 / 6, 13));

	static_assert(first_payment_date(frequency::annually, 
		2024y / 6 / 6, 
		2025y / 5 / 6) == std::tuple(
		2025y / 5 / 6, 1));
	static_assert(first_payment_date(frequency::semiannually, 
		2024y / 6 / 6, 
		2025y / 5 / 6) == std::tuple(
		2024y / 11 / 6, 2));
	static_assert(first_payment_date(frequency::quarterly, 
		2024y / 6 / 6, 
		2025y / 5 / 6) == std::tuple(
		2024y / 8 / 6, 4));
	static_assert(first_payment_date(frequency::monthly, 
		2024y / 6 / 6, 
		2025y / 5 / 6) == std::tuple(
		2024y / 7 / 6, 11));


} // namespace tmx::date
