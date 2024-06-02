// tmx_instrument.h - times and cash flows
#pragma once
#ifdef _DEBUG
#include <cassert>
#endif // _DEBUG
#include <initializer_list>
#include "../fms_iterable/fms_iterable.h"
#include "tmx_cash_flow.h"

namespace tmx::instrument {

	//template<fms::iterable::input IU, fms::iterable::input IC>
	//concept input = fms::iterable::input<cash_flow<typename IU::value_type, typename IC::value_type>>;
	template<class U = double, class C = double>
	struct interface {
		using iterator_category = std::input_iterator_tag;
		using value_type = std::pair<U, C>;

		virtual ~interface()
		{ }

		explicit operator bool() const
		{
			return op_bool();
		}
		std::pair<U, C> operator*() const
		{
			return op_star();
		}
		interface& operator++()
		{
			return op_incr();
		}
	private:
		virtual bool op_bool() const = 0;
		virtual std::pair<U,C> op_star() const = 0;
		virtual interface& op_incr() = 0;
	};

	// Fixed income instrument from time and cash flow iterables.
	template<fms::iterable::input IU, fms::iterable::input IC,
		class U = typename IU::value_type, class C = typename IC::value_type>
	class iterable : public interface<U,C>	{
		IU u;
		IC c;
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = std::pair<U,C>;

		iterable() = default;
		iterable(const IU& u, const IC& c)
			: u(u), c(c)
		{ }
		iterable(const iterable&) = default;
		iterable& operator=(const iterable&) = default;
		~iterable() = default;

		bool operator==(const iterable&) const = default;

		IU time() const
		{
			return u;
		}
		IC cash() const
		{
			return c;
		}

		bool op_bool() const override
		{
			return u && c;
		}
		value_type op_star() const
		{
			return cash_flow(*u, *c);
		}
		iterable& operator++()
		{
			++u;
			++c;

			return *this;
		}
	};

	// Instrument value type
	template<class U = double, class C = double>
	class value : public interface<U,C> {
		std::vector<U> u;
		std::vector<C> c;
		size_t i;
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = std::pair<U, C>;

		value(const std::vector<U>& u, const std::vector<C>& c)
			: u(u), c(c), i(0)
		{ }
		value(size_t n, const U* u, const C* c)
			: u(u, u + n), c(c, c + n), i(0)
		{ }
		value(const std::initializer_list<U>& u, const std::initializer_list<C>& c)
			: u(u), c(c), i(0)
		{
			// assert(u.size() == c.size());
		}
		template<fms::iterable::input I>
			requires std::same_as<typename I::value_type, std::pair<U,C>>
		value(I i)
			: i(0)
		{
			for (const auto [u_, c_] : i) {
				u.push_back(u_);
				c.push_back(c_);
			}
		}	
		value(const value&) = default;
		value(value&&) = default;
		value& operator=(const value&) = default;
		value& operator=(value&&) = default;
		~value() = default;

		// Strong equality
		bool operator==(const value& v) const = default; // i == i && u.begin() == && c.begin() == 

		value& reset(size_t i_ = 0)
		{
			i = i_;

			return *this;
		}

		auto time() const
		{
			return fms::iterable::make_interval(u);
		}
		auto cash() const
		{
			return fms::iterable::make_interval(c);
		}

		bool op_bool () const override
		{
			return i < u.size();
		}
		value_type op_star() const override
		{
			return { u[i], c[i] };
		}
		value& op_incr()
		{
			if (op_bool()) {
				++i;
			}

			return *this;
		}
	};
	// E.g., make_iterable({1,2,3}, {.2,.3,.4})
	template<class U, class C>
	inline auto make_iterable(const std::initializer_list<U>& u, const std::initializer_list<C>& c)
	{
		return iterable(fms::iterable::vector(u), fms::iterable::vector(c));
	}
	template<class U, class C>
	inline auto make_iterable(const value<U,C>& v)
	{
		return iterable(v.time(), v.cash());
	}
#ifdef _DEBUG
	inline int iterable_test()
	{
		{
			auto u = std::vector({ 1,2,3 });
			auto c = std::vector({ 2,3,4 });
			auto i = value(u, c); // iterable(fms::iterable::make_interval(u), fms::iterable::make_interval(c));
			assert(i);
			assert(*i == cash_flow(1, 2));
			++i;
			assert(i);
			assert(*i == cash_flow(2, 3));
			++i;
			assert(i);
			assert(*i == cash_flow(3, 4));
			++i;
			assert(!i);
		}
		{
			auto u = std::vector({ 1,2,3 });
			auto c = std::vector({ 2,3,4 });
			auto ii = value({ 1,2,3 }, { 2,3,4 });
			auto i = iterable(ii.time(), ii.cash());
			assert(i);
			assert(*i == cash_flow(1, 2));
			++i;
			assert(i);
			assert(*i == cash_flow(2, 3));
			++i;
			assert(i);
			assert(*i == cash_flow(3, 4));
			++i;
			assert(!i);
		}
		{
			auto z = value({ 1 }, { 2 });
			assert(z);
			assert(*z == cash_flow(1, 2));
			++z;
			assert(!z);
		}
		{
			auto v = value({ 1, 2 }, { 3, 4 });
			assert(v);
			auto w = value(v);
			assert(*w == cash_flow(1, 3));
			++w;
			assert(w);
			assert(*w == cash_flow(2, 4));
			++w;
			assert(!w);
		}

		return 0;
	}
#endif // _DEBUG

	// TODO: forward_rate_agreement
}
