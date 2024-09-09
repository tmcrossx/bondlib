// tmx_ho_lee.h - Ho-Lee model with constant volatility.
// D_t = exp(-int_0^t φ(s) ds - int_0^t σ(t - s) dB_s)
//     ~ exp(-int_0^t φ(s) ds - B_t σ t/sqrt(3))
//     = D(t) exp(-σ^2 t^3/6 - B_t σ t/sqrt(3))
// Var(int_0^t σ(t - s) dB_s) = σ^2 int_0^t (t - s)^2 ds = σ^2 t^3/3
// D(t) = E[D_t] = exp(-int_0^t φ(s) ds + σ^2 t^3/6)
// D_t(u) = exp(-int_t^u φ(s) ds + σ^2 (u - t)^3/6 - σ (u - t) B_t)
//        = D(u)/D(t) exp(σ^2 [-u^3 + t^3 + (u - t)^3]/6 - σ (u - t) B_t)
//        = D(u)/D(t) exp(σ^2 [-3u^2 t + 3 u t^2]/6 - σ (u - t) B_t)
//        = D(u)/D(t) exp(σ^2 [-ut(u - t)]/2 - σ (u - t) B_t)
// E[log D_t(u)] = log(D(u)/D(t)) - σ^2 ut (u - t)/2
// Var(log D_t(u)) = σ^2 (u - t)^2 t
#pragma once
#include <cmath>
#include <algorithm>
#include <utility>

namespace tmx::ho_lee {

	// iterable lite
	template<class T>
	class iterable {
		const T* pt;
		size_t n;
	public:
		using value_type = T;
		using difference_type = std::ptrdiff_t;

		constexpr iterable(const T* pt, size_t n)
			: pt(pt),  n(n)
		{ }
		constexpr iterable() = default;
		constexpr iterable(const iterable&) = default;
		constexpr iterable& operator=(const iterable&) = default;
		constexpr ~iterable() = default;

		constexpr bool operator==(const iterable& i) const = default;
		constexpr auto begin() const
		{
			return *this;
		}
		constexpr auto end() const
		{
			return iterable(pt + n, 0);
		}	
		constexpr operator bool() const
		{
			return n > 0;
		}
		constexpr T operator*() const
		{
			return *pt;
		}	
		constexpr iterable& operator++()
		{
			if (n > 0) {
				++pt;
				--n;
			}

			return *this;
		}
		constexpr iterable operator++(int)
		{
			iterable i = *this;
			++*this;
			
			return i;
		}	
	};
	static_assert(std::forward_iterator<iterable<int>>);

	template<class IU, class IC>
	class cash_flows {
		IU iu;
		IC ic;
	public:
		using value_type = std::pair<typename IU::value_type, typename IC::value_type>;
		using ptrdiff_t = std::ptrdiff_t;

		constexpr cash_flows(const IU& iu, const IC& ic)
			: iu(iu), ic(ic)
		{ }
		constexpr cash_flows() = default;
		constexpr cash_flows& operator=(const cash_flows&) = default;
		constexpr ~cash_flows() = default;

		constexpr bool operator==(const cash_flows& i) const = default;
		constexpr auto begin() const
		{
			return *this;
		}
		constexpr auto end() const
		{
			return cash_flows(iu.end(), ic.end());
		}

		constexpr operator bool() const
		{
			return iu && ic;
		}
		value_type operator*() const
		{
			return { *iu, *ic };
		}
		cash_flows& operator++()
		{
			++iu;
			++ic;

			return *this;
		}
		cash_flows operator++(int)
		{
			cash_flows i = *this;
			++*this;
			
			return i;
		}	
	};
	static_assert(std::forward_iterator<cash_flows<iterable<int>, iterable<double>>>);

	// D_t(u) = D(u) / D(t) exp(σ^2 [-ut(u - t)] / 2 - σ(u - t) B_t)
	template<class X = double>
	inline X discount(X Dt, X Du, X t, X u, X σ, X B_t)
	{
		return (Du / Dt) * std::exp(σ * σ * (-u * t * (u - t) / 2 - σ * (u - t) * B_t));
	}
	/*
	struct translate : private instrument<U, C> {
		size_t i0;
		U u0;
		using u = instrument<U, C>::u;

		translate(const instrument<U, C>& i, U u0) 
			: instrument<U, C>(i), u(u)
		{
			i0 = std::lower_bound(u, u + size, u0) - u;
		}
	};

	template<class U = double, class C = double>
	inline auto present(const instrument<U, C>& i, const C* D, C σ, C B_t)
	{
		C pv = 0;

		for (size_t j = 0; j < i.size; ++j) {
			pv += i.c[j] * discount(1., D[j], 0., i.u[j], σ, B_t);
		}

		return pv;
	}
	*/
	// E[log D_t]
	template<class X = double>
	inline auto ELogD(X t, X φ)
	{
		return -φ * t;
	}
	// Var(log D_t)
	template<class X = double>
	inline auto VarLogD(X t, X σ)
	{
		return σ * σ * t * t * t / 3;
	}
	// D(t) = E[D_t]
	template<class X = double>
	inline auto ED(X t, X φ, X σ)
	{
		return std::exp(ELogD(t, φ) + VarLogD(t, σ) / 2);
	}

	// E[log D_t(u)]
	template<class X = double>
	inline auto ELogD(X Dt, X Du, X t, X u, X σ)
	{
		return std::log(Du / Dt) - σ * σ * u * t * (u - t) / 2;
	}
	// Var[log D_t(u)]
	template<class X = double>
	inline auto VarLogD(X t, X u, X σ)
	{
		return σ * σ * (u - t) * (u - t) * t;
	}
	// E[D_t(u)]
	template<class X = double>
	inline auto ED(X Dt, X Du, X t, X u, X σ)
	{
		return std::exp(ELogD(Dt, Du, t, u, σ) + VarLogD(t, u, σ) / 2);
	}

	// Cov(log D_t(u), log D_t(v))
	//   = Cov(-σ(u - t)B_t, -σ(v - t)B_t)
	//   = σ^2 (u - t)(v - t)t
	template<class X = double>
	inline X CovLogD(X t, X u, X v, X σ)
	{
		return σ * σ * (u - t) * (v - t) * t;
	}
	// E[e^N e^M] = E[e^N] E[e^M] exp(Cov(N,M))
	// Cov(D_t(u), D_t(v))
	template<class X = double>
	inline auto CovD(X Dt, X Du, X Dv, X t, X u, X v, X σ)
	{
		return ED(Dt, Du, t, u, σ) * ED(Dt, Dv, t, v, σ) * std::exp(CovLogD(t, u, v, σ));
	}

	// Cov(log D_t(u), log D_t) 
	//   = Cov(-σ(u - t)B_t, -int_0^t σB_s ds)
	//   = σ^2 (u - t) int_0^t s ds
	//   = σ^2 (u - t) t^2/2
	template<class X = double>
	inline auto CovLogD_(X t, X u, X σ)
	{
		return σ * σ * (u - t) * t * t / 2;
	}

	// Bond option valuation.
	// E[f(P_t) D_t] = E[f(sum_{u_j > t} c_j D_t(u_j)) D_t]
	//   = E[f(sum_{u_j > t} c_j D_t(u_j) e^C_j)] E[D_t],
	// where C_j = Cov(log D_t(u_j), log D_t)

	/*
	// mean and variance of P_t = sum_{u_j > t} c_j D_t(u_j) e^C_j
	template<class U, class C, class T, class F>
	inline std::pair<F, F> moments(size_t m, const U* u, const C* c, const curve::interface<T, F>& f, T t, F σ)
	{
		F mean = 0, var = 0;

		auto j0 = view(c, c + m).upper_index(t);

		auto Dt = f.discount(t);
		for (auto j = j0; j < m; ++j) {
			auto Duj = f.discount(u[j]);
			auto Cj = std::exp(CovLogD_(t, u[j], σ));
			mean += c[j] * ED(Dt, Duj, t, u[j], σ) * Cj;
			for (auto k = j0; k < m; ++k) {
				auto Duk = f.discount(u[k]);
				auto Ck = std::exp(CovLogD_(t, u[k], σ));
				var += c[j] * c[k] * CovD(Dt, Duj, Duk, t, u[j], u[k], σ) * Cj * Ck;
			}
		}
		// Var(e^N) = E[e^N]^2 (e^Var(N) - 1)
		var = std::log(var / (mean * mean) + 1);

		return { mean, var };
	}
	// Cov(sum_{u_j > t} c_j D_t(u_j), D_t)
	template<class U, class C, class T, class F>
	inline F Cov(const instrument<U, C>& i, const curve<T, F>& f, T t, F σ)
	{
	}

	// E[f(B_t) e^N] = E[f(B_t + Cov(B_t, N))] E[e^N]
	// E[(P_t - p)^+ D_t] 
	// = E[(sum_{u_j > t} c_j D_t(u_j) - p)^+ D_t]
	// = E[(sum_{u_j > t} c_j D_t(u_j) exp(σ^2(u_j - t) - p)^+] D(t)
	template<class U, class C, class T, class F>
	inline F option(const instrument::interface<U, C>& i, const curve::interface<T, F>& f, T t, F σ, F p)
	{
		auto [m, m_, v] = moments(i, f, t, σ);
		// σ += cov!!!
		return black::call::value(m_, std::sqrt(v), p) * f.discount(t);
	}
	*/

}
