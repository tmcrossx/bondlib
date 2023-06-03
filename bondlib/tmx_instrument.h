// tmx_instrument.h - times and cash flows
#pragma once
#include <vector>

namespace tmx {

	template<class U = double, class C = double>
	struct instrument {
		virtual ~instrument()
		{ }

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
	private:
		virtual size_t _size() const = 0;
		virtual const U* _time() const = 0;
		virtual const C* _cash() const = 0;
	};

	// instrument value type
	template<class U = double, class C = double>
	class instrument_vector : public instrument<U, C> {
		std::vector<U> u;
		std::vector<C> c;
	public:
		instrument_vector()
		{ }
		instrument_vector(size_t m, const U* u, const C* c)
			: u(u, u + m), c(c, c + m)
		{ }
		~instrument_vector()
		{ }

		instrument_vector& push_back(U _u, C _c)
		{
			u.push_back(_u);
			c.push_back(_c);

			return *this;
		}

		size_t _size() const override
		{
			return u.size();
		}
		const U* _time() const override
		{
			return u.data();
		}
		const C* _cash() const override
		{
			return c.data();
		}
	};
};
