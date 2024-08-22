// tmx_binomial.h - Binomial model.
// P(U_j = 1) = p, P(U_j) = 0) = 1 - p
// V_n = U_1 + ... + U_n
// E[V_n] = n p, Var(V_n) = n p (1 - p)
// X(n, p) = (V_n - np)sqrt(n p (1 - p)) 
#pragma once

namespace tmx::binomial {

	template<class X>
	constexpr void step(size_t n, X* x, X(*f)(X), X p = 0.5)
	{
		for (size_t i = 0; i < n - 1; ++i) {
			x[i] = (1 - p) * f(x[i]) + p * f(x[i + 1]);
		}
	}

	template<class X>
	constexpr X solve_european(size_t n, X* x, X(*f)(X), X p = 0.5)
	{
		while (n) {
			step(n, x, f, p);
			--n;
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
