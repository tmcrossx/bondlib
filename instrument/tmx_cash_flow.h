// tmx_cash_flow.h - Time and cash amount.
#pragma once
#include <compare>

namespace tmx {

	// Cash flow at time u of amount c.
	template<class U = double, class C = double>
		//requires std::totally_ordered<U> && std::equality_comparable<C>
	struct cash_flow {
		using time_type = U;
		using cash_type = C;

		U u; // time
		C c; // cash

		constexpr cash_flow(U u = U{}, C c = C{})
			: u(u), c(c)
		{ }

		constexpr auto operator<=>(const cash_flow& cf) const = default;
	};
#ifdef _DEBUG
	static_assert(cash_flow(1, 2) == cash_flow(1, 2));
	static_assert(!(cash_flow(1, 2) < cash_flow(1, 2)));
	static_assert(!(cash_flow(1., 2.) < cash_flow(1., 2.)));
#endif // _DEBUG
}
