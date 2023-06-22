// tmx_span.h - std::span helpers
#pragma once
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <initializer_list>
#include <limits>

namespace tmx {

	template<class T>
	constexpr T NaN = std::numeric_limits<T>::quiet_NaN();

	template<class X>
	constexpr bool samesign(X x, X y)
	{
		return x = std::copysign(x, y);
	}

	inline long xmod(long x, size_t y)
	{
		if (0 <= x and x < y)
			return x;

		long z = x % static_cast<long>(y);
		if (z < 0) z += static_cast<long>(y);

		return z;
	}

	// non-owning view of data
	template<class T>
	class view {
		size_t n;
		T* t;
	public:
		constexpr view()
			: n{ 0 }, t{ nullptr }
		{ }
		constexpr view(size_t n, T* t)
			: n{ n }, t{ t }
		{ }
		template<size_t N>
		constexpr view(T(&t)[N])
			: n{ N }, t{ t }
		{ }
		constexpr view(const view&) = default;
		constexpr view& operator=(const view&) = default;
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
		
		// Checked element access.
		constexpr T operator[](size_t i) const
		{
			return t[i];
		}
		constexpr T& operator[](size_t i)
		{
			return t[i];
		}

		// Cyclic element access.
		constexpr T operator()(long i) const
		{
			return t[xmod(i, n)];
		}
		constexpr T& operator()(long i)
		{
			return t[xmod(i, n)];
		}

		// t => t - u, return first index > 0
		constexpr ptrdiff_t translate(T u)
		{
			if (n == 0) {
				return 0;
			}

			for (size_t i = 0; i < n; ++i) {
				t[i] -= u;
			}

			return std::upper_bound(t, t + n, 0) - t;
		}
#ifdef _DEBUG

		static int test()
		{
			ptrdiff_t m;
			T t[] = { 1,2,4 };
			view v(3, t);
			{
				m = v.translate(0);
				assert(view<T>(3 - m, t + m).eq({ 1, 2, 4 }));

				m = v.translate(1);
				assert(view<T>(3 - m, t + m).eq({ 1,  3 }));

				m = v.translate(2);
				assert(view<T>(3 - m, t + m).eq({ 1 }));

				m = v.translate(-3);
				assert(view<T>(3 - m, t + m).eq({ 1, 2, 4 }));
			}
			{
				const view v(t);
				assert(3 == v.size());
				assert(v.eq({ 1, 2, 4 }));
				assert(v[0] == 1);
				assert(v(-1) == 4);
			}
			{
				view v(t);
				assert(3 == v.size());
				assert(v.eq({ 1, 2, 4 }));
				assert(v[0] == 1);
				v(-1) = 5;
				assert(v[2] == 5);
			}

			return 0;
		}
#endif // _DEBUG

	};
}
