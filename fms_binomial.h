// fms_binomial.h - Binomial model
// f(W_tau)P(Omega) = sum_j f(W_j)P(tau = j)
#include "fms_iterable.h"

namespace fms::binomial {

	constexpr std::size_t choose(std::size_t n, std::size_t k)
	{
		if (k == 0 || k == n) {
			return 1;
		}
		else if (n == 0 || k > n) {
			return -1;
		}
		else {
			if (k > n / 2) {
				k = n - k;
			}
		}
			
		return choose(n - 1, k - 1) + choose(n - 1, k);
	}
#ifdef _DEBUG
	static_assert(choose(0, 0) == 1);
	static_assert(choose(1, 0) == 1);
	static_assert(choose(1, 1) == 1);
	static_assert(choose(2, 0) == 1);
	static_assert(choose(2, 1) == 2);
	static_assert(choose(2, 2) == 1);
	static_assert(choose(3, 0) == 1);
	static_assert(choose(3, 1) == 3);
	static_assert(choose(3, 2) == 3);
	static_assert(choose(3, 3) == 1);
	constexpr auto nnn = choose(3, 3);
#endif // _DEBUG

	// W_n = k
	struct atom {
		std::size_t n, k; 
	};

	inline double probability(const atom& a, double p = 0.5)
	{
		return choose(a.n, a.k) * pow(p, a.k) * pow(1 - p, a.n - a.k);
	}

	// Atoms at time N containing atom{n,k}.
	class atoms {
		atom a;
		std:: size_t N, i;
	public:
		constexpr atoms(const atom& a, std::size_t N)
			: a(a), N(N), i(a.k)
		{ }
		constexpr operator bool() const
		{
			return i <= a.k + N - a.n;
		}
		constexpr atom operator*() const
		{
			return { N, i };
		}
		constexpr atoms& operator++()
		{
			if (operator bool()) {
				++i;
			}

			return *this;
		}
	};

} // namespace fms::binomial
