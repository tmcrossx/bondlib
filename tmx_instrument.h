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

		// last cash flow
		std::pair<U, C> back() const
		{
			return _back();
		}

		explicit operator bool() const
		{
			return _op_bool();
		}
		value_type operator*() const
		{
			return _op_star();
		}
		base& operator++()
		{
			return _op_incr();
		}
		base operator++(int)
		{
			base b = *this;

			++*this;

			return b;
		}

	private:
		virtual std::pair<U, C> _back() const = 0;
		virtual bool _op_bool() const = 0;
		virtual value_type _op_star() const = 0;
		virtual base& _op_incr() = 0;
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

		std::pair<U, C> _back() const override
		{
			return { u, c };
		}
		bool _op_bool() const override
		{
			return !done;
		}
		value_type _op_star() const override
		{
			return { u, c };
		}
		zero_coupon_bond& _op_incr() override
		{
			done = true;

			return *this;
		}
	};
}
