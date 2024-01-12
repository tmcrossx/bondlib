// tmx_view.h - Non-owning view of contiguous data.
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

	// cyclic division
	constexpr long xmod(long x, long y)
	{
		x = x % y;

		return x >= 0 ? x : x + y;
	}

	// TODO: use std::view
	// non-owning view of data
	template<class T>
	class view {
		size_t n;
		T* t;
	public:
		constexpr view(size_t n = 0, T* t = nullptr)
			: n{ n }, t{ t }
		{ }
		template<size_t N>
		constexpr view(T(&t)[N])
			: view(N, t)
		{ }
		template<std::contiguous_iterator I>
		constexpr view(I b, I e)
			: n{ static_cast<size_t>(std::distance(b, e)) }, t{ n ? &*b : nullptr }
		{ }
		constexpr view(const view&) = default;
		constexpr view& operator=(const view&) = default;
		constexpr ~view()
		{ }

		constexpr bool operator==(const view& v) const
		{
			return n == v.n
				&& std::equal(t, t + n, v.t, v.t + v.n);
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
			return n == 0 ? NaN<T> : t[xmod(i, (long)n)];
		}
		constexpr T& operator()(long i)
		{
			return t[xmod(i, (long)n)];
		}

		constexpr T back() const
		{
			return n == 0 ? NaN<T> : t[n - 1];
		}

#ifdef _DEBUG

		static int test()
		{
			{
				static_assert(-1 == -1 % 3, "xmod broken");
				static_assert(1 == 1 % -3, "xmod broken");
				static_assert(-1 == -1 % -3, "xmod broken");
			}
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

	template<class T>
	class view_iterable {
		view<T> v;
		size_t i;
	public:
		using value_type = std::remove_cv_t<T>;

		view_iterable()
			: v{}, i{ 0 }
		{ }
		view_iterable(view<T> v)
			: v{ v }, i{ 0 }
		{ }

		bool operator==(const view_iterable& o) const = default;

		explicit constexpr operator bool() const
		{
			return i < v.size();
		}
		T operator*() const
		{
			return v[i];
		}
		view_iterable& operator++()
		{
			if (i < v.size()) {
				++i;
			}

			return *this;
		}

#ifdef _DEBUG
		static int test()
		{
			{
				view_iterable<T> vi{};
				assert(!vi);
				view_iterable<T> vi2{ vi };
				assert(vi == vi2);
				vi = vi2;
				assert(!(vi != vi2));
			}
			{
				T t[] = { 1,2,4 };
				view v(t);
				view_iterable vi(v);
				view_iterable vi2{ vi };
				assert(vi2 == vi);
				vi = vi2;
				assert(!(vi != vi2));

				assert(vi);
				assert(*vi == 1);
				++vi;
				assert(vi);
				assert(*vi == 2);
				++vi;
				assert(vi);
				assert(*vi == 4);
				++vi;
				assert(!vi);
			}

			return 0;
		}
#endif // _DEBUG
	};

	// strictly increasing values
	template<class I>
	constexpr bool monotonic(const I b, const I e)
	{
		using T = typename std::iterator_traits<I>::value_type;

		// std::is_sorted(b, e, less) is not correct 
		return e == std::adjacent_find(b, e, std::greater_equal<T>{});
	}
	template<class T>
	constexpr bool monotonic(size_t n, const T* t)
	{
		return monotonic(t, t + n);
	}
	template<class T>
	constexpr bool monotonic(const view<T>& t)
	{
		return monotonic(t.begin(), t.end());
	}
	template<class T>
	constexpr bool monotonic(const std::initializer_list<T>& t)
	{
		return monotonic(t.begin(), t.end());
	}

#ifdef _DEBUG

	inline int monotonic_test()
	{
		static_assert(monotonic<int>(0, nullptr));
		static_assert(monotonic({ 1 }));
		static_assert(monotonic({ 1,2 }));
		static_assert(!monotonic({ 1.,1. }));

		return 0;
	}

#endif // _DEBUG

}
