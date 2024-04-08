// tmx_instrument.h - times and cash flows
#pragma once
#include <algorithm>
#include <concepts>
#include <span>
#include <vector>

namespace tmx::instrument {

	template<class U, class C>
	struct base {
		virtual ~base() { }

		// Advance to next cash flow strictly after u.
		base& advance(const U& u)
		base&& iterable() const
		base&& iterable() const
		base&& iterable() const
		base&& iterable() const
		base&& iterable() const
		base&& iterable() const
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
			return op_bool();
		}
		std::pair<U, C> operator*() const
		{
			return op_star();
		}
		base& operator++()
		{
			return op_incr();
		}
	private:
		virtual bool op_bool() const = 0;
		virtual std::pair<U, C> op_star() const = 0;
		virtual base& op_incr() = 0;
	};

}
