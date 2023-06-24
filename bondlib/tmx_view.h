// tmx_span.h - std::span helpers
#pragma once
#include <algorithm>
#include <cmath>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <stdexcept>

namespace tmx {

	template<class T>
	constexpr T NaN = std::numeric_limits<T>::quiet_NaN();

	template<class X>
	constexpr bool samesign(X x, X y)
	{
		return x = std::copysign(x, y);
	}

	// cyclic division
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
		template<std::contiguous_iterator I>
		constexpr view(I b, I e)
			: n{ static_cast<size_t>(std::distance(b, e)) }, t{ n ? &*b : nullptr }
		{ }
		template<class C>
		// requires std::contiguous_iterator<typename C::iterator>
		constexpr view(C& t)
			: view(t.begin(), t.end())
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
		// == view({1,2,3}) ???
		constexpr bool eq(const std::initializer_list<T>& v) const
		{
			return size() == v.size()
				&& std::equal(data(), data() + size(), v.begin(), v.end());
		}

		// Least index with t[i] > 0.
		constexpr ptrdiff_t offset(T u) const
		{
			return std::upper_bound(t, t + n, u) - t;
		}

		constexpr size_t size() const
		{
			return n;
		}

		constexpr T* data()
		{
			return t;
		}
		constexpr T* begin()
		{
			return t;
		}
		constexpr T* end()
		{
			return t + n;
		}

		constexpr const T* data() const
		{
			return t;
		}
		constexpr const T* begin() const
		{
			return t;
		}
		constexpr const T* end() const
		{
			return t + n;
		}

		// Unchecked element access.
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
			return n == 0 ? NaN<T> : t[xmod(i, n)];
		}
		constexpr T& operator()(long i)
		{
			//??? static T nan = NaN<T>;
			return t[xmod(i, n)];
		}

#ifdef _DEBUG

		static int test()
		{
			double t[] = { 1,2,4 };
			view v(t);
			{
				assert(0 == v.offset(0));
				assert(0 == v.offset(0.9));
				assert(1 == v.offset(1));
				assert(1 == v.offset(1.1));
				assert(3 == v.offset(5));
			}
			{
				const view cv(t);
				assert(3 == cv.size());
				assert(cv.eq({ 1, 2, 4 }));
				assert(cv[0] == 1); // operator[] const
				assert(cv(-1) == 4);
			}
			{
				view v2(t);
				assert(3 == v2.size());
				assert(v2.eq({ 1, 2, 4 }));
				assert(v2[0] == 1); // operator[]
				v2(-1) = 5;
				assert(v2[2] == 5);
			}

			return 0;
		}
#endif // _DEBUG

	};
}
