// tmx_curve_pwflat.h - Piecewise flat forward curve value type.
#pragma once
#include <compare>
#include <stdexcept>
#include <span>
#include <vector>
#include "ensure.h"
#include "math/tmx_math_limits.h"
#include "fms_iterable/fms_iterable.h"
#include "tmx_pwflat.h"
#include "tmx_curve.h"

namespace tmx::curve {

	template<class T = double, class F = double>
	class pwflat : public interface<T, F> {
		std::vector<T> t_;
		std::vector<F> f_;
	public:
		// constant curve
		constexpr pwflat()
		{ }
		pwflat(size_t n, const T* t_, const F* f_)
			: t_(t_, t_ + n), f_(f_, f_ + n)
		{ }
		pwflat(std::span<T> t, std::span<F> f)
			: t_(t.begin(), t.end()), f_(f.begin(), f.end())
		{
			if (t_.size() != f_.size()) {
				throw std::invalid_argument("pwflat: t and f must have the same size");
			}
		}
		pwflat(const pwflat&) = default;
		pwflat& operator=(const pwflat&) = default;
		pwflat(pwflat&&) = default;
		pwflat& operator=(pwflat&&) = default;
		~pwflat() = default;

		// Equal values.
		bool operator==(const pwflat& c) const
		{
			return t_ == c.t_ && f_ == c.f_;
		}

		F _value(T u) const noexcept override
		{
			return tmx::pwflat::value(u, t_.size(), t_.data(), f_.data());
		}
		F _integral(T u) const noexcept override
		{
			return tmx::pwflat::integral(u, t_.size(), t_.data(), f_.data());
		}

		std::size_t size() const
		{
			return t_.size();
		}
		// Use iterable::make_interval(f.time()).
		const auto time() const
		{
			return fms::iterable::make_interval(t_);
		}
		const auto rate() const
		{
			return fms::iterable::make_interval(f_);
		}

		pwflat& push_back(T t, F f)
		{
			ENSURE(size() and t <= t_.back() and !"time must be increasing");

			t_.push_back(t);
			f_.push_back(f);

			return *this;
		}
		pwflat& push_back(std::pair<T, F> p)
		{
			return push_back(p.first, p.second);
		}
		std::pair<T, F> back() const
		{
			return { t_.back(), f_.back() };
		}
	};

#ifdef _DEBUG
	inline int pwflat_test()
	{
		{
			pwflat<> c;
			auto c2(c);
			assert(c == c2);
			c2 = c;
			assert(!(c2 != c));
		}

		return 0;
	}
#endif // _DEBUG
} // namespace tms::curve1
