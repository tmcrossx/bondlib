// tmx_bond.h - Bonds
#pragma once
#include <algorithm>
#include <vector>
#include "tmx_date_day_count.h"
#include "tmx_instrument.h"
#include "tmx_valuation.h"


namespace tmx::bond {

	// Basic bond indicative data.
	template<class C = double>
	struct basic 
	{
		date::ymd dated; // when interest starts accruing
		date::ymd maturity; // 
		C coupon; // not in percent
		date::frequency frequency = date::frequency::semiannually;
		date::day_count_t day_count = date::day_count_isma30360;
		C face = 100;
	};

	// Return cash flows for basic bond from present value date.
	template<class C>
	inline auto fix(const basic<C>& bond, const date::ymd& pvdate)
	{
		using namespace fms::iterable;

		const auto d0 = std::max(bond.dated, pvdate);
		const auto [d, _] = date::first_payment_date(bond.frequency, d0, bond.maturity);
		const auto t = date::periodic(bond.frequency, d, bond.maturity);
		const auto dt = nabla(concatenate(singleton(pvdate), t), bond.day_count);

		const auto u = apply([pvdate](auto ymd) { return ymd - pvdate; }, t);
		const auto c = constant(bond.face * bond.coupon) * dt;
		
		const auto f = instrument::zero_coupon_bond(bond.maturity - pvdate, bond.face);

		return cache(merge(instrument::iterable(u, c), f));
	}
#ifdef _DEBUG

	inline int basic_test()
	{
		using namespace std::literals::chrono_literals;
		using namespace std::chrono;
		using namespace date;
		auto d = 2023y / 1 / 1;
		bond::basic<> bond{ d, d + years(10), 0.05 };

		{
			auto i = fix(bond, d);
			assert(20 == length(i));
			auto c0 = *i;
			assert(c0.c == 2.5);
			assert(c0.u == d + date::period(bond.frequency) - d);

			i = skip(i, 19);
			auto cn = *i;
			assert(cn.u = bond.maturity - bond.dated);
			assert(cn.c == 102.5);
		}
		{
			auto pvdate = d + months(1);
			auto i = fix(bond, pvdate);
			assert(20 == length(i));
			auto c0 = *i;
			assert(c0.c != 2.5);
			assert(c0.u == bond.dated + date::period(bond.frequency) - pvdate);

			++i;
			auto c1 = *i;
			assert(c1.c == 2.5);
			i = skip(i, 18);
			auto cn = *i;
			assert(cn.u = bond.maturity - pvdate);
			assert(cn.c == 102.5);
		}

		return 0;
	}

#endif // _DEBUG

#if 0

	// fix(basic bond, date::ymd pvdate, ...) -> instrument::interface

	// Return instrument cash flows for unit notional with time based on issue date.
	template<class C = double>
	class basic_instrument : public instrument::interface<instrument::cash_flow<U,C>> {
		const basic<C>& bond;
		date::ymd dated, issue;
		date::ymd maturity;
		std::chrono::months period;
		date::ymd d0, d1; // current period
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = std::pair<double, C>;

		basic_instrument(const basic<C>& bond, const date::ymd& dated, date::ymd issue = date::ymd{})
			: bond(bond), dated(dated), issue(issue.ok() ? issue : dated), maturity(date::addyears(issue, bond.maturity)), period(date::period(bond.frequency))
		{
			if (!issue.ok()) {
				issue = dated;
			}
			// Work back from maturity.
			d0 = maturity;
			while (d0 >= dated) { // TODO: d0 -= tenor*frequency
				d1 = d0;
				d0 -= period;
			}
		}
		explicit operator bool() const
		{
			return d1 <= maturity;
		}
		value_type operator*() const
		{
			C c = bond.coupon * bond.day_count(d0, d1);
			if (d1 == maturity) {
				c += bond.redemption;
			}
			return { date::diffyears(issue, d1), c };
		}
		basic_instrument& operator++()
		{
			d0 = d1;
			d1 += period;

			return *this;
		}
	};
	/*
	template<class C>
	constexpr auto accrued(const basic<C>& bond, const date::ymd& dated, const date::ymd& valuation)
	{
		auto d = dated;

		while (d + bond.frequency <= valuation) {
			d += bond.frequency;
		}

		return bond.coupon * bond.day_count_fraction(d, valuation);
	}
	//!!! add tests
	*/
#endif // 0
} // namespace tmx::bond

// class callable : public basic { ... };

