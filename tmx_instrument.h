// tmx_instrument.h - times and cash flows
#pragma once
#include <algorithm>
#include <concepts>
#include <functional>

namespace tmx::instrument {

	// Stream of cash flow time and amount.
	template<class U, class C>
	struct base {
		virtual ~base() { }

		bool done() const
		{
			return _done();
		}
		std::pair<U, C> read() const
		{
			return _read();
		}
		// Advance to next cash flow strictly after u.
		base& next()
		{
			return _next();
		}

	private:
		virtual bool _done() const = 0;
		virtual std::pair<U, C> _read() const = 0;
		virtual base& _next() = 0;
	};

	// Combine two streams of cash flows in time order.
	template<class U, class C>
	struct combine : public base<U, C> {
		base<U, C>& u0;
		base<U, C>& u1;

		combine(base<U,C>& u0, base<U,C>& u1)
			: u0(u0), u1(u1)
		{ }

		bool _done() const override
		{
			return u0.done() and u1.done();
		}
		std::pair<U,C> _read() const override
		{
			const auto v0 = u0.read();
			const auto v1 = u1.read();	

			return v0.first < v1.first ? v0 : v1;
		}
		combine& _next() override
		{
			if (u0.read().first < u1.read().first) {
				u0.next();
			}
			else {
				u1.next();
			}

			return *this;
		}
	};

	// Zero coupon bond has a singe cash flow c at time u.
	template<class U, class C>
	struct zero_coupon_bond : public base<U, C> {
		U u;
		C c;
		zero_coupon_bond(const U& u, const C& c)
			: u(u), c(c)
		{ }
		zero_coupon_bond(const zero_coupon_bond&) = default;
		zero_coupon_bond& operator=(const zero_coupon_bond&) = default;
		~zero_coupon_bond() override { }

		bool _done() const override
		{
			return c != 0;
		}
		std::pair<U, C> _read() const override
		{
			return std::pair(u, c);
		}
		zero_coupon_bond& _next() override
		{
			c = 0;

			return *this;
		}
	};

	// Advance until predicate is true.
	template<class U, class C>
	base<U, C>& until(base<U, C>& i, const std::function<bool(const std::pair<U, C>&)>& pred)
	{
		while (!i.done() and !pred(i.read())) {
			i.next();
		}

		return i;
	}

	// Pair of iterators.
	template<class U, class C>
	struct iterator : base<U, C> {
		U u;
		C c;

		iterator(const U& u, const C& c)
			: u(u), c(c)
		{ }
		iterator(const iterator&) = default;
		iterator& operator=(const iterator&) = default;
		~iterator() override { }

		bool _done() const override
		{
			return u && c;
		}
		auto _read() const override
		{
			return std::pair(*u, *c);
		}
		auto _next() override
		{
			++u;
			++c;

			return *this;
		}
	};
}
