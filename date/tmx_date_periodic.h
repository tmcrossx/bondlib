// tmx_date_periodic.h - periodic dates
#pragma once
#include "fms_iterable/fms_iterable.h"
#include "tmx_date.h"

#define TMX_DATE_FREQUENCY(X) \
	X(missing, std::chrono::years(0), "Missing frequency.") \
	X(annually, std::chrono::years(1), "Yearly payments.") \
	X(semiannually, std::chrono::months(6), "biannual payments.") \
	X(quarterly, std::chrono::months(3), "quarterly payments.") \
	X(monthly, std::chrono::months(1), "monthly payments.") \
	X(weekly, std::chrono::days(7), "weekly payments.") \
	X(daily, std::chrono::days(1), "daily payments.") \

namespace tmx::date {

	using sys_days = std::chrono::sys_days;
	using common_duration = std::common_type<std::chrono::years, std::chrono::months, std::chrono::days>::type;
	constexpr auto xxx = std::chrono::duration_cast<common_duration>(std::chrono::months(1)).count();

#define TMX_DATE_FREQUENCY_ENUM(a, b, c) a = std::chrono::duration_cast<common_duration>(b).count(),
	enum class frequency {
		TMX_DATE_FREQUENCY(TMX_DATE_FREQUENCY_ENUM)
	};
#undef TMX_DATE_FREQUENCY_ENUM

	static_assert(common_duration((int)frequency::weekly) == std::chrono::days(7));

	// common duration periods to first payment date 
	constexpr int payment_modulus(frequency f, sys_days b, sys_days e)
	{
		using namespace std::chrono;

		return duration_cast<common_duration>(e - b).count() % static_cast<int>(f);
	}
	constexpr auto x1 = payment_modulus(frequency::daily, sys_days(2024y / 5 / 6), sys_days(2025y / 5 / 6));
	static_assert(payment_modulus(frequency::annually, sys_days(2024y / 5 / 6), sys_days(2025y / 5 / 6)) == 584000);
	static_assert(payment_modulus(frequency::semiannually, sys_days(2024y / 5 / 6), sys_days(2025y / 5 / 6)) == 291806);
	static_assert(payment_modulus(frequency::quarterly, sys_days(2024y / 5 / 6), sys_days(2025y / 5 / 6)) == 145709);
	static_assert(payment_modulus(frequency::monthly, sys_days(2024y / 5 / 6), sys_days(2025y / 5 / 6)) == 48311);
	static_assert(payment_modulus(frequency::weekly, sys_days(2024y / 5 / 6), sys_days(2025y / 5 / 6)) == 1600);
	static_assert(payment_modulus(frequency::daily, sys_days(2024y / 5 / 6), sys_days(2025y / 5 / 6)) == 0);

	// Sequence of times from b in steps of dt until past e.
	constexpr auto periodic(frequency f, sys_days b, sys_days e)
	{
		using namespace std::chrono;
		using namespace fms::iterable;

		b += duration_cast<days>(common_duration(payment_modulus(f, b, e)));

		return until([e](auto t) { return t > e; }, sequence(b, duration_cast<days>(common_duration((int)f))));
	}
	constexpr auto x2 = ymd{ *periodic(frequency::quarterly, sys_days(2024y / 5 / 6), sys_days(2025y / 5 / 6)) };
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
		{
			ymd d0 = 2024y / 5 / 6;
			ymd d1 = 2025y / 5 / 6;

			auto p = periodic(frequency::quarterly, sys_days(2024y / 5 / 6), sys_days(2025y / 5 / 6));
			//periodic p(frequency::quarterly, sys_days(2024y / 5 / 6), sys_days(2025y / 5 / 6));
			//periodic p(frequency::annually, d0, d1);
			/*assert(p);
			assert(*p == 2024y / 5 / 6);
			++p;
			assert(*p == 2025y / 5 / 6);
			assert(p);*/
		}
		{
			ymd d = 2024y / 11 / 6;
			auto p = periodic(frequency::monthly, d, d + std::chrono::years(1));
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
