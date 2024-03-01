// tmx_instrument.h - times and cash flows
#pragma once
#include <algorithm>
#include <concepts>
#include <span>
#include <vector>

namespace tmx {

	template<class U, class C>
	struct instrument {
		virtual ~instrument() { }

		// Advance to next cash flow strictly after u.
		instrument& advance(const U& u)
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
		instrument& operator++()
		{
			return op_incr();
		}
	private:
		virtual bool op_bool() const = 0;
		virtual std::pair<U, C> op_star() const = 0;
		virtual instrument& op_incr() = 0;
	};

}
