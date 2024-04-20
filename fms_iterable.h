// fms_iterable.h - iterator with operator bool()
#pragma once
#include <compare>
#include <functional>

namespace fms::iterable {

	// TODO: add fms::iterable::input concept

	// NVI interface for iterable.
	template<class T>
	struct base {
		using value_type = T;

		virtual ~base() { };
		
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

	// TODO: replace class with iterable::input concept
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

	// length(i, length(j)) = length(i) + length(j)
	template<class I>
	inline std::size_t length(I i, std::size_t n = 0) noexcept
	{
		while (i) {
			++i;
			++n;
		}

		return n;
	}

	// Drop at most n from the beginning.
	template<class I>
	inline I skip(I i, std::size_t n) noexcept
	{
		while (i && n > 0) {
			++i;
			--n;
		}

		return i;
	}

	// Last iterable element if finite length.
	template<class I>
	inline I last(I i)
	{
		I _i(i);

		while (++_i) {
			i = _i;
		}

		return i;
	}

	// skip until predicate is true
	template<class P, class I>
	inline I until(const P& p, I i)
	{
		while (i && !p(*i)) {
			++i;
		}

		return i;
	}

	// Constant iterable.
	template<class T>
	class constant : public base<T> {
		T c;
	public:
		constant(T c) noexcept
			: c(c)
		{ }

		// auto operator<=>(const constant&) const = default not working for MSVC
		bool operator==(const constant& _c) const
		{
			return c == _c.c;
		}

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
	static_assert(std::is_same_v<constant<int>::value_type, base<int>::value_type>);

	// t, t+1, t+2, ...
	template<class T>
	class iota : public base<T> {
		T t;
	public:
		iota(T t = 0) noexcept
			: t(t)
		{ }

		bool operator==(const iota& i) const
		{
			return t == i.t;
		}

		bool op_bool() const noexcept override
		{
			return true;
		}
		T op_star() const noexcept override
		{
			return t;
		}
		iota& op_incr() noexcept override
		{
			++t;

			return *this;
		}
	};

	// tn, tn*t, tn*t*t, ...
	template<class T>
	class power : public base<T> {
		T t, tn;
	public:
		power(T t, T tn = 1)
			: t(t), tn(tn)
		{ }

		bool operator==(const power& p) const
		{
			return t == p.t && tn == p.tn;
		}

		bool op_bool() const override
		{
			return true;
		}
		T op_star() const override
		{
			return tn;
		}
		power& op_incr() override
		{
			tn *= t;

			return *this;
		}
	};

	template<class T = double>
	class factorial : public base<T> {
		T t, n;
	public:
		factorial(T t = 1)
			: t(t), n(1)
		{ }

		bool operator==(const factorial& f) const
		{
			return t == f.t && n == f.n;
		}

		bool op_bool() const override
		{
			return true;
		}
		T op_star() const override
		{
			return t;
		}
		factorial& op_incr() override
		{
			t *= n++;

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

		bool operator==(const pointer& _p) const
		{
			return p == _p.p;
		}

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

	// Terminate on 0 value.
	template<class T>
	class null_terminated_pointer : public base<T> {
		T* p;
	public:
		null_terminated_pointer(T* p) noexcept
			: p(p)
		{ }
		null_terminated_pointer(const null_terminated_pointer& _p)
			: p(_p.p)
		{ }
		null_terminated_pointer& operator=(const null_terminated_pointer& _p)
		{
			if (this != &_p) {
				p = _p.p;
			}

			return *this;
		}
		~null_terminated_pointer()
		{ }

		bool operator==(const null_terminated_pointer& _p) const
		{
			return p == _p.p;
		}

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

		bool operator==(const take& t) const
		{
			return i == t.i && n == t.n;
		}

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

	// Apply a function to elements of an iterable.
	template<class F, class I, class T = typename I::value_type,
		class U = std::invoke_result_t<F, T>>
		class apply : public base <U>
	{
		const F& f;
		I i;
	public:
		using value_type = U;

		apply(const F& f, const I& i)
			: f(f), i(i)
		{ }
		apply(const apply& a)
			: f(a.f), i(a.i)
		{ }
		apply& operator=(const apply& a)
		{
			if (this != &a) {
				//f = a.f;
				i = a.i;
			}

			return *this;
		}
		~apply()
		{ }

		bool operator==(const apply& a) const
		{
			return f == a.f and i == a.i;
		}

		bool op_bool() const override
		{
			return i.op_bool();
		}
		U op_star() const override
		{
			return f(*i);
		}
		apply& op_incr() override
		{
			++i;

			return *this;
		}
	};

	// Apply a binary operation to elements of two iterable.
	template<class BinOp, class I0, class I1, class T0 = typename I0::value_type, class T1 = typename I1::value_type,
		class T = std::invoke_result_t<BinOp, T0, T1>>
	class binop : public base<T> {
		const BinOp& op;
		I0 i0;
		I1 i1;
	public:
		binop(const BinOp& op, I0 i0, I1 i1)
			: op(op), i0(i0), i1(i1)
		{ }
		binop(const binop& o)
			: op(o.op), i0(o.i0), i1(o.i1)
		{ }
		binop& operator=(const binop& o)
		{
			if (this != &o) {
				i0 = o.i0;
				i1 = o.i1;
			}

			return *this;
		}
		~binop()
		{ }

		bool operator==(const binop& o) const
		{
			return i0 == o.i0 && i1 == o.i1;
		}

		bool op_bool() const override
		{
			return i0.op_bool() && i1.op_bool();
		}
		T op_star() const override
		{
			return op(*i0, *i1);
		}
		binop& op_incr() override
		{
			++i0;
			++i1;

			return *this;
		}
	};

	// Right fold: t, op(t, *i), op(op(t, *i), *++i), ...
	template<class BinOp, class I, class T = typename I::value_type>
	class fold : public base<T>
	{
		const BinOp& op;
		I i;
		T t;
	public:
		fold(const BinOp& op, const I& i, T t = 0)
			: op(op), i(i), t(t)
		{ }
		fold(const fold& f)
			: op(f.op), i(f.i), t(f.t)
		{ }
		fold& operator=(const fold& f)
		{
			if (this != &f) {
				i = f.i;
				t = f.t;
			}

			return *this;
		}
		~fold()
		{ }

		bool operator==(const fold& f) const
		{
			return i == f.i && t == f.t; // BinOp is part of type
		}

		bool op_bool() const override
		{
			return i.op_bool();
		}
		T op_star() const override
		{
			return t;
		}
		fold& op_incr() override
		{
			if (i) {
				t = op(t, *i);
				++i;
			}

			return *this;
		}
	};
	template<class I, class T = typename I::value_type>
	inline auto sum(I i)
	{
		return fold(std::plus<T>{}, i, T(0)) ;
	} 
	template<class I, class T = typename I::value_type>
	inline T series(I i, T eps = std::numeric_limits<T>::epsilon())
	{
		T s = 0;

		while (i && std::fabs(*i) > eps) {
			s += *i;
			++i;
		}

		return s;
	}
	template<class I, class T = typename I::value_type>
	inline auto prod(I i)
	{
		return fold(std::multiplies<T>{}, i, T(1));
	}


} // namespace fms::iterable

// TODO: use fms::iterable::input concept
template<class I, class J, class T = std::common_type_t<typename I::value_type, typename J::value_type>>
inline auto operator+(I i, J j)
{
	return fms::iterable::binop(std::plus<T>{}, i, j);
}
template<class I, class J, class T = std::common_type_t<typename I::value_type, typename J::value_type>>
inline auto operator-(I i, J j)
{
	return fms::iterable::binop(std::minus<T>{}, i, j);
}
template<class I, class J, class T = std::common_type_t<typename I::value_type, typename J::value_type>>
inline auto operator*(I i, J j)
{
	return fms::iterable::binop(std::multiplies<T>{}, i, j);
}
template<class I, class J, class T = std::common_type_t<typename I::value_type, typename J::value_type>>
inline auto operator/(I i, J j)
{
	return fms::iterable::binop(std::divides<T>{}, i, j);
}
