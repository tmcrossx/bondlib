// tmx_instrument.h - times and cash flows
#pragma once
#ifdef _DEBUG
#include <cassert>
#endif // _DEBUG
#include <functional>
#include <limits>

namespace tmx::instrument {

	// Stream of cash flow time and amount.
	template<class U = double, class C = double>
	struct base {
		virtual ~base() { }

		explicit operator bool() const
		{
			return op_bool();
		}
		std::pair<U, C> operator*() const
		{
			return op_star();
		}
		// Advance to next cash flow strictly after u.
		base& operator++()
		{
			return op_incr();
		}

	private:
		virtual bool op_bool() const = 0;
		virtual std::pair<U, C> op_star() const = 0;
		virtual base& op_incr() = 0;
	};

	// Zero coupon bond has a singe cash flow c at time u.
	template<class U = double, class C = double>
	class zero_coupon_bond : public base<U, C> {
		U u;
		C c;
		static constexpr U infty = std::numeric_limits<U>::infinity();
	public:
		zero_coupon_bond(const U& u = infty, const C& c = 0)
			: u(u), c(c)
		{ }
		zero_coupon_bond(const zero_coupon_bond&) = default;
		zero_coupon_bond& operator=(const zero_coupon_bond&) = default;
		~zero_coupon_bond() override { }

		bool op_bool() const override
		{
			return u != infty;
		}
		std::pair<U, C> op_star() const override
		{
			return std::pair(u, c);
		}
		zero_coupon_bond& op_incr() override
		{
			u = infty;
			c = 0;

			return *this;
		}
	};

	// Combine two streams of cash flows in time order.
	template<class U = double, class C = double>
	struct combine : public base<U, C> {
		base<U, C>& u0;
		base<U, C>& u1;

		combine(base<U,C>& u0, base<U,C>& u1)
			: u0(u0), u1(u1)
		{ }

		bool op_bool() const override
		{
			return u0 || u1;
		}
		std::pair<U,C> op_star() const override
		{
			return (*u0).first < (*u1).first ? *u0 : *u1;
		}
		combine& op_incr() override
		{
			if ((*u0).first < (*u1).first) {
				++u0;
			}
			else {
				++u1;
			}

			return *this;
		}
#ifdef _DEBUG
		static int test()
		{
			{
				zero_coupon_bond z(1, 2);
				assert(z);
				assert(*z == std::pair(1, 2));
				++z;
				assert(!z);
			}
			{
				zero_coupon_bond z0(1., 2.), z1(3., 4.);
				combine i(z0, z1);
				assert(i);
				assert(*i == std::pair(1., 2.));
				++i;
				assert(i);
				assert(*i == std::pair(3., 4.));
				++i;
				assert(!i);
			}
			{
				zero_coupon_bond z0(1., 2.), z1(3., 4.);
				combine i(z1, z0);
				assert(i);
				assert(*i == std::pair(1., 2.));
				++i;
				assert(i);
				assert(*i == std::pair(3., 4.));
				++i;
				assert(!i);
			}

			return 0;
		}
#endif // _DEBUG
	};

	/*
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

		bool op_bool() const override
		{
			return u and c;
		}
		std::pair<U::value_type,C::value_type> op_star() const override
		{
			return std::pair(*u, *c);
		}
		auto op_incr() override
		{
			++u;
			++c;

			return *this;
		}
	};
	*/
}
