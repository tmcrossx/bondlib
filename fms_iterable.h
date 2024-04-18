// fms_iterable.h - iterable_iterator with operator bool()
#pragma once
#ifdef _DEBUG
#include <cassert>
#endif
#include <concepts>
#include <iterator>

namespace fms::iterable {

	template<class T>
	struct base {
		base() = default;
		base(const base&) = default;
		base& operator=(const base&) = default;
		virtual ~base() {};
		
		explicit operator bool() const
		{
			return op_bool();
		}
		T operator*() const
		{
			return op_star();
		}
		base& operator++()
		{
			return op_incr();
		}
	private:
		virtual bool op_bool() const = 0;
		virtual T op_star() const = 0;
		virtual base& op_incr() = 0;
	};

	template<class T>
	inline bool equal(base<T>& i, base<T>& j) noexcept
	{
		while (i && j) {
			if (*i != *j) {
				return false;
			}
			++i;
			++j;
		}

		return !i && !j;	
	}

	template<class T>
	inline size_t length(base<T>& i, size_t n = 0) noexcept
	{
		while (i) {
			++i;
			++n;
		}

		return n;
	}

	template<class T>
	inline base<T>& skip(base<T>& i, size_t n) noexcept
	{
		while (i && n > 0) {
			++i;
			--n;
		}

		return i;
	}

	template<class T>
	class constant : public base<T> {
		T c;
	public:
		constant(T c) noexcept
			: c(c)
		{ }
		constant(const constant&) noexcept = default;
		constant& operator=(const constant&) noexcept = default;
		~constant() noexcept = default;

		bool op_bool() const noexcept override
		{
			return true;
		}
		T op_star() const noexcept override
		{
			return c;
		}
		constant& op_incr() noexcept override
		{
			return *this;
		}
	};

	// Unsafe pointer interface.
	template<class T>
	class pointer : public base<T> {
		T* p;
	public:
		pointer(T* p) noexcept
			: p(p)
		{ }
		pointer(const pointer&) noexcept = default;
		pointer& operator=(const pointer&) noexcept = default;
		~pointer() noexcept = default;

		bool op_bool() const noexcept override
		{
			return p != nullptr; // unsafe
		}
		T op_star() const noexcept override
		{
			return *p;
		}
		pointer& op_incr() noexcept override
		{
			++p;

			return *this;
		}
	};

	// Terminate on 0
	template<class T>
	class zero_pointer : public base<T> {
		T* p;
	public:
		zero_pointer(T* p) noexcept
			: p(p)
		{ }
		zero_pointer(const zero_pointer&) noexcept = default;
		zero_pointer& operator=(const zero_pointer&) noexcept = default;
		~zero_pointer() noexcept = default;

		bool op_bool() const noexcept override
		{
			return *p != 0;
		}
		T op_star() const noexcept override
		{
			return *p;
		}
		zero_pointer& op_incr() noexcept override
		{
			++p;

			return *this;
		}
	};

	// Take at most n elements.
	template<class T>
	class take : public base<T> {
		base<T>& i;
		size_t n;
	public:
		take(base<T>& i, size_t n)
			: i(i), n(n)
		{ }
		take(const take&) noexcept = default;
		take& operator=(const take&) noexcept = default;
		~take() noexcept = default;

		bool op_bool() const noexcept override
		{
			return i && n > 0;
		}
		T op_star() const noexcept override
		{
			return *i;
		}
		take& op_incr() noexcept override
		{
			if (n) {
				++i;
				--n;
			}

			return *this;
		}
	};

	template<class T, size_t N>
	inline auto array(T(&a)[N])
	{
		//pointer<T> p(&a[0]);

		return take<T>(pointer<T>(&a[0]), N);
	}
#if 0

	// Drop at most n elements.
	template<class T>
	base<T>& drop(base<T>& i, size_t n)
	{
		while (i && n > 0) {
			++i;
			--n;
		}

		return i;
	}

	// i0 then i1
	template<class T>
	class concatenate : public base<T> {
		base<T>& i0;
		base<T>& i1;
	public:
		concatenate(base<T> i0, base<T> i1)
			: i0(i0), i1(i1)
		{ }
		bool op_bool() const override
		{
			return i0 || i1;
		}
		T op_star() const override
		{
			return i0 ? *i0 : *i1;
		}
		concatenate& op_incr() override
		{
			if (i0) {
				++i0;
			}
			else {
				++i1;
			}

			return *this;
		}
	};

	// i0 and i1 in order
	template<class T>
	class merge : public base<T> {
		base<T>& i0;
		base<T>& i1;
	public:
		merge(base<T> i0, base<T> i1)
			: i0(i0), i1(i1)
		{ }
		bool op_bool() const override
		{
			return i0 || i1;
		}
		T op_star() const override
		{
			return i0 && i1
				? (*i0 < *i1 ? *i0 : *i1)
				: i0 ? *i0 : *i1;
		}
		merge& op_incr() override
		{
			i0&& i1
				? *i0 < *i1 ? ++i0 : ++i1
				: i0 ? ++i0 : ++i1;

			return *this;
		}
	};
#endif // 0

} // namespace fms::iterable
