// tmx_span.h - Non-owning view of contiguous data having std::dynamic_extent.
#pragma once
#ifdef _DEBUG
#include <cassert>
#endif // _DEBUG
#include <algorithm>
#include <span>

namespace tmx {

	namespace span {

		// Least index with t[i] > u.
		template<class T, size_t N>
		constexpr ptrdiff_t upper_index(std::span<T, N> t, T u)
		{
			auto t0 = t.data();

			return std::upper_bound(t0, t0 + t.size(), u) - t.data();
		}
#ifdef _DEBUG
		static int upper_index_test()
		{
			static double t[] = { 1,2,3 };
			constexpr std::span<double, 3> s(t);
			assert(upper_index(s, 0.) == 0);
			assert(upper_index(s, .5) == 0);
			assert(upper_index(s, 1.) == 1);
			assert(upper_index(s, 1.5) == 1);
			assert(upper_index(s, 2.) == 2);
			assert(upper_index(s, 3.) == 3);
			assert(upper_index(s, 4.) == 3);

			return 0;
		}
#endif // _DEBUG


		// t[i - 1] <= u < t[i] < ... < t[n-1]
		template<class T, size_t N = std::dynamic_extent>
		constexpr auto drop(std::span<T, N> t, T u)
		{
			return t.subspan(upper_index(t, u));
		}
		// TODO: come up with static_assert that works
#ifdef _DEBUG
		static int drop_test()
		{
			static double t[3] = { 1,2,3 };
			constexpr std::span s(t);
			/*constexpr*/ auto a = drop(s, 1.5);
			assert(a.size() == 2);
			assert(a[0] == 2);

			return 0;
		}
#endif // _DEBUG

		/*
		template<class T>
		constexpr T NaN = std::numeric_limits<T>::quiet_NaN();

		// cyclic division
		constexpr long xmod(long x, long y)
		{
			x = x % y;

			return x >= 0 ? x : x + y;
		}
		*/

		// TODO: use std::view
		// non-owning view of data
		// TODO: use std::span

		template<class T>
		class iterable {
			std::span<T> v;
			size_t i;
		public:
			using iterator_category = std::forward_iterator_tag;
			using value_type = std::remove_cv_t<T>;

			iterable()
				: v{}, i{ 0 }
			{ }
			iterable(std::span<T> v)
				: v{ v }, i{ 0 }
			{ }

			bool operator==(const iterable&) const = default;

			explicit constexpr operator bool() const
			{
				return i < v.size();
			}
			T operator*() const
			{
				return v[i];
			}
			iterable& operator++()
			{
				if (i < v.size()) {
					++i;
				}

				return *this;
			}
			iterable operator++(int)
			{
				iterable o(*this);

				operator++();

				return o;
			}

#ifdef _DEBUG
			static int test()
			{
				{
					iterable<T> vi{};
					assert(!vi);
					iterable<T> vi2{ vi };
					assert(vi == vi2);
					vi = vi2;
					assert(!(vi != vi2));
				}
				{
					T t[] = { 1,2,4 };
					std::span v(t);
					iterable vi(v);
					iterable vi2{ vi };
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
		template<class T>
		constexpr bool monotonic(const std::span<T>& t)
		{
			return t.end() == std::adjacent_find(t.begin(), t.end(), std::greater_equal<T>{});
		}

#ifdef _DEBUG

		inline int monotonic_test()
		{
			return 0;
		}

#endif // _DEBUG
	} // namspace span

} // namespace tmx
