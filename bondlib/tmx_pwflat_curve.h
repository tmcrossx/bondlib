// tmx_pwflat_curve.h - piecewise flat curve value type
#pragma once
#include "tmx_pwflat.h"

namespace tmx::pwflat {

	// Pwflat curve value type.
	template<class T = double, class F = double>
	class curve {
		std::vector<T> t;
		std::vector<F> f;
		F _f;
	public:
		// constant curve
		curve(F _f = NaN<F>)
			: _f{ _f }
		{ }
		curve(size_t n, const T* t_, const F* f_, F _f = NaN<F>)
			: t(t_, t_ + n), f(f_, f_ + n), _f(_f)
		{
			ensure(ok());
		}
		curve(const std::vector<T>& t, const std::vector<F>& f, F _f = NaN<F>)
			: t(t), f(f), _f(_f)
		{
			ensure(ok());
		}
		curve(const curve&) = default;
		curve& operator=(const curve&) = default;
		curve(curve&&) = default;
		curve& operator=(curve&&) = default;
		~curve()
		{ }

		bool ok() const
		{
			return t.size() == f.size()
				and (t.size() == 0 or t[0] > 0 and monotonic(t.begin(), t.end()));
		}

		size_t size() const
		{
			return t.size();
		}
		const T* time() const
		{
			return t.data();
		}
		const F* forward() const
		{
			return f.data();
		}
		const F* rate() const
		{
			return f.data();
		}
		std::pair<T, F> back() const
		{
			return { t.back(), f.back() };
		}

		// add point
		curve& extend(T t_, F f_)
		{
			ensure(size() == 0 || t_ > t.back());

			t.push_back(t_);
			f.push_back(f_);

			return *this;
		}
		// Get extrapolated value.
		F extrapolate() const
		{
			return _f;
		}
		// Set extrapolated value.
		curve& extrapolate(F f_)
		{
			_f = f_;
		}

		// Parallel shift
		curve& shift(F& df)
		{
			std::for_each(f.begin(), f.end(), [df](F fi) { fi += df; });

			return *this;
		}

		F value(T u) const
		{
			return pwflat::value(u, t.size(), t.data(), f.data(), _f);
		}
		F operator()(T u) const
		{
			return value(u);
		}
		F integral(T u) const
		{
			return pwflat::integral(u, t.size(), t.data(), f.data(), _f);
		}
		F discount(T u) const
		{
			return pwflat::discount(u, t.size(), t.data(), f.data(), _f);
		}
		F spot(T u) const
		{
			return pwflat::spot(u, t.size(), t.data(), f.data(), _f);
		}
	};

}