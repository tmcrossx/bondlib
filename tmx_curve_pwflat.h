// tmx_curve_pwflat.h - Piecewise flat forward curve
#pragma once
#include <compare>
#include <span>
#include <vector>
#include "tmx_math_limits.h"
#include "tmx_pwflat.h"
#include "tmx_curve.h"

namespace tmx::curve {

	template<class T = double, class F = double>
	class pwflat : public base<T, F> {
		std::vector<T> t_;
		std::vector<F> f_;
		F _f; // extrapolation value
	public:
		// constant curve
		constexpr pwflat(F _f = math::NaN<F>)
			: _f(_f)
		{ }
		pwflat(size_t n, const T* t_, const F* f_, F _f = math::NaN<F>)
			: t_(t_, t_ + n), f_(f_, f_ + n), _f(_f)
		{ }
		/*
		pwflat(const pwflat&) = default;
		pwflat& operator=(const pwflat&) = default;
		pwflat(pwflat&&) = default;
		pwflat& operator=(pwflat&&) = default;
		~pwflat() = default;
		*/

		auto constexpr operator<=>(const pwflat&) const = default;

		F _forward(T u, T t = 0) const noexcept override
		{
			return tmx::pwflat::forward(u - t, t_.size(), t_.data(), f_.data(), _f);
		}
		F _integral(T u, T t = 0) const noexcept override
		{
			return tmx::pwflat::integral(u, t_.size(), t_.data(), f_.data(), _f)
				 - tmx::pwflat::integral(t, t_.size(), t_.data(), f_.data(), _f);
		}

		std::size_t size() const
		{
			return t_.size();
		}
		auto time() const
		{
			return std::span(t_.begin(), t_.end());
		}
		auto rate() const
		{
			return std::span(f_.begin(), f_.end());
		}

		// Last point on curve.
		std::pair<T, F> back() const noexcept
		{
			return t_.size() ? std::make_pair(t_.back(), f_.back()) : std::make_pair(math::infinity<T>, _f);
		}

		// Get extrapolation level.
		F extrapolate() const noexcept
		{
			return _f;
		}
		// Set extrapolation level.
		F extrapolate(F f = math::NaN<F>) noexcept
		{
			return _f = f;
		}
	};

#ifdef _DEBUG
	inline int pwflat_test()
	{
		{
			pwflat<> c; // default constructor
			auto c2(c); // copy constructor
			c2 = c;     // copy assignment
		}
		{
			// test one case for _value, _integral, _extrapolate, and _back.
			pwflat<> p1(3);
			assert(3.0 == p1.forward(0));
			assert(15 == p1.integral(5, 0));
			//p1.extrapolate(7);
			//assert(7.0 == p1.value(0));
			//assert(21 == p1.integral(3, 0));

			//auto back = p1.back();
			//assert(back.first);
			//assert(3.0 == back.second);
			//std::cout << back.second << std::endl;
			//std::cout << p.integral(5, 0) << std::endl;
		}

		return 0;
	}
#endif // _DEBUG
} // namespace tms::curve1