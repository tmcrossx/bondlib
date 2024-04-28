// tmx_bond.h - Bonds
#pragma once
#include <algorithm>
#include <vector>
#include "tmx_date_day_count.h"
#include "tmx_instrument.h"
#include "tmx_valuation.h"


namespace tmx::bond {

	// Elementary bond.
	template<class U = double, class C = double>
	inline auto elementary(const U& maturity, const C& coupon = 0.05, size_t frequency = 2)
	{
		using namespace fms::iterable;
		
		const U period = 1. / frequency;
		const size_t count = static_cast<size_t>(maturity * frequency);

		auto u = constant(period) * iota(U(1));
		auto c = constant(coupon / frequency);
		auto i = instrument::iterable(u, c);

		return merge(take(i, count), instrument::zero_coupon_bond(maturity, C(1)));
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
			assert(*e == instrument::cash_flow(10., 1.025));
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
		C face = 1;
	};


	// Return cash flows for basic bond from present value date.
	template<class C>
	inline auto fix(const basic<C>& bond, const date::ymd& pvdate)
	{
		using namespace fms::iterable;

		const auto [d0, _] = date::first_payment_date(bond.frequency, bond.dated < pvdate ? pvdate : bond.dated, bond.maturity);
		auto t = date::periodic(bond.frequency, d0, bond.maturity);
		auto u = apply([pvdate](auto ymd) { return date::diffyears(ymd, pvdate); }, t);
		auto c = fms::iterable::constant(bond.face * bond.coupon) * fms::iterable::nabla(concatenate(singleton(pvdate), t), bond.day_count);

		return cache(instrument::iterable(u, c));// , instrument::zero_coupon_bond(diffyears(bond.maturity, pvdate), bond.face);
	}
#ifdef _DEBUG

	inline int basic_test()
	{
		{
			using namespace std::literals::chrono_literals;
			using namespace date;

			auto d = 2023y / 1 / 1;
			bond::basic<> bond{ d, d + std::chrono::years(10), 0.05, frequency::semiannually, day_count_isma30360 };
			auto i = fix(bond, d + std::chrono::months(1));
			//tmx::instrument::value v(i);
			//assert(20 == length(i));
			auto c0 = *i;
			++i;
			auto c2 = *i;
			i = skip(i, 18);
			auto cn = *i;
			/*
			auto u = i.time();
			auto c = i.cash();
			assert(u[0] != 0);
			assert(c[0] == 0.05 / 2);
			assert(std::fabs(-c[19] + c[0] + 1) < 1e-15);
			assert(std::fabs(-u[19] + 10) <= 1 / date::days_per_year);

			bond::basic<> bond2{ 10, 0.05 };
			auto i2 = instrument(bond2, d);
			assert(i.size() == i2.size());
			auto u2 = i2.time();
			auto c2 = i2.cash();
			for (size_t n = 0; n < i.size(); ++n) {
				assert(u[n] == u2[n]);
				assert(c[n] == c2[n]);
			}
			*/
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

