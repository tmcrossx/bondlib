// tmx_date_periodic.h - periodic dates
#pragma once
#include "fms_iterable/fms_iterable.h"
#include "tmx_date.h"

// name, months, description
#define TMX_DATE_FREQUENCY(X) \
	X(missing, 0, "Missing frequency.") \
	X(annually, 12, "Yearly payments.") \
	X(semiannually, 6, "biannual payments.") \
	X(quarterly, 3, "quarterly payments.") \
	X(monthly, 1, "monthly payments.") \

namespace tmx::date {

	using sys_days = std::chrono::sys_days;

#define TMX_DATE_FREQUENCY_ENUM(a, b, c) a = b,
	enum frequency {
		TMX_DATE_FREQUENCY(TMX_DATE_FREQUENCY_ENUM)
	};
#undef TMX_DATE_FREQUENCY_ENUM

	// Sequence of dates after b at frequency f up to and including e.
	constexpr auto periodic(frequency f, ymd b, ymd e)
	{
		using namespace std::chrono;
		using namespace fms::iterable;

		int n = 0;
		while (b < e) {
			e -= months(f);
			++n;
		}
		e += months(f);

		return take(sequence(e, months(f)), n);
	}
	constexpr auto x2 = *periodic(frequency::monthly, (2024y / 4 / 1), (2025y / 12 / 1));
	/*
	template<class Rep, class Period>
	constexpr auto periodic(frequency f, ymd b, ymd e)
	{
		return 0;// periodic(f, sys_days(b), sys_days(e));
	}
	*/
#ifdef _DEBUG
	static int periodic_test()
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
#if 0
	//*/
	// iterable periodic dates from beginning to end at frequency.
	class periodic {
		frequency f;
		ymd b, e;
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = ymd;
		using difference_type = std::ptrdiff_t;

		constexpr periodic(const frequency& f, const ymd& b, const ymd& e = ymd{})
			: f(f), b(b), e(e)
		{ }

		bool operator==(const periodic& p) const = default;

		periodic begin() const
		{
			return *this;
		}
		periodic end() const
		{
			return periodic(f, e, e);
		}

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
			b += to_period(f);

			return *this;
		}
		constexpr periodic operator++(int)
		{
			auto tmp{ *this };

			operator++();

			return tmp;
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
			}

			return 0;
		}
#endif // _DEBUG
	};

	// Return first payment date after effective date and number of payments working backward from termination date.
	constexpr std::tuple<ymd, int> first_payment_date(frequency f, ymd effective, ymd termination)
	{
		int n = -1 + diffmonths(termination, effective) / to_period(f).count();

		return { termination - std::chrono::months(n * to_period(f).count()), n + 1 };
	}
	constexpr auto xxx = first_payment_date(frequency::semiannually,
		2024y / 5 / 6, 2025y / 5 / 6);
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
#endif // 0

} // namespace tmx::date
