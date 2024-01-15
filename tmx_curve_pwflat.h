// tmx_curve_pwflat.h - left continuous piecewise flat curve
/*
		   { f[i] if t[i-1] < t <= t[i];
	f(t) = { _f   if t > t[n-1];
		   { NaN  if t < 0
		   { f[0] if t = 0
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
#include <vector>
#include "ensure.h"
#include "tmx_span.h"
#include "tmx_curve.h"

namespace tmx::curve {

	// Non-owning view of piecewise flat curve.
	template<class T = double, class F = double>
	class view : public base<T, F> {
	protected:
		std::span<T> t;
		std::span<F> f;
		F _f; // extrapolation value
	public:
		// constant curve
		view(F _f = NaN<F>)
			: t{}, f{}, _f(_f)
		{ }
		view(view<T> t, view<F> f, F _f = NaN<F>)
			: t(t), f(f), _f(_f)
		{
			ensure(t.size() == f.size());
			ensure(t.size() == 0 or t[0] > 0);
			ensure(monotonic(t));
		}
		view(size_t n, const T* t, const F* f, F _f = NaN<F>)
			: view(view<T>(n, t), view<F>(n, f), _f)
		{ }
		view(const view&) = default;
		view& operator=(const view&) = default;
		view(view&&) = default;
		view& operator=(view&&) = default;
		virtual ~view() = default;

		view<T> time() const
		{
			return t;
		}
		view<F> rate() const
		{
			return f;
		}

		bool operator==(const view& cv) const
		{
			return size() == cv.size()
				&& t == cv.t
				&& f == cv.f
				&& ((std::isnan(extrapolate()) and std::isnan(cv.extrapolate()))
					or (extrapolate() == cv.extrapolate()));
		}

		size_t size() const
		{
			return t.size();
		}

		F extrapolate() const
		{
			return _f;
		}
		view<T, F>& _extrapolate(F f_) override
		{
			_f = f_;

			return *this;
		}

		std::pair<T, F> _back() const override
		{
			return { t.back(), f.back() };
		}

		F _forward(T u, T t0) const override
		{
			if (u < 0)
				return NaN<F>;
			if (size() == 0)
				return extrapolate();

			auto ti = std::lower_bound(t.begin(), t.end(), u + t0);

			return ti == t.end() ? _f : f[ti - t.begin()];
		}

		// int_t^u f(s) ds
		F integral(T u, T t0 = 0) const
		{
			if (u < t0)
				return NaN<F>;
			if (u == t0)
				return 0;
			if (size() == 0)
				return (u - t0) * extrapolate();

			F I = 0;
			T t_ = t0;

			size_t i = t.offset(t0);
			for (; i < size() and t[i] <= u; ++i) {
				I += f[i] * (t[i] - t_);
				t_ = t[i];
			}
			if (u > t_) {
				I += (i == size() ? extrapolate() : f[i]) * (u - t_);
			}

			return I;
		}
		// discount D_t(u) = exp(-int_t^u f(s) ds)
		// std::exp not constexpr
		F _discount(T u, T t0 = 0) const override
		{
			return std::exp(-integral(u, t0));
		}

		// D_t(u) = exp(-(u - t) r_t(u)) = exp(-int_t^u f(s) ds)
		// r_t(u) = int_t^u f(s) ds / (u - t)
		// r_t(u) = f(u) if u <= t[offset]
		constexpr F _yield(T u, T t0 = 0) const override
		{
			size_t i = t.offset(t0);

			return u < t0 ? NaN<F>
				: i == size() ? extrapolate()
				: u <= t[i] ? f[i]
				: integral(u, t0) / (u - t0);
		}

		view<T, F>& _shift(F s) override
		{
			for (size_t i = 0; i < size(); ++i) {
				f[i] += s;
			}
			_f += s;

			return *this;
		}

#ifdef _DEBUG
		static int test()
		{
			T t[] = { 1, 2, 3 };
			F f[] = { 2, 3, 4 };
			auto cv = view(view(t), view(f));
			{
				{
					auto c = view(view(t), view(f));
					auto c2{ c };
					assert(c == c2);
					c = c2;
					assert(!(c2 != c));
				}
				{
					double v = cv(-0.1);
					assert(v != v);
				}
				assert(2 == cv(0.));
				assert(2 == cv(0.1));
				assert(2 == cv(1.));
				assert(3 == cv(1.1));
				assert(4 == cv(2.9));
				assert(4 == cv(3.));
				{
					double v = cv(3.1);
					assert(v != v);
				}
			}
			{
				{
					double v = cv.integral(-0.1);
					assert(v != v);
				}
				assert(0 == cv.integral(0.));
				assert(1 == cv.integral(0.5));
				assert(2 == cv.integral(1.));
				assert(2 + 3. / 2 == cv.integral(1.5));
				assert(5 == cv.integral(2.));
				assert(9 == cv.integral(3.));
				cv._extrapolate(5.);
				assert(9 + 2.5 == cv.integral(3.5));
			}


			return 0;
		}
#endif // _DEBUG

	};

	// Pwflat curve value type.
	template<class T = double, class F = double>
	class pwflat : public view<T, F> {
		std::vector<T> t;
		std::vector<F> f;
		void update()
		{
			view<T, F>::t = view<T>(t.begin(), t.end());
			view<T, F>::f = view<F>(f.begin(), f.end());
		}
	public:
		// constant curve
		constexpr pwflat(F _f = NaN<F>)
			: view<T, F>(_f), t{}, f{}
		{
			update();
		}
		pwflat(size_t n, const T* t_, const F* f_, F _f = NaN<F>)
			: view<T, F>(_f), t(t_, t_ + n), f(f_, f_ + n)
		{
			update();
		}
		// Promote view to a value.
		pwflat(const view<T, F>& c)
			: base(c.size(), c.t.data(), c.f.data(), c._f)
		{ }
		pwflat(const pwflat& cv)
			: view<T, F>(cv), t(cv.t), f(cv.f)
		{
			update();
		}
		pwflat& operator=(const pwflat& cv)
		{
			if (this != &cv) {
				view<T, F>::operator=(cv);
				t = cv.t;
				f = cv.f;
				update();
			}

			return *this;
		}
		pwflat(pwflat&& cv)
			: view<T, F>(cv), t(std::move(cv.t)), f(std::move(cv.f))
		{
			update();
		}
		pwflat& operator=(pwflat&& cv)
		{
			if (this != &cv) {
				view<T, F>::operator=(cv);
				t = std::move(cv.t);
				f = std::move(cv.f);
				update();
			}

			return *this;
		}
		~pwflat()
		{ }

		// add point
		pwflat& extend(T t_, F f_)
		{
			ensure(t.size() == 0 || t_ > t.back());

			t.push_back(t_);
			f.push_back(f_);
			update();

			return *this;
		}
		pwflat& push_back(const std::pair<T, F>& tf)
		{
			return extend(tf.first, tf.second);
		}
#ifdef _DEBUG

		static int test()
		{
			T t[] = { 1, 2, 3 };
			F f[] = { .1, .2, .3 };
			{
				pwflat c(.1);
				ensure(0 == c.size());
				ensure(.1 == c.extrapolate());
				pwflat c2{ c };
				ensure(c2 == c);
				c = c2;
				ensure(!(c != c2));
				ensure(.1 == c(.2));
			}
			{
				pwflat c(3, t, f);

				ensure(3 == c.size());
				ensure(f[0] == c(1.));
				ensure(std::isnan(c.extrapolate()));
				pwflat c2{ c };
				ensure(c2 == c);
				c = c2;
				ensure(!(c != c2));

				ensure(std::isnan(c(-1.)));
				for (size_t i = 0; i < 3; ++i) {
					ensure(f[i] == c(t[i]));
				}
				ensure(f[0] == c(0.5));
				ensure(f[1] == c(1.5));
				ensure(f[2] == c(2.5));
				ensure(std::isnan(c(3.5)));
			}
			/*
			{
				curve c(3, t, f);
				c.translate(0.5);
				ensure(f[0] == c(0.5));
				ensure(f[1] == c(1.5));
			}
			*/
			return 0;
		}

#endif // _DEBUG

	};

}