// tmx_instrument.h - times and cash flows
#pragma once
#include <algorithm>
#include <vector>
#include "ensure.h"

namespace tmx {

	// NVI base class for instruments
	template<class U = double, class C = double>
	struct instrument {
		virtual ~instrument()
		{ }

		bool operator==(const instrument& i) const
		{
			return size() == i.size()
				&& std::equal(cash(), cash() + size(), i.cash())
				&& std::equal(time(), time() + size(), i.time());
		}

		size_t size() const
		{
			return _size();
		}
		const U* time() const
		{
			return _time();
		}
		const C* cash() const
		{
			return _cash();
		}

		std::pair<U, C> back() const
		{
			size_t m = size() - 1;

			return { time()[m], cash()[m] };
		}
	private:
		virtual size_t _size() const = 0;
		virtual const U* _time() const = 0;
		virtual const C* _cash() const = 0;
	};

	// non-owning instrument view
	template<class U = double, class C = double>
	class instrument_view : public instrument<U, C> {
	protected:
		size_t m;
		const U* u;
		const C* c;
	public:
		instrument_view()
			: m{ 0 }, u{ nullptr }, c{ nullptr }
		{ }
		instrument_view(size_t m, const U* u, const C* c)
			: m{ m }, u{ u }, c{ c }
		{ }
		instrument_view(const instrument_view& v) = default;
		instrument_view& operator=(const instrument_view& v) = default;
		~instrument_view()
		{ }

		size_t _size() const override
		{
			return m;
		}
		const U* _time() const override
		{
			return u;
		}
		const C* _cash() const override
		{
			return c;
		}
	};

	// instrument value type
	template<class U = double, class C = double>
	class instrument_value : public instrument_view<U, C> {
		std::vector<U> u;
		std::vector<C> c;
		void update()
		{
			instrument_view<U, C>::m = u.size();
			instrument_view<U, C>::u = this->u.data();
			instrument_view<U, C>::c = this->c.data();
		}
	public:
		instrument_value()
		{ }
		instrument_value(size_t m, const U* u, const C* c)
			: u(u, u + m), c(c, c + m)
		{
			update();
		}
		instrument_value(const instrument_value& v) = default;
		instrument_value& operator=(const instrument_value& v) = default;
		instrument_value(instrument_value&& v) = default;
		instrument_value& operator=(instrument_value&& v) = default;
		~instrument_value()
		{ }

		// add cash flow keeping times sorted
		instrument_value& push_back(U _u, C _c)
		{
			if (u.size() > 0 and u.back() == _u) {
				c.back() += _c;
			}
			else {
				ensure(u.size() == 0 or u.back() < _u);

				u.push_back(_u);
				c.push_back(_c);
				update();
			}

			return *this;
		}

#ifdef _DEBUG
		static int test()
		{
			{
				instrument_value<U, C> i;
				assert(0 == i.size());
				instrument_value<U, C> i2{i};
				assert(i == i2);
				i = i2;
				assert(!(i != i2));

				i.push_back(1, 1);
				assert(1 == i.size());
				i.push_back(2, 2);
				assert(2 == i.size());
				U u[] = {1, 2};
				C c[] = {1, 2};
				instrument_value<U, C> i3(2, u, c);
				assert(i == i3);
			}

			return 0;
		}
#endif // _DEBUG
	};

}
