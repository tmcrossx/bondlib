// fms_iterable.h - iterator with operator bool()
#pragma once
#include <type_traits>

namespace fms::iterable {

	template<class T>
	struct base {
		using value_type = T;
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
	class constant : public base<T> {
		T c;
	public:
		constant(T c) noexcept
			: c(c)
		{ }

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

	// sequence

	// Unsafe pointer interface.
	template<class T>
	class pointer : public base<T> {
		T* p;
	public:
		pointer(T* p) noexcept
			: p(p)
		{ }

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
	class null_terminated_pointer : public base<T> {
		T* p;
	public:
		null_terminated_pointer(T* p) noexcept
			: p(p)
		{ }

		bool op_bool() const noexcept override
		{
			return *p != 0;
		}
		T op_star() const noexcept override
		{
			return *p;
		}
		null_terminated_pointer& op_incr() noexcept override
		{
			if (op_bool())
				++p;

			return *this;
		}
	};

	// Take at most n elements.
	template<class I, class T = I::value_type>
	class take : public base<T> {
		I i;
		std::size_t n;
	public:
		take(const I& i, std::size_t n)
			: i(i), n(n)
		{ }

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

	// Assumes lifetime of a[N].
	template<class T, std::size_t N>
	inline auto array(T(&a)[N]) noexcept
	{
		return take<pointer<T>, T>(pointer<T>(a), N);
	}

	// i0 then i1
	template<class I0, class I1, class T = std::common_type_t<typename I0::value_type, typename I1::value_type>>
	class concatenate : public base<T> {
		I0 i0;
		I1 i1;
	public:
		concatenate(const I0& i0, const I1& i1)
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

	// sorted i0 and i1 in order
	template<class I0, class I1, class T = std::common_type_t<typename I0::value_type, typename I1::value_type>>
	class merge : public base<T> {
		I0 i0;
		I1 i1;
	public:
		merge(const I0& i0, const I1& i1)
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

	template<class I, class J>
	inline bool equal(I i, J j) noexcept
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

	template<class I>
	inline std::size_t length(I i, std::size_t n = 0) noexcept
	{
		while (i) {
			++i;
			++n;
		}

		return n;
	}

	template<class I>
	inline I skip(I i, std::size_t n) noexcept
	{
		while (i && n > 0) {
			++i;
			--n;
		}

		return i;
	}

} // namespace fms::iterable
