// tmx_span.h - std::span helpers
#pragma once
#include <algorithm>
#include <initializer_list>
#include <span>

namespace tmx {

	// non-owning view of data
	template<class T>
	class view {
		const size_t n;
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
			return size() == v.size()
				&& std::equal(data(), data() + size(), v.begin(), v.end());
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
#ifdef _DEBUG

	template<class T>
	inline int view_test()
	{
		ptrdiff_t m;
		T t[] = { 1,2,4 };
		{
			m = translate<T>(0, 3, t);
			assert(view(3 - m, t + m).eq({ 1, 2, 4 }));
			
			m = translate<T>(1, 3, t);
			assert(view(3 - m, t + m).eq({ 1,  3 }));

			m = translate<T>(2, 3, t);
			assert(view(3 - m, t + m).eq({ 1 }));

			m = translate<T>(-3, 3, t);
			assert(view(3 - m, t + m).eq({ 1, 2, 4 }));

		}

		return 0;
	}
#endif // _DEBUG

	// put things back
	template<class T>
	class translate_ {
		T dt;
		size_t n;
		ptrdiff_t m; // offset
		T* t;
	public:
		translate_(T dt_, size_t n_, T* t_, ptrdiff_t m_ = 0)
			: dt{ dt_ }, n{ n_ }, m{ m_ }, t { t_ }
		{
			m = tmx::translate(dt, n, t);
		}
		~translate_()
		{
			tmx::translate(-dt, n, t);
		}

		size_t size() const
		{
			return n - m;
		}
		T* data()
		{
			return t + m;
		}
		const T* data() const
		{
			return t + m;
		}
		auto view() const
		{
			return tmx::view(size(), data());
		}

#ifdef _DEBUG
		static int test()
		{
			T t[] = { 1,2,3 };
			{
				tmx::translate_ t0(0.5, 3, t);
				assert(3 == t0.size());
				assert(1 - 0.5 == t[0]);
			}
			assert(tmx::view(3, t).eq({ 1,2,3 }));
			{
				tmx::translate_ t0(1.5, 3, t);
				assert(2 == t0.size());
				assert(1 - 1.5 == t[0]);
				assert(t0.view().eq( { 0.5, 1.5 }));
			}
			assert(tmx::view(3, t).eq({ 1,2,3 }));

			return 0;
		}
#endif // _DEBUG

	};


}
