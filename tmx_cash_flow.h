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

		constexpr cash_flow() = default;
		constexpr cash_flow(U u, C c)
			: u(u), c(c)
		{ }

		// binary operator
		cash_flow operator()(U u_, C c_) const
		{
			return cash_flow(u_, c_);
		}

		std::pair<U, C> pair() const
		{
			return { u, c };
		}

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


// Add cash flows having the same time.
template<class U, class C>
inline tmx::cash_flow<U, C> operator+(const tmx::cash_flow<U, C>& uc1, const tmx::cash_flow<U, C>& uc2)
{
	return uc1.u == uc2.u ? tmx::cash_flow(uc1.u, uc1.c + uc2.c) : tmx::cash_flow{};
}