// tmx_bond.h - Bonds
#pragma once
#include <algorithm>
#include <vector>
#include "ensure.h"
#include "tmx_date_day_count.h"
#include "tmx_instrument.h"
#include "tmx_valuation.h"

using namespace fms::iterable;

namespace tmx::bond {

	// Elementary bond.
	template<class U = double, class C = double>
	inline auto elementary(const U& maturity, const C& coupon = 0.05, size_t frequency = 2)
	{
		const U period = 1. / frequency;
		const size_t count = static_cast<size_t>(maturity * frequency);
		const auto uc = [](U u, C c) { return instrument::cash_flow(u, c); };

		return merge(take(binop(uc, constant(period) * iota(U(1)), constant(coupon / frequency)), count), instrument::zero_coupon_bond(maturity, C(1)));
	}
#ifdef _DEBUG
	inline int elementary_test()
	{
		{
			auto e = elementary(10., 0.05, 2);
			assert(20 == length(e));
			assert(*e == instrument::cash_flow(0.5, 0.025));
			e = skip(e, 19);
			assert(e);
			assert(*e == instrument::cash_flow(10, 1.025));
			assert(!++e);
		}

		return 0;
	}
#endif // _DEBUG
	// Basic bond indicative data.
	template<class C = double>
	struct basic 
	{
		date::ymd dated; // when interest starts accruing
		date::ymd maturity; // 
		C coupon; // not in percent
		date::frequency frequency = date::frequency::semiannually;
		date::day_count_t day_count = date::day_count_isma30360;
		C redemption = 1;
	};


	// Return cash flows for basic bond from present value date.
	template<class C>
	inline auto fix(const basic<C>& b, const date::ymd& pvdate)
	{
		return pvdate;
	}


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
#ifdef _DEBUG

	inline int basic_test()
	{
		{
			using namespace std::literals::chrono_literals;
			using namespace date;

			auto d = 2023y / 1 / 1;
			bond::basic<> bond{ 10, 0.05, frequency::semiannually, day_count_isma30360 };
			auto i = instrument(bond, d, d);
			ensure(20 == i.size());
			auto u = i.time();
			auto c = i.cash();
			ensure(u[0] != 0);
			ensure(c[0] == 0.05 / 2);
			ensure(std::fabs(-c[19] + c[0] + 1) < 1e-15);
			ensure(std::fabs(-u[19] + 10) <= 1/date::days_per_year);

			bond::basic<> bond2{ 10, 0.05 };
			auto i2 = instrument(bond2, d);
			ensure(i.size() == i2.size());
			auto u2 = i2.time();
			auto c2 = i2.cash();
			for (size_t n = 0; n < i.size(); ++n) {
				ensure(u[n] == u2[n]);
				ensure(c[n] == c2[n]);
			}
		}

		return 0;
	}

#endif // _DEBUG
#endif // 0
} // namespace tmx::bond

// class callable : public basic { ... };

