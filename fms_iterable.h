// fms_iterable.h - iterator with operator bool() const to detect the end
#pragma once
#include <functional>
#include <initializer_list>

#include <type_traits>
#include <vector>

namespace fms::iterable {
	
	/* TODO: concepts
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

	// non-virtual interface for input iterable.
	template<class T>
	struct interface {
		using value_type = T;

		virtual ~interface() { };
		
		operator bool() const
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

	//
	// Stand alone functions
	//

	// All elements equal.
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
	inline I drop(I i, std::size_t n) noexcept
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

	// For use with STL
	template<input I>
	inline I end(I i)
	{
		while (i) {
			++i;
		}

		return i;
	}

	// Make STL container iterable. Assumes lifetime of c.
	template<class C, class T = typename C::value_type>
	class container : public interface<T> {
		const C& c;
		typename C::const_iterator i;
	public:
		container(const C& _c)
			: c(_c), i(c.begin())
		{ }

		bool operator==(const container& _c) const
		{
			return &c == &_c.c && i == _c.i;
		}

		bool op_bool() const override
		{
			return i != c.end();
		}
		T op_star() const override
		{
			return *i;
		}
		container& op_incr() override
		{
			if (op_bool()) {
				++i;
			}

			return *this;
		}
	};

	template<class T>
	class list : public interface<T> {
		std::vector<T> l;
		std::size_t i;
	public:
		// E.g., list({1,2,3})
		list(std::initializer_list<T> t)
			: l(t), i(0)
		{ }

		// same vector
		bool operator==(const list& _l) const
		{
			return l == _l.l && i == _l.i;
		}

		bool op_bool() const override
		{
			return i < l.size();
		}
		T op_star() const override
		{
			return l[i];
		}
		list& op_incr() override
		{
			if (op_bool()) {
				++i;
			}

			return *this;
		}
	};
	// Constant iterable: {c, c, c, ...}
	template<class T>
	class constant : public interface<T> {
		T c;
	public:
		constant(T c) noexcept
			: c(c)
		{ }

		bool operator==(const constant& _c) const = default;

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

	// t, t + 1, t + 2, ...
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

	// 1, 1, 2, 6, 24, ...
	template<class T = double>
	class factorial : public interface<T> {
		T t, n;
	public:
		factorial(T t = 1)
			: t(t), n(1)
		{ }

		bool operator==(const factorial& f) const = default;

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
		// pointer() is empty iterator
		pointer(T* p = nullptr) noexcept
			: p(p)
		{ }

		bool operator==(const pointer& _p) const = default;

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

		bool operator==(const null_terminated_pointer& _p) const = default;
		
		bool op_bool() const override
		{
			return *p != 0;
		}
		T op_star() const override
		{
			return *p;
		}
		null_terminated_pointer& op_incr() override
		{
			if (op_bool())
				++p;

			return *this;
		}
	};

	// Iterable having exactly one element. {t}
	template<class T>
	class once : public interface<T> {
		T t;
		bool b;
	public:
		once(T t) noexcept
			: t(t), b(true)
		{ }

		bool operator==(const once& o) const = default;

		bool op_bool() const noexcept override
		{
			return b;
		}
		T op_star() const noexcept override
		{
			return t;
		}
		once& op_incr() noexcept override
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

		bool operator==(const take& t) const = default;

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
		return take(pointer<T>(a), N);
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

		bool operator==(const concatenate& i) const = default;

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

	// add sorted i0 and i1 in order
	template<input I0, input I1, class T = std::common_type_t<typename I0::value_type, typename I1::value_type>>
	class merge : public interface<T> {
		I0 i0;
		I1 i1;
	public:
		merge(const I0& i0, const I1& i1)
			: i0(i0), i1(i1)
		{ }

		bool operator==(const merge& i) const = default;

		bool op_bool() const override
		{
			return i0 || i1;
		}
		T op_star() const override
		{
			if (i0 && i1) {
				if (*i0 < *i1) {
					return *i0;
				}
				else if (*i1 < *i0) {
					return *i1;
				}
				else {
					return *i0 + *i1;
				}
			}

			return i0 ? *i0 : *i1;
		}
		merge& op_incr() override
		{
			if (i0 && i1) {
				if (*i0 < *i1) {
					++i0;
				}
				else if (*i1 < *i0) {
					++i1;
				}
				else {
					++i0;
					++i1;
				}
			}
			else {
				if (i0) {
					++i0;
				}
				else if (i1) {
					++i1;
				}
			}

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
			return f == a.f && i == a.i;
		}

		bool op_bool() const override
		{
			return i;
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
			return op == o.op && i0 == o.i0 && i1 == o.i1;
		}

		bool op_bool() const override
		{
			return i0 && i1;
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
			while (i && ++i && !p(*i)) {
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

		bool op_bool() const override
		{
			return i;
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

		bool op_bool() const override
		{
			return i && !p(*i);
		}
		T op_star() const override
		{
			return *i;
		}
		until& op_incr() override
		{
			++i;

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

		bool op_bool() const override
		{
			return i;
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
	template<input I, class T = typename I::value_type>
	class cache : public interface<T> {
		std::vector<T> a;
		std::size_t n;
	public:
		// Consume i.
		cache(I i)
			: n(0)
		{
			while (i) {
				a.push_back(*i);
				++i;
			}
		}

		bool op_bool() const override
		{
			return n < a.size();
		}
		T op_star() const override
		{
			return a[n];
		}
		cache& op_incr() override
		{
			if (op_bool()) {
				++n;
			}

			return *this;
		}
	};

	// d(*++i, *i), d(*++++i, *++i), ...
	template<input I, class T = typename I::value_type, class D = std::minus<T>, typename U = std::invoke_result_t<D,T,T>>
	class delta : public interface<U> {
		const D& d;
		I i;
		T t, _t;
	public:
		using value_type = U;

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

		bool op_bool() const override
		{
			return i;
		}
		U op_star() const override
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

	// reversed delta d(*++i, *i), d(*++++i, *++i), ...
	template<input I, class T = typename I::value_type, class D = std::minus<T>, typename U = std::invoke_result_t<D, T, T>>
	class nabla : public interface<U> {
		const D& d;
		I i;
		T t, _t;
	public:
		using value_type = U;

		nabla(const I& _i, const D& _d = std::minus<T>{})
			: d(_d), i(_i)
		{
			if (i) {
				t = *i;
				++i;
				_t = i ? *i : t;
			}
		}
		nabla(const nabla& _d)
			: d(_d.d), i(_d.i), t(_d.t), _t(_d._t)
		{ }
		nabla& operator=(const nabla& _d)
		{
			if (this != &_d) {
				i = _d.i;
				t = _d.t;
				_t = _d._t;
			}

			return *this;
		}
		~nabla()
		{ }

		bool operator==(const nabla& _d) const
		{
			return i == _d.i && t == _d.t && _t == _d._t;
		}

		bool op_bool() const override
		{
			return i;
		}
		U op_star() const override
		{
			return d(t, *i);
		}
		nabla& op_incr() override
		{
			if (i) {
				t = *i;
				++i;
			}

			return *this;
		}
	};


	// uptick + downtick = delta
	template<input I, class T = typename I::value_type>
	inline auto uptick(I i)
	{
		return delta(i, [](T a, T b) { return std::max<T>(b - a, 0); });
	}
	template<input I, class T = typename I::value_type>
	inline auto downtick(I i)
	{
		return delta(i, [](T a, T b) { return std::min<T>(b - a, 0); });
	}

	// {f(), f(), f(), ...}
	template<class F, class T = std::invoke_result_t<F>>
	class call : public interface<T> {
		const F& f;
	public:
		call(const F& f)
			: f(f)
		{ }

		bool operator==(const call& c) const
		{
			return f == c.f;
		}

		bool op_bool() const override
		{
			return true;
		}
		T op_star() const override
		{
			return f();
		}
		call& op_incr() override
		{
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
inline auto operator OP(const I& i, const J& j) { return fms::iterable::binop(OP_, i, j); } \

FMS_ITERABLE_OPERATOR(FMS_ITERABLE_OPERATOR_FUNCTION)
#undef FMS_ITERABLE_OPERATOR_FUNCTION

template<fms::iterable::input I, fms::iterable::input J>
inline auto operator,(const I& i, const J& j)
{
	return fms::iterable::concatenate(i, j);
}

