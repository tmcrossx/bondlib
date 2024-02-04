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
		virtual ~base()
		{ }

		size_t size() const
		{
			return _time().size();
		}
		const std::span<U> time() const
		{
			return _time();
		}
		const std::span<C> cash() const
		{
			return _cash();
		}
		// last cash flow
		std::pair<U, C> back() const
		{
			return { _time().back(), _cash().back() };
		}

		template<class U = double, class C = double>
		class iterable {
			const base<U, C>& inst;
			size_t i;
		public:
			using iterator_category = std::forward_iterator_tag;
			using value_type = std::pair<U, C>;

			iterable(const base& inst, size_t i = 0)
				: inst{ inst }, i{ i }
			{ }

			explicit operator bool() const
			{
				return i < inst.size();
			}	

			value_type operator*() const
			{
				return { inst.time()[i], inst.cash()[i] };
			}

			iterable& operator++()
			{
				if (operator bool()) {
					++i;
				}

				return *this;
			}

			iterable& operator++(int)
			{
				auto tmp = *this;
				
				operator++();

				return tmp;
			}
		};

		iterable iterate() const
		{
			return iterable(*this);
		}
	private:
		virtual const std::span<U> _time() const = 0;
		virtual const std::span<C> _cash() const = 0;
	};

	// single cash flow instrument
	template<class U = double, class C = double>
	class zero_coupon_bond : public base<U, C> {
		U u;
		C c;
	public:
		zero_coupon_bond(U u = 0, C c = 0)
			: u{ u }, c{ c }
		{ }
		const std::span<U> _time() const override
		{
			return std::span(const_cast<U*>(&u), 1);
		}
		const std::span<C> _cash() const override
		{
			return std::span(const_cast<C*>(&c), 1);
		}
	};
}
