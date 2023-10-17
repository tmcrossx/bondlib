// fms_iterable.h - iterable_iterator with operator bool()
#pragma once
#include <concepts>
#include <iterator>

namespace fms {

	template<class I>
	concept forward_iterable = requires (I i) {
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

	// Unsafe pointer iterable
	template<class T>
	class iterable_ptr {
		const T* pt;
	public:
		using value_type = T;

		iterable_ptr(const T* pt)
			: pt{ pt }
		{ }
		iterable_ptr(const iterable_ptr&) = default;
		iterable_ptr& operator=(const iterable_ptr&) = default;
		~iterable_ptr() = default;

		bool operator==(const iterable_ptr& i) const
		{
			return pt == i.pt;
		}

		explicit operator bool() const
		{
			return true; // !!!unsafe
		}
		value_type operator*() const
		{
			return *pt;
		}
		iterable_ptr& operator++()
		{
			++pt;

			return *this;
		}
#ifdef _DEBUG
		static int test()
		{
			{
				T t[] = { 1,2,3 };
				iterable_ptr<T> i(t);
				assert(i);

				iterable_ptr<T> i2{ i };
				assert(i2 == i);
				i = i2;
				assert(!(i != i2));

				assert(t[0] == *i);
				++i;
				assert(i);
				assert(t[1] == *i);
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
			if (n != 0) {
				--n;
				++i;
			}

			return *this;
		}
	};

	// convert iterable_iterator to iterable
	template<class I>
	class iterable_iterator {
		I i;
	public:
		using value_type = typename I::value_type;

		iterable_iterator(I i)
			: i{ i }
		{ }
		iterable_iterator(const iterable_iterator&) = default;
		iterable_iterator& operator=(const iterable_iterator&) = default;
		~iterable_iterator() = default;

		bool operator==(const iterable_iterator& j) const
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
		iterable_iterator& operator++()
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
	template<forward_iterable I> \
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
