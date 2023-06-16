// tmx_pwflat.h - piecewise flat curve
/*
		   { f[i] if t[i-1] < t <= t[i];
	f(t) = { _f   if t > t[n-1];
		   { NaN  if t < 0
	F                                   _f
	|        f[1]             f[n-1] o--------
	| f[0] o------          o--------x
	x------x      ... ------x
	|
	0-----t[0]--- ... ---t[n-2]---t[n-1]--- T
*/
#pragma once
#ifdef _DEBUG
#include <cassert>
#endif
#include <cmath>
#include <algorithm>
//#include <iterator>
//#include <limits>
//#include <numeric>
#include "tmx_view.h"

namespace tmx::pwflat {

	template<class X>
	inline static constexpr X NaN = std::numeric_limits<X>::quiet_NaN();

	// strictly increasing values
	template<class I>
	constexpr bool monotonic(const I b, const I e)
	{
		using T = typename std::iterator_traits<I>::value_type;

		return std::is_sorted(b, e, std::less<T>{});
	}
	template<class T>
	constexpr bool monotonic(size_t n, const T* t)
	{
		return monotonic(t, t + n);
	}
	template<class T>
	constexpr bool monotonic(const std::initializer_list<T>& t)
	{
		return monotonic(t.begin(), t.end());
	}

#ifdef _DEBUG

	inline int test_pwflat_monotonic()
	{
		static_assert(monotonic<int>(0, nullptr));
		static_assert(monotonic({ 1 }));
		static_assert(monotonic({ 1,2 }));
		//static_assert(!monotonic({ 1,1 })); //??? fails

		return 0;
	}

#endif // _DEBUG

	// f(u) assuming t[i] monotonically increasing
	template<class T = double, class F = double>
	constexpr F value(T u, size_t n, const T* t, const F* f, F _f = NaN<F>)
	{
		if (u < 0)
			return NaN<F>;
		if (n == 0)
			return _f;

		auto ti = std::lower_bound(t, t + n, u);

		return ti == t + n ? _f : f[ti - t];
	}

#ifdef _DEBUG

	inline int pwflat_value_test()
	{
		constexpr double t[] = { 1, 2, 3 };
		constexpr double f[] = { 2, 3, 4 };
		{
			{
				constexpr double v = value(0., 0, t, f);
				static_assert(v != v); // std::isnan not constexpr
			}
			static_assert(5 == value(0., 0, t, f, 5.));
			{
				constexpr double v = value(-0.1, 3, t, f);
				static_assert(v != v);
			}
			static_assert(2 == value(0., 3, t, f));
			static_assert(2 == value(0.1, 3, t, f));
			static_assert(2 == value(1., 3, t, f));
			static_assert(3 == value(1.1, 3, t, f));
			static_assert(4 == value(2.9, 3, t, f));
			static_assert(4 == value(3., 3, t, f));
			{
				constexpr double v = value(3.1, 3, t, f);
				static_assert(v != v);
			}
		}

		return 0;
	}

#endif // _DEBUG

	// int_0^u f(t) dt
	template<class T, class F>
	constexpr F integral(T u, size_t n, const T* t, const F* f, F _f = NaN<F>)
	{
		if (u < 0)
			return NaN<F>;
		if (u == 0)
			return 0;
		if (n == 0)
			return u * _f;

		F I = 0;
		T t_ = 0;

		size_t i;
		for (i = 0; i < n and t[i] <= u; ++i) {
			I += f[i] * (t[i] - t_);
			t_ = t[i];
		}
		if (u > t_) {
			I += (i == n ? _f : f[i]) * (u - t_);
		}

		return I;
	}

#ifdef _DEBUG

	inline int pwflat_integral_test()
	{
		constexpr double t[] = { 1, 2, 3 };
		constexpr double f[] = { 2, 3, 4 };
		{

			static_assert(0 == integral(0., 0, t, f));
			static_assert(10 == integral(2., 0, t, f, 5.));
			{
				constexpr double v = integral(-0.1, 3, t, f);
				static_assert(v != v);
			}
			static_assert(0 == integral(0., 3, t, f));
			static_assert(1 == integral(0.5, 3, t, f));
			static_assert(2 == integral(1., 3, t, f));
			static_assert(2 + 3. / 2 == integral(1.5, 3, t, f));
			static_assert(5 == integral(2., 3, t, f));
			static_assert(9 == integral(3., 3, t, f));
			//ensure(9 == integral(3., 3, t, f));
			{
				//				constexpr double v = integral(3.1, 3, t, f);
				//				static_assert(v != v);
			}
			static_assert(9 + 2.5 == integral(3.5, 3, t, f, 5.));
		}

		return 0;
	}

#endif // _DEBUG

	// discount D(u) = exp(-int_0^u f(t) dt)
	// std::exp not constexpr
	template<class T, class F>
	F discount(T u, size_t n, const T* t, const F* f, F _f = NaN<F>)
	{
		return std::exp(-integral(u, n, t, f, _f));
	}

#ifdef _DEBUG

	inline int pwflat_discount_test()
	{
		double t[] = { 1, 2, 3 };
		double f[] = { 2, 3, 4 };
		{
			assert(0 == discount(0., 0, t, f));
			assert(10 == discount(2., 0, t, f, 5.));
			{
				double v = discount(-0.1, 3, t, f);
				assert(v != v);
			}
			assert(0 == discount(0., 3, t, f));
			assert(1 == discount(0.5, 3, t, f));
			assert(2 == discount(1., 3, t, f));
			assert(2 + 3. / 2 == discount(1.5, 3, t, f));
			assert(5 == discount(2., 3, t, f));
			//!!!failing static_assert(9 == discount(3., 3, t, f));
			assert(9 == discount(3., 3, t, f));
			{
				double v = discount(3.1, 3, t, f);
				assert(v != v);
			}
		}

		return 0;
	}

#endif // _DEBUG

	// spot r(u) = (int_0^u f(t) dt)/u
	// r(u) = f(u) if u <= t[0]
	template<class T, class F>
	constexpr F spot(T u, size_t n, const T* t, const F* f, F _f = NaN<F>)
	{
		return n == 0 ? _f
			: u <= t[0] ? value(u, n, t, f, _f) : integral(u, n, t, f, _f) / u;
	}

#ifdef _DEBUG

	inline int pwflat_spot_test()
	{
		constexpr double t[] = { 1, 2, 3 };
		constexpr double f[] = { 2, 3, 4 };
		{
			/*!!!
			static_assert(0 == spot(0., 0, t, f));
			static_assert(10 == spot(2., 0, t, f, 5.));
			{
				constexpr double v = spot(-0.1, 3, t, f);
				static_assert(v != v);
			}
			static_assert(0 == spot(0., 3, t, f));
			static_assert(1 == spot(0.5, 3, t, f));
			static_assert(2 == spot(1., 3, t, f));
			static_assert(2 + 3. / 2 == spot(1.5, 3, t, f));
			static_assert(5 == spot(2., 3, t, f));
			//!!!failing static_assert(9 == spot(3., 3, t, f));
			ensure(9 == spot(3., 3, t, f));
			{
				constexpr double v = spot(3.1, 3, t, f);
				static_assert(v != v);
			}
			*/
		}

		return 0;
	}

#endif // _DEBUG
	template<class T = double, class F = double>
	class curve_base {
	public:
		virtual ~curve_base() = default;

		F value(T u) const
		{
			return _value(u);
		}
		F rate(T u) const
		{
			return value(u);
		}
		F operator()(T u) const
		{
			return value(u);
		}

		F integral(T u) const
		{
			return _integral(u);
		}
		F discount(T u) const
		{
			return _discount(u);
		}
		F spot(T u) const
		{
			return _spot(u);
		}
		/*
		size_t offset() const
		{
			return _offset();
		}
		//??? remove
		size_t size() const
		{
			return _size();
		}
		const T* time() const
		{
			return _time;
		}
		const F* rate() const
		{
			return _rate();
		}
		const F* forward() const
		{
			return rate();
		}
		*/
		std::pair<T, F> back() const
		{
			return _back();
		}

		// Get extrapolated value.
		F extrapolate() const
		{
			return _extrapolate();
		}
		// Set extrapolated value.
		curve_base& extrapolate(F f_)
		{
			return _extrapolate(f_);
		}

		// Parallel shift
		curve_base& shift(F df)
		{
			return _shift(df);
		}

		// t -> t - u > 0
		curve_base& translate(T u)
		{
			return _translate();
		}
	private:
		virtual F _value(T u) const = 0;
		virtual F _integral(T u) const = 0;
		virtual F _discount(T u) const = 0;
		virtual F _spot(T u) const = 0;
		virtual std::pair<T, F> _back() const = 0;
		virtual F _extrapolate() const = 0;
		virtual curve_base& _extrapolate(F f_) = 0;
		virtual curve_base& _shift(F df) = 0;
		virtual curve_base& _translate(T u) = 0;
	};

	// Non-owning view of pwflat curve.
	template<class T = double, class F = double>
	class curve_view : public curve_base<T,F> {
	protected:
		size_t n;
		T* t;
		F* f;
		F _f;
		ptrdiff_t off;
	public:
		// constant curve
		curve_view(F _f = NaN<F>)
			: n(0), t(nullptr), f(nullptr), _f(_f), off(0)
		{}
		curve_view(size_t n = 0, T* t = nullptr, F* f = nullptr, F _f = NaN<F>)
			: n(n), t(t), f(f), _f(_f), off(0)
		{}
		curve_view(const curve_view&) = default;
		curve_view& operator=(const curve_view&) = default;
		virtual ~curve_view() = default;

		bool ok() const
		{
			return (n == off or t[off] >= 0 and monotonic(t, t + n));
		}

		bool operator==(const curve_view& c) const
		{
			return offset() == c.offset()
				and std::equal(time(), time() + size(), c.time())
				and std::equal(rate(), rate() + size(), c.rate())
				and (_extrapolate() == c._extrapolate() or std::isnan(_extrapolate()) and std::isnan(c._extrapolate()));
		}

		size_t offset() const
		{
			return off;
		}
		size_t size() const
		{
			return n - off;
		}
		const T* time() const
		{
			return t + off;
		}
		const F* rate() const
		{
			return f + off;
		}
		const F* forward() const
		{
			return rate();
		}
		// rate element access
		F operator[](size_t i) const
		{
			return f[i];
		}
		F& operator[](size_t i)
		{
			return f[i];
		}
		std::pair<T, F> _back() const override
		{
			if (n == off) {
				return { NaN<T>, NaN<F> };
			}

			return { t[n - 1], f[n - 1] };
		}

		// Get extrapolated value.
		F _extrapolate() const override
		{
			return _f;
		}
		// Set extrapolated value.
		curve_view& _extrapolate(F f_) override
		{
			_f = f_;

			return *this;
		}

		// Parallel shift
		curve_view& _shift(F df) override
		{
			for (size_t i = 0; i < size(); ++i) {
				f[i] += df;
			}
			_f += df;

			return *this;
		}

		// t -> t - u > 0
		curve_view& _translate(T u) override
		{
			// !!! make sure curve.translate(u).translate(v) == curve.translate(u + v)
			view v(n, t);
			off += v.translate(u);

			return *this;
		}
		/*
		template<class T, class F>
		struct _translate : public curve_view<T, F> {
			T u;
			constexpr _translate(curve_view<T, F> c, T u)
				: curve_view<T, F>(c), u(0)
			{
				translate(u);
			}
			constexpr _translate(const _translate&) = default;
			constexpr _translate& operator=(const _translate&) = default;
			constexpr ~_translate()
			{
				translate(-u);
			}
		};
		constexpr _translate<T,F> translate_(T u) const
		{
			return _translate<T,F>(*this, u);
		}
		*/
		F _value(T u) const override
		{
			return pwflat::value(u, size(), time(), rate(), _f);
		}
		F _integral(T u) const override
		{
			return pwflat::integral(u, size(), time(), rate(), _f);
		}
		F _discount(T u) const override
		{
			return pwflat::discount(u, size(), time(), rate(), _f);
		}
		F _spot(T u) const override
		{
			return pwflat::spot(u, size(), time(), rate(), _f);
		}
#ifdef _DEBUG
		static int test()
		{
			T t[] = { 1, 2, 3 };
			F f[] = { .1,.2,.3 };
			
			{
				curve_view c(3, t, f);
				assert(c.ok());
				assert(0 == c.offset());
				assert(c.extrapolate() != c.extrapolate()); // NaN
				curve_view c2{ c };
				assert(c2 == c);
				c = c2;
				assert(!(c != c2));
				assert(c(0) == .1);
				assert(c(1) == .1);
				assert(c(2) == .2);
				assert(c(3) == .3);
				assert(!(c != c2));
				assert(c(0.5) == .1);
				assert(c(1.5) == .2);
				assert(c(2.5) == .3);
				assert(c(3.5) != c(3.5));
			}
			{
				T t2[] = { 1, 2, 3 };
				F f2[] = { .1,.2,.3 };
				curve_view c2(3, t, f);
				c2.shift(0.1);
				assert(c2(0) == .1 + 0.1);
				assert(c2(1.5) == .2 + 0.1);
				/*
				c2.translate(0.5);
				assert(t2[0] == 0.5);
				c2.extrapolate(-0.5);
				*/
				c2.shift(-0.1);
				assert(c2 == curve_view(3, t, f));
			}
		
			return 0;
		}

#endif // _DEBUG
	};

}

