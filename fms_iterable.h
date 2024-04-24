// fms_iterable.h - iterator with operator bool() to detect the end
#pragma once
#include <compare>
#include <functional>
#include <initializer_list>
#include <limits>
#include <memory>
#include <type_traits>

namespace fms::iterable {
	
	/*
	template<class T> struct interface;

	template <typename I>
	concept IsReferenceToBase = std::is_base_of_v<fms::iterable::interface<typename I::value_type>, std::remove_reference_t<I>>;
	*/

	template<class I>
	concept input = requires (I i) {
		{ i.operator bool() } -> std::same_as<bool>;
		{ *i } -> std::convertible_to<typename I::value_type>;
//		{ ++i } -> IsReferenceToBase;
	};


	// NVI interface for iterable.
	template<class T>
	struct interface {
		using value_type = T;

		virtual ~interface() { };

		virtual interface* clone() const = 0;
		virtual void destroy() = 0;
		
		explicit operator bool() const
		{
			return op_bool();
		}
		T operator*() const
		{
			return op_star();
		}
		interface& operator++()
		{
			return op_incr();
		}
	private:
		virtual bool op_bool() const = 0;
		virtual T op_star() const = 0;
		virtual interface& op_incr() = 0;
	};

	template<input I, input J>
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
	template<input I>
	inline std::size_t length(I i, std::size_t n = 0) noexcept
	{
		while (i) {
			++i;
			++n;
		}

		return n;
	}

	// Drop at most n from the beginning.
	template<input I>
	inline I skip(I i, std::size_t n) noexcept
	{
		while (i && n > 0) {
			++i;
			--n;
		}

		return i;
	}

	// Last iterable element if finite length.
	template<input I>
	inline I back(I i)
	{
		I _i(i);

		while (++_i) {
			i = _i; // move, swap???
		}

		return i;
	}

	// Constant iterable.
	template<class T>
	class constant : public interface<T> {
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

		constant* clone() const override
		{
			return new constant(*this);
		}
		void destroy() override
		{
			delete this;
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

	// t, t+1, t+2, ...
	template<class T>
	class iota : public interface<T> {
		T t;
	public:
		iota(T t = 0) noexcept
			: t(t)
		{ }

		bool operator==(const iota& i) const
		{
			return t == i.t;
		}

		iota* clone() const override
		{
			return new iota(*this);
		}
		void destroy() override
		{
			delete this;
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
	class power : public interface<T> {
		T t, tn;
	public:
		power(T t, T tn = 1)
			: t(t), tn(tn)
		{ }

		bool operator==(const power& p) const
		{
			return t == p.t && tn == p.tn;
		}

		power* clone() const override
		{
			return new power(*this);
		}
		void destroy() override
		{
			delete this;
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
	class factorial : public interface<T> {
		T t, n;
	public:
		factorial(T t = 1)
			: t(t), n(1)
		{ }

		bool operator==(const factorial& f) const
		{
			return t == f.t && n == f.n;
		}

		factorial* clone() const override
		{
			return new factorial(*this);
		}
		void destroy() override
		{
			delete this;
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
	class pointer : public interface<T> {
		T* p;
	public:
		pointer(T* p) noexcept
			: p(p)
		{ }

		bool operator==(const pointer& _p) const
		{
			return p == _p.p;
		}

		pointer* clone() const override
		{
			return new pointer(*this);
		}
		void destroy() override
		{
			delete this;
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
	class null_terminated_pointer : public interface<T> {
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

		null_terminated_pointer* clone() const override
		{
			return new null_terminated_pointer(*this);
		}
		void destroy() override
		{
			delete this;
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

	// Iterable having exactly one element.
	template<class T>
	class singleton : public interface<T> {
		T t;
		bool b;
	public:
		singleton(T t) noexcept
			: t(t), b(true)
		{ }

		bool operator==(const singleton& o) const
		{
			return t == o.t && b == o.b;
		}

		singleton* clone() const override
		{
			return new singleton(*this);
		}
		void destroy() override
		{
			delete this;
		}

		bool op_bool() const noexcept override
		{
			return b;
		}
		T op_star() const noexcept override
		{
			return t;
		}
		singleton& op_incr() noexcept override
		{
			b = false;

			return *this;
		}
	};

	// Take at most n elements.
	template<input I, class T = I::value_type>
	class take : public interface<T> {
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

		take* clone() const override
		{
			return new take(*this);
		}
		void destroy() override
		{
			delete this;
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
	template<input I0, input I1, class T = std::common_type_t<typename I0::value_type, typename I1::value_type>>
	class concatenate : public interface<T> {
		I0 i0;
		I1 i1;
	public:
		concatenate(const I0& i0, const I1& i1)
			: i0(i0), i1(i1)
		{ }

		concatenate* clone() const override
		{
			return new concatenate(*this);
		}
		void destroy() override
		{
			delete this;
		}

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
	template<input I0, input I1, class T = std::common_type_t<typename I0::value_type, typename I1::value_type>>
	class merge : public interface<T> {
		I0 i0;
		I1 i1;
	public:
		merge(const I0& i0, const I1& i1)
			: i0(i0), i1(i1)
		{ }

		merge* clone() const override
		{
			return new merge(*this);
		}
		void destroy() override
		{
			delete this;
		}

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
			i0 && i1
				? *i0 < *i1 ? ++i0 : ++i1
				: i0 ? ++i0 : ++i1;

			return *this;
		}
	};

	// Apply a function to elements of an iterable.
	template<class F, input I, class T = typename I::value_type,
		class U = std::invoke_result_t<F, T>>
		class apply : public interface <U>
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

		apply* clone() const override
		{
			return new apply(*this);
		}
		void destroy() override
		{
			delete this;
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
	template<class BinOp, input I0, input I1, class T0 = typename I0::value_type, class T1 = typename I1::value_type,
		class T = std::invoke_result_t<BinOp, T0, T1>>
	class binop : public interface<T> {
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

		binop* clone() const override
		{
			return new binop(*this);
		}
		void destroy() override
		{
			delete this;
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

	// Elements satisfying predicate.
	template<class P, input I, class T = typename I::value_type>
	class filter : public interface <T>
	{
		const P& p;
		I i;
		void incr()
		{
			while (++i && !p(*i)) {
				;
			}
		}
	public:
		filter(const P& p, const I& i)
			: p(p), i(i)
		{
			incr();
		}
		filter(const filter& a)
			: p(a.p), i(a.i)
		{ }
		filter& operator=(const filter& a)
		{
			if (this != &a) {
				i = a.i;
			}

			return *this;
		}
		~filter()
		{ }

		bool operator==(const filter& a) const
		{
			return p == a.p and i == a.i;
		}

		filter* clone() const override
		{
			return new filter(*this);
		}
		void destroy() override
		{
			delete this;
		}

		bool op_bool() const override
		{
			return i.op_bool();
		}
		T op_star() const override
		{
			return *i;
		}
		filter& op_incr() override
		{
			incr();

			return *this;
		}
	};

	// Stop at first element satisfying predicate.
	template<class P, input I, class T = typename I::value_type>
	class until : public interface <T>
	{
		const P& p;
		I i;
	public:
		until(const P& p, const I& i)
			: p(p), i(i)
		{ }
		until(const until& a)
			: p(a.p), i(a.i)
		{ }
		until& operator=(const until& a)
		{
			if (this != &a) {
				i = a.i;
			}

			return *this;
		}
		~until()
		{ }

		bool operator==(const until& a) const
		{
			return p == a.p and i == a.i;
		}
		
		until* clone() const override
		{
			return new until(*this);
		}
		void destroy() override
		{
			delete this;
		}

		bool op_bool() const override
		{
			return i.op_bool() && !p(*i);
		}
		T op_star() const override
		{
			return *i;
		}
		until& op_incr() override
		{
			if (op_bool()) {
				++i;
			}

			return *this;
		}
	};

	// Right fold: t, op(t, *i), op(op(t, *i), *++i), ...
	template<class BinOp, input I, class T = typename I::value_type>
	class fold : public interface<T>
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

		fold* clone() const override
		{
			return new fold(*this);
		}
		void destroy() override
		{
			delete this;
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
	template<input I, class T = typename I::value_type>
	inline auto sum(I i, T t = 0)
	{
		while (i) {
			t += *i;
			++i;
		}

		return t;
	} 
	template<input I, class T = typename I::value_type>
	inline auto prod(I i, T t = 1)
	{
		while (i) {
			t *= *i;
			++i;
		}

		return t;
	}
	//inline auto horner(I i, T x, T t = 1)

	// Precompute values.
	template<input I, class T, std::size_t N>
	class cache : public interface<T> {
		std::array<T,N> a;
		std::size_t n;
	public:
		cache(I i)
			: n(0)
		{
			while (i && n < N) {
				a[n++] = *i;
				++i;
			}
			n = 0;
		}

		cache* clone() const override
		{
			return new cache(*this);
		}
		void destroy() override
		{
			delete this;
		}

		bool op_bool() const override
		{
			return n < N;
		}
		T op_star() const override
		{
			return a[n];
		}
		cache& op_incr() override
		{
			++n;

			return *this;
		}
	};

	// d(*++i, *i), d(*++++i, *++i), ...
	template<input I, class T = typename I::value_type, class D = std::minus<T>>
	class delta : public interface<T> {
		const D& d;
		I i;
		T t, _t;
	public:
		delta(const I& _i, const D& _d = std::minus<T>{})
			: d(_d), i(_i)
		{
			if (i) {
				t = *i;
				++i;
				_t = i ? *i : t;
			}
		}
		delta(const delta& _d)
			: d(_d.d), i(_d.i), t(_d.t), _t(_d._t)
		{ }
		delta& operator=(const delta& _d)
		{
			if (this != &_d) {
				i = _d.i;
				t = _d.t;
				_t = _d._t;
			}

			return *this;
		}
		~delta()
		{ }

		bool operator==(const delta& _d) const
		{
			return i == _d.i && t == _d.t && _t == _d._t;
		}

		delta* clone() const override
		{
			return new delta(*this);
		}
		void destroy() override
		{
			delete this;
		}

		bool op_bool() const override
		{
			return i.op_bool();
		}
		T op_star() const override
		{
			return d(*i, t);
		}
		delta& op_incr() override
		{
			if (i) {
				t = *i;
				++i;
			}

			return *this;
		}
	};

} // namespace fms::iterable

#define FMS_ITERABLE_OPERATOR(X) \
X(+, std::plus<T>{}) \
X(-, std::minus<T>{}) \
X(*, std::multiplies<T>{}) \
X(/, std::divides<T>{}) \
X(%, std::modulus<T>{}) \

#define FMS_ITERABLE_OPERATOR_FUNCTION(OP, OP_) \
template<fms::iterable::input I, fms::iterable::input J, class T = std::common_type_t<typename I::value_type, typename J::value_type>> \
inline auto operator OP(I i, J j) { return fms::iterable::binop(OP_, i, j); } \

  //template<fms::iterable::input J, class T = typename J::value_type> \
//inline auto operator OP(const T& i, J j) { return fms::iterable::binop(OP_, fms::iterable::constant(i), j); } \

FMS_ITERABLE_OPERATOR(FMS_ITERABLE_OPERATOR_FUNCTION)
#undef FMS_ITERABLE_OPERATOR_FUNCTION

