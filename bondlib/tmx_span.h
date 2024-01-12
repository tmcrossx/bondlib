// tmx_span.h - Non-owning view of contiguous data having std::dynamic_extent.
#pragma once
#include <algorithm>
#include <span>

namespace tmx {

	template<class X>
	constexpr X NaN = std::numeric_limits<X>::quiet_NaN();

	namespace span {
		// Least index with t[i] > u.
		template<class T>
		constexpr ptrdiff_t offset(T u, std::span<T> t)
		{
			return std::upper_bound(t.data(), t.data() + t.size(), u) - t.data();
		}
#ifdef _DEBUG
		// TODO: come up with static_assert that works
		//static_assert (int i[] = { 1,2,3 }, 0 == offset(0, std::span<int, 3>(std::array<int, 3>({ 1,2,3 }))));
#endif // _DEBUG

	// t[i - 1] <= u < t[i] < ... < t[n-1]
		template<class T, size_t E = std::dynamic_extent>
		constexpr std::span<T, E> advance(T u, std::span<T> t)
		{
			return t.last(t.size() - offset(u, t));
		}
		// TODO: come up with static_assert that works

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
