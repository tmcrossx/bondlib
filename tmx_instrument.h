// tmx_instrument.h - times and cash flows
#pragma once
#include <algorithm>
#include <vector>
#include <span>
#include "fms_iterable.h"

namespace tmx::instrument {

	// NVI base class for instruments
	template<class U = double, class C = double>
	struct base {
		using iterator_category = std::forward_iterator_tag;
		using value_type = std::pair<U, C>;

		virtual ~base()
		{ }

		// Iterable over time cash pairs.
		base&& iterable() const
		{
			return _iterable();
		}

		std::pair<U, C> back() const
		{
			std::pair<U,C>(0,0);
		}

	private:
		virtual base&& _iterable() const = 0;
	};

	// single cash flow instrument
	template<class U = double, class C = double>
	class zero_coupon_bond : public base<U, C> {
		U u;
		C c;
		bool done = false;
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = std::pair<U, C>;

		zero_coupon_bond(U u = 0, C c = 0)
			: u{ u }, c{ c }
		{ }

#pragma warning(push)
#pragma warning(disable: 4172)
		zero_coupon_bond&& _iterable() const override
		{
			zero_coupon_bond z(u, c);

			return std::move(z);
		}
#pragma warning(pop)	
		std::pair<U, C> _back() const
		{
			return { u, c };
		}
		explicit operator bool() const
		{
			return !done;
		}
		value_type operator*() const
		{
			return { u, c };
		}
		zero_coupon_bond& operator++()
		{
			done = true;

			return *this;
		}
	};
}
