// tmx_cash_flow.h - Time and cash amount.
#pragma once
#include <utility>

namespace tmx {

	// Cash flow at time u of amount c.
	template<class U = double, class C = double>
		//requires std::totally_ordered<U> && std::equality_comparable<C>
	struct cash_flow {
		using time_type = U;
		using cash_type = C;

		U u; // time
		C c; // cash

		constexpr bool operator==(const cash_flow& cf) const = default;

		// ordered by time
		constexpr bool operator<(const cash_flow& cf) const
		{
			return u < cf.u;
		}
	};
#ifdef _DEBUG
	static_assert(cash_flow(1, 2) == cash_flow(1, 2));
	static_assert(!(cash_flow(1, 2) < cash_flow(1, 2)));
	static_assert(!(cash_flow(1., 2.) < cash_flow(1., 2.)));
#endif // _DEBUG
}
