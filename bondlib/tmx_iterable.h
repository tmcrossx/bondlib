// tmx_iterable.h - iterator with operator bool() const
#pragma once
#include <concepts>
#include <iterator>

namespace tmx {

	template<class I>
	concept iterable = requires (I i) {
		//std::is_base_of_v<std::forward_iterator_tag, typename I::interator_category>;
		//typename I::iterator_concept;
		//typename I::iterator_category;
		//typename I::difference_type;
		typename I::value_type;
		//typename I::reference;
		{ !!i } -> std::same_as<bool>;
		{  *i } -> std::convertible_to<typename I::value_type>;
		{ ++i } -> std::same_as<I&>;
		{ i == i } -> std::same_as<bool>;
		{ i != i } -> std::same_as<bool>;
	};

	template<iterable I, iterable J>
	constexpr bool equal(I i, J j)
	{
		return (!i && !j) || (i && j && *i == *j && equal(++i, ++j));
	}
	template<iterable I>
	constexpr size_t length(I i, size_t n = 0)
	{
		return i ? length(++i, n + 1) : n;
	}

	template<class T>
	class iota_iterable {
		T t;
	public:
		using value_type = T;
		constexpr iota_iterable(T t = 0)
			: t(t)
		{ }
		constexpr bool operator==(const iota_iterable&) const = default;
		constexpr explicit operator bool() const
		{
			return true;
		}
		constexpr auto operator*() const
		{
			return t;
		}
		constexpr iota_iterable& operator++()
		{
			++t;

			return *this;
		}
	};

	// STL range to iterable
	template<class I>
	class range_iterable {
		I b, e;
	public:
		range_iterable(I b, I e)
			: b(b), e(e)
		{ }
		bool operator==(const range_iterable&) const = default;
		constexpr explicit operator bool() const
		{
			return b != e;
		}
		constexpr auto operator*() const
		{
			return *b;
		}
		constexpr range_iterable& operator++()
		{
			if (b != e) {
				++b;
			}

			return *this;
		}
#ifdef _DEBUG
		static int test()
		{
			{
				constexpr int i[] = { 1,2,3 };
				constexpr range_iterable ci(i, i + 3);
				constexpr auto ci2 = ci;
				static_assert(ci == ci2);
				ci = ci2;
				static_assert(!(ci != ci2));

				static_assert(ci);
				static_assert(*ci == 1);

				++ci;
				static_assert(ci);
				static_assert(*ci == 2);

				static_assert(*++ci == 3);
				static_assert(!++ci);
			}

			return 0;
		}
#endif // _DEBUG
	};

	template<iterable I>
	class counted_iterable {
		I i;
		size_t n;
	public:
		constexpr counted_iterable(I i, size_t n)
			: i(i), n(n)
		{ }
		bool operator==(const counted_iterable&) const = default;
		constexpr explicit operator bool() const
		{
			return n > 0;
		}
		constexpr auto operator*() const
		{
			return *i;
		}
		constexpr counted_iterable& operator++()
		{
			if (n) {
				--n;
				++i;
			}

			return *this;
		}

#ifdef _DEBUG
		static int test()
		{
			{
				constexpr int i[] = { 1,2,3 };
				constexpr counted_iterable ci(i, 3);
				constexpr auto ci2 = ci;
				static_assert(ci == ci2);
				ci = ci2;
				static_assert(!(ci != ci2));

				static_assert(ci);
				static_assert(*ci == 1);
				
				++ci;
				static_assert(ci);
				static_assert(*ci == 2);

				static_assert(*++ci == 3);
				static_assert(!++ci);
			}

			return 0;
		}
#endif // _DEBUG

	};

	template<iterable I>
	constexpr auto take(size_t n, I i)
	{
		return counted_iterable(i, n);
	}


} // namespace tmx
