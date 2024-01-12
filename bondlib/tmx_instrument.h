// tmx_instrument.h - times and cash flows
#pragma once
#include <algorithm>
#include <vector>
#include "tmx_view.h"

namespace tmx {

	// NVI base class for instruments
	template<class U = double, class C = double>
	struct instrument {
		virtual ~instrument()
		{ }

		bool operator==(const instrument& i) const
		{
			return time() == i.time() and cash() == i.cash();
		}

		size_t size() const
		{
			return _time().size();
		}
		const view<U> time() const
		{
			return _time();
		}
		const view<C> cash() const
		{
			return _cash();
		}
		// last cash flow
		std::pair<U, C> back() const
		{
			return { _time()(-1), _cash()(-1) };
		}
	private:
		virtual const view<U> _time() const = 0;
		virtual const view<C> _cash() const = 0;
	};

	// TODO: intrument_zero ???
	// single cash flow instrument
	template<class U = double, class C = double>
	class instrument_zcb : public instrument<U, C> {
		U u;
		C c;
	public:
		instrument_zcb(U u = 0, C c = 0)
			: u{ u }, c{ c }
		{ }
		const view<U> _time() const override
		{
			return view(1, const_cast<U*>(&u));
		}
		const view<C> _cash() const override
		{
			return view(1, const_cast<C*>(&c));
		}
	};
}
