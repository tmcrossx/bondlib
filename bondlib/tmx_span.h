// tmx_span.h - std::span helpers
#pragma once
#include <algorithm>
#include <initializer_list>
#include <span>

namespace tmx {
	namespace span {

#ifdef _DEBUG

		template<class T, size_t N>
		constexpr bool equal(const std::span<T, N>& s, const std::initializer_list<T>& t)
		{
			return std::equal(s.begin(), s.end(), t.begin(), t.end());
		};

#endif // _DEBUG

		// t => t - u > 0
		template<class T>
		constexpr std::span<T> translate(T u, std::span<T> t)
		{
			std::transform(t.begin(), t.end(), t.begin(), [u](T ti) { return ti - u; });
			size_t m = std::upper_bound(t.begin(), t.end(), 0) - t.begin();

			return t.last(t.size() - m);
		}
		template<class T>
		constexpr std::span<T> translate(T u, size_t n, T* t)
		{
			return translate(u, std::span(t, n));
		}

#ifdef _DEBUG

		template<class T>
		inline int pwflat_translate_test()
		{
			{
				constexpr T t[] = { 1,2,4 };
				auto t0 = translate<T>(0, 3, t);
				static_assert(equal(t0, { 1, 2, 4 }));

				auto t1 = translate<T>(1, 3, t);
				static_assert(equal(t1, { 1,  3 }));

				auto t2 = translate<T>(2, 3, t);
				static_assert(equal(t2, { 1 }));

				auto t3 = translate<T>(-3, 3, t);
				static_assert(equal(t3, { 1, 2, 4 }));
			}
			{
				T t[] = { 1,2,4 };
				auto t0 = translate<T>(0, std::span(t));
				static_assert(equal(t0, { 1, 2, 4 }));

				auto t1 = translate<T>(1, t0);
				static_assert(equal(t1, { 1,  3 }));

				auto t2 = translate<T>(2, t1);
				static_assert(equal(t2, { 1 }));

				auto t3 = translate<T>(-3, t2);
				static_assert(equal(std::span<T>(t), { 1 - 1, 2 - 1 - 2, 4 }));
			}

			return 0;
		}
#endif // _DEBUG
	} // namespace span

	template<class T>
	class translate {
		T dt;
		size_t n, m;
		T* t;
	public:
		translate(T dt, size_t n, T* t)
			: dt{ dt }, n{ n }, t{ t }
		{
			m = span::translate(dt, n, t).size();
		}
		translate(T dt, const std::span<T>& t)
			: translate(t, t.size(), t.data())
		{ }
		~translate()
		{
			span::translate(-dt, n, t);
		}

		size_t size() const
		{
			return m;
		}
		auto span() const 
		{
			return std::span(t, n - m);
		}

#ifdef _DEBUG
		static int test()
		{
			T t[] = { 1,2,3 };
			{
				tmx::translate t0(0.5, 3, t);// std::span(t));
				assert(3 == t0.size());
				assert(1 - 0.5 == t[0]);
			}
			assert(span::equal(std::span(t), { 1.,2.,3. }));
			{
				tmx::translate t0(1.5, 3, t);// std::span(t));
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
