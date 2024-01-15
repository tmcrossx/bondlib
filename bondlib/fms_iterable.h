// fms_iterable.h - iterable_iterator with operator bool()
#pragma once
#ifdef _DEBUG
#include <cassert>
#include <vector>
#endif
#include <concepts>
#include <iterator>
#include <span>

namespace fms::iterable {

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
		//{ i.operator==(I) };
		//{ i.end() } -> std::same_as<I>;
	};

	// length(i, length(j)) == length(i) + length(j)
	template<iterable I>
	constexpr size_t length(I i, size_t n = 0)
	{
		return i ? length(++i, n + 1) : n;
	}
	
	// Drop at most n elements from iterable.
	template<iterable I>
	constexpr I drop(size_t n, I i)
	{
		while (n && i) {
			--n;
			++i;
		}

		return i;
	}

	// Unsafe pointer iterable
	template<class T>
	class ptr {
		T* p;
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = T;

		constexpr ptr(T* p) noexcept
			: p{ p }
		{ }
		constexpr ptr(const ptr& ptr)
			: p{ ptr.p }
		{ }
		/*constexpr*/ ptr& operator=(const ptr& ptr)
		{
			if (this != &ptr) {
				p = ptr.p;
			}

			return *this;
		}
		constexpr ~ptr()
		{ }

		constexpr bool operator==(const ptr& i) const = default;
	
		constexpr explicit operator bool() const
		{
			return p != nullptr; // !!!unsafe
		}
		constexpr value_type operator*() const
		{
			return *p;
		}
		constexpr ptr& operator++()
		{
			++p;

			return *this;
		}
#ifdef _DEBUG
		static int test()
		{
			{
				static T t[] = { 1,2,3 };
				ptr<T> i(t);
				assert(i);

				ptr<T> i2{ i };
				assert(i2 == i);
				i = i2;
				assert(!(i != i2));

				assert(t[0] == *i);
				auto i1 = ++i;
				assert(i1);
				assert(t[1] == *i1);
			}

			return 0;
		}
#endif // _DEBUG
	};

	// Non-owning span iterable.
	template<class T>
	class span {
		std::span<T> s;
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = T;

		constexpr span(std::span<T> s)
			: s{s}
		{ }
		constexpr span(const span&) = default;
		constexpr span& operator=(const span&) = default;
		constexpr ~span() = default;

		constexpr bool operator==(const span& i) const
		{
			return s.size() == i.s.size() && s.data() == i.s.data();
		}

		constexpr explicit operator bool() const
		{
			return s.size() != 0;
		}
		constexpr value_type operator*() const
		{
			return s.front();
		}
		constexpr span& operator++()
		{
			if (operator bool()) {
				s = s.subspan(1);
			}

			return *this;
		}
#ifdef _DEBUG
		static int test()
		{
			{
				T t[] = { 1,2,3 };
				span<T> i(t); // std::span from T[n]
				assert(i);

				span<T> i2{ i };
				assert(i2 == i);
				i = i2;
				assert(!(i != i2));

				assert(t[0] == *i);
				++i;
				assert(i);
				assert(t[1] == *i);
				assert(t[2] == *++i);
				assert(i);
				++i;
				assert(!i);
			}
			{
				std::vector<T> v{ 1,2,3 };
				span<T> i(v); // std::span from std::vector
				assert(i);
				assert(3 == length(i));
			}

			return 0;
		}
#endif // _DEBUG
	};

	// First n elements of iterable
	template<class I>
	class take {
		size_t n;
		I i;
	public:
		using value_type = typename I::value_type;

		take(size_t n, I i)
			: n{ n }, i{ i }
		{ }
		take(const take&) = default;
		take& operator=(const take&) = default;
		~take() = default;

		operator bool() const
		{
			return n != 0;
		}
		value_type operator*() const
		{
			return *i;
		}
		take& operator++()
		{
			if (operator bool()) {
				--n;
				++i;
			}

			return *this;
		}
	};

	// convert iterable_iterator to iterable
	template<class I>
	class iterator {
		I i;
	public:
		using value_type = typename I::value_type;

		iterator(I i)
			: i{ i }
		{ }
		iterator(const iterator&) = default;
		iterator& operator=(const iterator&) = default;
		~iterator() = default;

		bool operator==(const iterator& j) const
		{
			return i == j;
		}

		explicit operator bool() const
		{
			return i != i.end();
		}
		value_type operator*() const
		{
			return *i;
		}
		iterator& operator++()
		{
			++i;

			return *this;
		}
#ifdef _DEBUG
		static int test()
		{
			{
				I i = {};
			}

			return 0;
		}
#endif // _DEBUG
	};

	// Advance until predicate is true.
	template<class I, class P>
	inline I until(I i, P&& p)
	{
		while (i and !p(*i)) {
			++i;
		}

		return i;
	}
#define FMS_BINOP(op) \
	template<iterable I> \
	inline I operator op(I i, typename I::value_type t) \
	{ return until(i, [t](I j) { return *j op t; }); }
	FMS_BINOP(<)
#undef FMS_BINOP

#ifdef _DEBUG

	inline int operator_op_test()
	{
		return 0;
	}

#endif // _DEBUG

} // namespace fms
