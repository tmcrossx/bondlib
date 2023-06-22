// tmx_pwflat_view.h - non-owning view of a pwflat curve
#pragma once
#include "tmx_pwflat.h"

namespace tmx::pwflat {

	// Non-owning view of pwflat curve.
	template<class T = double, class F = double>
	class curve_view : public curve<T, F> {
	protected:
		size_t n;
		view<T> t;
		view<F> f;
		F _f;
		ptrdiff_t off;
	public:
		// constant curve
		curve_view(F _f)
			: n{ 0 }, t{ }, f{ }, _f{ _f }, off{ 0 }
		{ }
		curve_view(size_t n = 0, T* t = nullptr, F* f = nullptr, F _f = NaN<F>)
			: n(n), t(n, t), f(n, f), _f(_f), off(0)
		{}
		curve_view(const curve_view&) = default;
		curve_view& operator=(const curve_view&) = default;
		virtual ~curve_view() = default;

		bool ok() const
		{
			return (n == off or t[off] >= 0 and monotonic(t));
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
			return t.size() - off;
		}
		const T* time() const
		{
			return t.data() + off;
		}
		const F* rate() const
		{
			return f.data() + off;
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
			if (size() == off) {
				return { NaN<T>, NaN<F> };
			}

			return { t(-1), f(-1) };
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
			if (size() > 0) {
//				view v(n, t);
				off += t.translate(u);
			}

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


} // namespace tmx::pwflat