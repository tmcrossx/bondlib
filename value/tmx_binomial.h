// tmx_binomial.h - Binomial model.
// P(U_j = 1) = 1/2 = P(U_j = -1)
// W_n = U_1 + ... + U_n
// E[W_n] = 0, Var(W_n) = n
#pragma once
#include <cmath>
#include <algorithm>

namespace tmx::binomial {

	template<class X>
	struct array {
		size_t n;
		X* x;

		template<size_t N>
		array(X(&a)[N])
			: n(N), x(a)
		{ }

		constexpr operator bool() const
		{
			return n != 0;
		}
		constexpr X operator*() const
		{
			return *x;
		}
		constexpr X& operator*()
		{
			return *x;
		}
		constexpr array& operator++()
		{
			if (n) {
				--n;
				++x;
			}

			return *this;
		}
		constexpr array operator++(int)
		{
			array a = *this;
			++*this;

			return a;
		}
	};
#ifdef _DEBUG
	inline int array_test()
	{
		{
			int i[] = { 1, 2, 3 };
			array a(i);
			//array a{ 3, i };
			assert(a);
		}
		return 0;
	}
#endif // _DEBUG

	template<class X>
	constexpr size_t step(size_t n, X* x, X(*f)(X), X p = 0.5)
	{
		for (size_t i = 0; i < n - 1; ++i) {
			x[i] = (1 - p) * f(x[i]) + p * f(x[i + 1]);
		}

		return n - 1;
	}

	template<class X>
	constexpr X solve_european(size_t n, X* x, X(*f)(X), X p = 0.5)
	{
		while (n) {
			n = step(n, x, f, p);
		}

		return x[0];
	}

	template<class X>
	constexpr void step_american(size_t n, X* x, X(*f)(X), X p = 0.5)
	{
		for (size_t i = 0; i < n - 1; ++i) {
			x[i] = std::max(f(x[i]), (1 - p) * f(x[i]) + p * f(x[i + 1]));
		}
	}
	template<class X>
	constexpr X solve_american(size_t n, X* x, X(*f)(X), X p = 0.5)
	{
		while (n) {
			step_american(n, x, f, p);
			--n;
		}

		return x[0];
	}

} // namespace tmx::binomial
