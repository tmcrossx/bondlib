// tmx_instrument.h - times and cash flows
#pragma once
#include <algorithm>
#include <vector>
#include "ensure.h"
#include "tmx_view.h"

namespace tmx {

	// NVI base class for instruments
	template<class U = double, class C = double>
	struct instrument {
		// instrument() = delete; ???
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
		view<U> time() const
		{
			return _time();
		}
		view<C> cash() const
		{
			return _cash();
		}
		// last cash flow
		std::pair<U, C> back() const
		{
			return { _time()(-1), _cash()(-1) };
		}
	private:
		virtual view<U> _time() const = 0;
		virtual view<C> _cash() const = 0;
	};

}
