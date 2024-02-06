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
		{ *i } -> std::convertible_to<typename I::value_type>;
		{ ++i } -> std::same_as<I&>;
		//{ i++ } -> std::same_as<I>;
		//{ i.operator==(I) };
		//{ i.end() } -> std::same_as<I>;
	};

	// size(i, size(j)) == size(i) + size(j)
	template<iterable I>
	constexpr size_t size(I i, size_t n = 0)
	{
		return i ? size(++i, n + 1) : n;
	}

	template<iterable I, iterable J>
	constexpr bool equal(I i, J j)
	{
		while (i && j) {
			if (*i++ != *j++) {
				return false;
			}
		}

		return !i && !j;
	}

	// Drop at most n elements from iterable.
	template<iterable I>
	constexpr I drop(size_t n, I i)
	{
		while (n-- && i++) {
			;
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

		ptr(T* p) noexcept
			: p{ p }
		{ }
		ptr(const ptr& ptr)
			: p{ ptr.p }
		{ }
		ptr& operator=(const ptr& ptr)
		{
			if (this != &ptr) {
				p = ptr.p;
			}

			return *this;
		}
		~ptr()
		{ }

		bool operator==(const ptr& i) const = default;

		auto begin()
		{
			return ptr{ p };
		}
		auto begin() const
		{
			return ptr{ p };
		}
		auto end()
		{
			return ptr{ nullptr };
		}
		auto end() const
		{
			return ptr{ nullptr };
		}

		explicit operator bool() const
		{
			return p != nullptr; // !!!unsafe
		}
		value_type operator*() const
		{
			return *p;
		}
		ptr& operator++()
		{
			++p;

			return *this;
		}
		ptr operator++(int)
		{
			auto p_{ *this };

			operator++();

			return p_;
		}
#ifdef _DEBUG
		static int test()
		{
			{
				static T t[] = { 1,2,3 };
				ptr<T> p(t);
				assert(p);

				ptr<T> p2{ p };
				assert(p2 == p);
				p = p2;
				assert(!(p != p2));

				assert(t[0] == *p);
				auto p1 = ++p;
				assert(p1);
				assert(t[1] == *p1);
				assert(t[1] == *p1++);
				assert(t[2] == *p1);
			}
			{
				static T t[] = { 1,2,3 };
				ptr<T> p(t);
				size_t i = 0;
				for (const auto& t_ : p) {
					assert(t_ == t[i++]);
					if (i == 3) {
						break;
					}
				}
			}

			return 0;
		}
#endif // _DEBUG
	};

	// First n elements of iterable
	template<iterable I>
	class take {
		size_t n;
		I i;
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = typename I::value_type;

		take(size_t n, I i)
			: n{ n }, i{ i }
		{ }
		template<size_t N>
		take(value_type(&a)[N])
			: take(N, ptr(a))
		{ }
		take(const take&) = default;
		take& operator=(const take&) = default;
		~take() = default;

		bool operator==(const take& i_) const
		{
			return n == i_.n && (n == 0 || i == i_.i);
		}

		auto begin()
		{
			return take{ n, i.begin() };
		}
		auto begin() const
		{
			return take{ n, i.begin() };
		}
		auto end()
		{
			return take{ 0, i.end() };
		}
		auto end() const
		{
			return take{ 0, i.end() };
		}

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
		take operator++(int)
		{
			take i_{ *this };

			operator++();

			return i_;
		}
#ifdef _DEBUG
		static int test()
		{
			{
				int i[] = { 1,2,3 };
				take t(i);
				assert(t);
				assert(3 == size(t));
				auto t2{ t };
				assert(t2);
				assert(t2 == t);
				t = t2;
				assert(!(t != t2));

				assert(1 == *t);
				assert(1 == *t++);
				assert(2 == *t);
				assert(3 == *++t);
				assert(!++t);
			}
			{
				int i[] = { 1,2,3 };
				take t(i);
				int i_ = 1;
				for (const auto& t_ : t) {
					assert(t_ == i_++);
				}
			}

			return 0;
		}
#endif // _DEBUG
	};

	// Non-owning span iterable.
	template<class T, size_t N = std::dynamic_extent>
	class span {
		std::span<T> s;
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = T;

		span(std::span<T, N> s)
			: s{ s }
		{ }
		span(const span&) = default;
		span& operator=(const span&) = default;
		~span() = default;

		bool operator==(const span& i) const
		{
			return s.size() == i.s.size() && (s.size() == 0 || s.data() == i.s.data());
		}

		auto begin()
		{
			return span{ s };
		}
		auto end()
		{
			return span{ std::span<T>{} };
		}

		explicit operator bool() const
		{
			return s.size() != 0;
		}
		value_type operator*() const
		{
			return s.front();
		}
		span& operator++()
		{
			if (operator bool()) {
				s = s.subspan(1);
			}

			return *this;
		}
		span operator++(int)
		{
			auto s_{ *this };

			operator++();

			return s_;
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
				assert(3 == size(i));
				int j = 1;
				for (const auto& i_ : i) {
					assert(i_ == j++);
				}
			}

			return 0;
		}
#endif // _DEBUG
	};

	// t, t, ...
	template<class T>
	class constant {
		T t;
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = T;

		constant(T t)
			: t{ t }
		{ }
		constant(const constant&) = default;
		constant& operator=(const constant&) = default;
		~constant() = default;

		bool operator==(const constant& i) const = default;

		auto begin()
		{
			return constant{ t };
		}
		auto end()
		{
			return constant{ std::numeric_limits<T>::quiet_NaN() };
		}

		explicit operator bool() const
		{
			return true;
		}
		value_type operator*() const
		{
			return t;
		}
		constant& operator++()
		{
			return *this;
		}
		constant operator++(int)
		{
			return *this;
		}
	};

	// t
	template<class T>
	inline auto once(T t)
	{
		return take(1, constant<T>(t));
	}

	template<iterable I, iterable J>
	class concatenate {
		I i;
		J j;
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = std::common_type_t<typename I::value_type, typename J::value_type>;

		concatenate(I i, J j)
			: i{ i }, j{ j }
		{ }
		concatenate(const concatenate&) = default;
		concatenate& operator=(const concatenate&) = default;
		~concatenate() = default;

		bool operator==(const concatenate& i_) const = default;

		explicit operator bool() const
		{
			return i || j;
		}
		value_type operator*() const
		{
			return i ? *i : *j;
		}
		concatenate& operator++()
		{
			if (i) {
				++i;
			}
			else if (j) {
				++j;
			}

			return *this;
		}
		concatenate operator++(int)
		{
			auto c_{ *this };

			operator++();

			return c_;
		}
	};

#if 0
	// convert container to iterable
	template<class C>
	class iterator {
		C::iterator b, e;
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = typename C::value_type;

		iterator(C::iterator b, C::iterator e)
			: b{ b }, e{ e }
		{ }
		iterator(C& c)
			: iterator(c.begin(), c.end())
		{ }
		iterator(const iterator&) = default;
		iterator& operator=(const iterator&) = default;
		~iterator() = default;

		bool operator==(const iterator& j) const = default;

		auto begin()
		{
			return iterator(b, e);
		}
		auto end()
		{
			return iterator(e, e);
		}

		explicit operator bool() const
		{
			return b != e;
		}
		value_type operator*() const
		{
			return *b;
		}
		iterator& operator++()
		{
			if (operator bool()) {
				++b;
			}

			return *this;
		}
		iterator operator++(int)
		{
			iterator i_{ *this };

			operator++();

			return i_;
		}
#ifdef _DEBUG
		static int test()
		{
			{
				/*
				std::vector<int> v{ 1,2,3 };
				iterator i(v);
				assert(i);
				assert(3 == size(i));
				int j = 1;
				for (const auto& i_ : i) {
					assert(i_ == j++);
				}
				*/
			}

			return 0;
		}
#endif // _DEBUG
	};
#endif // 0

	template<iterable I, iterable J, typename TI = I::value_type, typename TJ = J::value_type>
	class pair
	{
		I i;
		J j;
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = std::pair<TI, TJ>;

		pair(I i, J j)
			: i{ i }, j{ j }
		{ }
		pair(const pair&) = default;
		pair& operator=(const pair&) = default;
		~pair() = default;

		operator bool() const
		{
			return i && j;
		}
		value_type operator*() const
		{
			return { *i, *j };
		}
		pair& operator++()
		{
			++i;
			++j;

			return *this;
		}
		pair operator++(int)
		{
			auto p{ *this };
			operator++();

			return p;
		}
#ifdef _DEBUG
		static int test()
		{
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
	FMS_BINOP(< )
#undef FMS_BINOP

#ifdef _DEBUG

		inline int operator_op_test()
	{
		return 0;
	}

#endif // _DEBUG

} // namespace fms
