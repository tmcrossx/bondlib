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
		{
			while (*this && **this <= u) {
				++*this;
			}

			return *this;
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
