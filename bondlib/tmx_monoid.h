// tmx_monoid.h - monoids
#pragma once
#include <cmath>
#include <functional>
#include <iterator>
#include <limits>
#include <type_traits>
#include "tmx_iterable.h"

namespace tmx {

	// associative binary operation with identity
	template<class X>
	struct monoid_base {
		constexpr bool eq(const monoid_base<X>& m) const
		{
			return _eq(m);
		}
		constexpr X id() const
		{
			return _id();
		}
		constexpr X op(const X& x, const X& y) const
		{
			return _op(x, y);
		}
	private:
		virtual bool _eq() const = 0;
		virtual X _id() const = 0;
		virtual X _op(const X& x, const X& y) const = 0;
	};

	template<class X, class Op>
	class monoid : public monoid_base<X>
	{
		X id_;
		Op op_;
	public:
		constexpr monoid(X id, Op op)
			: id_(id)//, op_(op)
		{ }
		constexpr monoid(const monoid&) = default;
		constexpr monoid& operator=(const monoid&) = default;
		constexpr ~monoid() = default;	

		constexpr bool operator==(const monoid& m) const
		{
			return _eq(m);
		}

		constexpr bool _eq(const monoid& m) const override
		{
			return id_ == m.id_ and op_ == m.op;
		}

		constexpr X _id() const override
		{
			return id_;
		}
		constexpr X _op(const X& x, const X& y) const override
		{
			return op_(x, y);
		}
	};

	template<class X>
	constexpr auto plus_monoid = monoid(X(0), std::plus<X>{});
	template<class X>
	constexpr auto multiplies_monoid = monoid(X(1), std::multiplies<X>{});
	template<class X>
	constexpr auto max_monoid = monoid(-std::numeric_limits<X>::max(),
		[](const X& x, const X& y) { return std::max<X>(x, y); });
	template<class X>
	constexpr auto min_monoid = monoid(std::numeric_limits<X>::max(),
		[](const X& x, const X& y) { return std::min<X>(x, y); });

	// fold iterable ??? foldl and foldr
	template<class X, class Op, class I>
	class scan {
		monoid<X, Op> m;
		I i;
		X x;
	public:
		constexpr scan(monoid<X, Op> m, I i)
			: m(m), i(i), x(m.id())
		{ }
		constexpr scan(const scan&) = default;
		constexpr scan& operator=(const scan&) = default;
		constexpr ~scan() = default;

		constexpr bool operator==(const scan& f) const
		{
			return m == f.m && i == f.i && x == f.x;
		};

		constexpr explicit operator bool() const
		{
			return i.operator bool();
		}
		constexpr auto operator*() const
		{
			return x;
		}
		constexpr scan& operator++()
		{
			if (i) {
				x = m.op(x, *i);
				++i;
			}

			return *this;
		}

	};

#ifdef _DEBUG
	template<class X>
	inline int monoid_test()
	{
		{
			auto m = plus_monoid<X>;
			auto i = iota_iterable<X>{};
			auto f = scan(m, i);
			auto f2{ f };
			assert(f == f2);
			f = f2;
			assert(!(f != f2));
			assert(f);
		}
		{
			constexpr auto m = plus_monoid<X>;
			assert (m.id() == 0);
			assert (m.op(1, 2) == 3);
			assert(m.op(m.id(), 3) == 3);
			assert(m.op(3, m.id()) == 3);
			assert(m.op(1, m.op(2, 3)) == m.op(m.op(1, 2), 3));
		}
		{
			constexpr auto m = plus_monoid<X>;
			assert(m.id() == 0);
			assert(m.op(1, 2) == 3);
			assert(m.op(m.id(), 3) == 3);
			assert(m.op(3, m.id()) == 3);
			assert(m.op(1, m.op(2, 3)) == m.op(m.op(1, 2), 3));
		}
		{
			constexpr auto m = max_monoid<X>;
			assert(m.id() == -std::numeric_limits<X>::max());
			assert(m.op(1, 2) == 2);
			assert(m.op(m.id(), 3) == 3);
			assert(m.op(3, m.id()) == 3);
			assert(m.op(1, m.op(2, 3)) == m.op(m.op(1, 2), 3));
		}
		{
			constexpr auto m = min_monoid<X>;
			assert(m.id() == std::numeric_limits<X>::max());
			assert(m.op(1, 2) == 1);
			assert(m.op(m.id(), 3) == 3);
			assert(m.op(3, m.id()) == 3);
			assert(m.op(1, m.op(2, 3)) == m.op(m.op(1, 2), 3));
		}

		return 0;
	}
#endif // _DEBUG

	template<class X>
	class mean {
		size_t n;
		X s; // running average
	public:
		constexpr mean(size_t n = 0, X s = 0)
			: n(n), s(s)
		{ }
		constexpr bool operator==(const mean& m) const = default;
		constexpr mean op(const mean& m) const
		{
			return mean(n + m.n, (s*n + m.s*m.n)/(n + m.n));
		}
	};
	template<class X>
	constexpr auto mean_monoid = monoid(mean<X>{}, [](const mean<X>& x, const mean<X>& y) { return x.op(y); });

#ifdef _DEBUG
	template<class X>
	inline int mean_monoid_test()
	{
		{
			constexpr auto m = mean_monoid<X>;
			assert(m.id() == mean<X>{});
			constexpr auto m1 = mean<X>(1, 2);
			constexpr auto m2 = mean<X>(3, 4);
			assert(m.op(m1, m2) == mean<X>(4, X(1*2 + 3*4)/4));
			assert(m.op(m.id(), m1) == m1);
			assert(m.op(m2, m.id()) == m2);
			constexpr auto m3 = mean<X>(5, 6);
			assert(m.op(m1, m.op(m2, m3)) == m.op(m.op(m1, m2), m3));
		}

		return 0;
	}
#endif // _DEBUG

	// class variance ...

}; // namespace tmx
