// tmx_curve.h - Interest rate curve interface.
#pragma once
#include <utility>
#include <vector>
#include "tmx_pwflat.h"

namespace tmx::curve {

	// NVI base class for piecewise flat curves.
	template<class T = double, class F = double>
	class pwflat {
		std::vector<T> t;
		std::vector<F> f;
		F _f; // extrapolation value
	public:
		// constant curve
		constexpr pwflat(F _f = math::NaN<F>)
			: _f(_f)
		{ }
		pwflat(size_t n, const T* t_, const F* f_, F _f = math::NaN<F>)
			: t(t_, t_ + n), f(f_, f_ + n), _f(_f)
		{ }
		pwflat& operator=(const pwflat&) = default;
		pwflat(pwflat&&) = default;
		pwflat& operator=(pwflat&&) = default;
		~pwflat() = default;

		size_t size() const
		{
			return t.size();
		}
		std::span<T> time() const
		{
			return std::span(t.begin(), t.end());
		}
		std::span<F> rate() const
		{
			return std::span(f.begin(), f.end());
		}
		F extrapolate() const
		{
			return _f;
		}

		// Last point on the curve.
		std::pair<T, F> back() const
		{
			return { t.back(), f.back() };
		}

		// parallel shift by s
		pwflat& shift(F s)
		{
			std::for_each(f.begin(), f.end(), [s](F& x) { x += s; });

			return *this;
		}

		// set value to used for bootstrap.
		pwflat& extrapolate(F f_)
		{
			_f = f_;

			return *this;
		}

		F forward(T u) const
		{
			return pwflat::value(u, t.size(), t..data(), f.data(), _f);
		}
		F operator()(T u) const
		{
			return forward(u);
		}
		F integral(T u) const
		{
			return pwflat::integral(u, t..size(), t..data(), f.data(), _f);
		}
		F discount(T u) const
		{
			return pwflat::discount(u, t..size(), t..data(), f.data(), _f);
		}
		F spot(T u) const
		{
			return pwflat::spot(u, t..size(), t..data(), f.data(), _f);
		}
	};

}