// tmx_span.h - std::span helpers
#pragma once
#include <algorithm>
#include <initializer_list>
#include <span>

namespace tmx {

	// non-owning view of data
	template<class T>
	class view {
		const size_t n; // size
		T* t;
	public:
		constexpr view(size_t n, T* t)
			: n{ n }, t{ t }
		{ }
		constexpr ~view()
		{ }

		constexpr bool operator==(const view& v) const
		{
			return size() == v.size()
				&& std::equal(data(), data() + size(), v.data(), v.data() + v.size());
		}
		constexpr bool eq(const std::initializer_list<T>& v) const
		{
			return operator==(view(v.size(), v.data()));
		}

		constexpr size_t size() const
		{
			return n;
		}
		constexpr T* data()
		{
			return t;
		}
		constexpr const T* data() const
		{
			return t;
		}

	};


	// t => t - u, return first index > 0
	template<class T>
	constexpr ptrdiff_t translate(T u, size_t n, T* t)
	{
		std::transform(t, t + n, t, [u](T ti) { return ti - u; });

		return std::upper_bound(t, t + n, 0) - t;
	}
	/*
	template<class T>
	constexpr std::span<T> translate(T u, std::span<T> t)
	{
		return t.last(t.size() - translate(u, t.size(), t.data()));
	}
	*/
#ifdef _DEBUG

	template<class T>
	inline int view_test()
	{
		T t[] = { 1,2,4 };
		{
			constexpr auto m0 = translate<T>(0, 3, t);
			assert(view(3 - m0, t + m0).eq({ 1, 2, 4 }));

			auto m1 = translate<T>(1, 3, t);
			static_assert(view(t + m1, 3 - m1).eq({ 1,  3 }));

			auto m2 = translate<T>(2, 3, t);
			static_assert(view(t + m2, 3 - m2).eq({ 1 }));

			auto m3 = translate<T>(-3, 3, t);
			static_assert(view(t + m2, 3 - m2).eq({ 1, 2, 4 }));
		}

		return 0;
	}
#endif // _DEBUG

	template<class T>
	class translate_ {
		T dt;
		size_t n, m;
		T* t;
	public:
		translate_(T dt, size_t n, T* t)
			: dt{ dt }, n{ n }, t{ t }
		{
			m = n - view::translate(dt, n, t);
		}
		translate_(T dt, std::span<T> t)
			: translate(t, t.size(), t.data())
		{ }
		~translate_()
		{
			view::translate(-dt, n, t);
		}

		size_t size() const
		{
			return m;
		}
		auto span() const
		{
			return std::span(t, n).last(m);
		}

#ifdef _DEBUG
		static int test()
		{
			T t[] = { 1,2,3 };
			{
				tmx::translate_ t0(0.5, 3, t);// std::span(t));
				assert(3 == t0.size());
				assert(1 - 0.5 == t[0]);
			}
			assert(span::equal(std::span(t), { 1.,2.,3. }));
			{
				tmx::translate_ t0(1.5, 3, t);// std::span(t));
				assert(2 == t0.size());
				assert(1 - 1.5 == t[0]);
				assert(span::equal(t0.span(), { 0.5, 1.5 }));
			}
			assert(span::equal(std::span(t), { 1.,2.,3. }));

			return 0;
		}
#endif // _DEBUG

	};


}
