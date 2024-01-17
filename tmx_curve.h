// tmx_curve.h - Interest rate curve interface.
#pragma once
#include <utility>

namespace tmx::curve {

	// NVI base class for bootstrapped curves.
	template<class T = double, class F = double>
	struct base {
		virtual ~base()
		{ }

		// Last point on the curve.
		std::pair<T, F> back() const
		{
			return _back();
		}

		// f_t(u) = f(u + t) is the forward at u seen at time t
		F forward(T u, T t = 0) const
		{
			return _forward(u + t);
		}
		F operator()(T u, T t = 0) const
		{
			return forward(u, t);
		}

		// D_t(u) is the price at time t of a zero coupon bond maturing at time u.
		// D_t(u) = exp(-int_t^u f(s) ds).
		F discount(T u, T t = 0) const
		{
			return _discount(u, t);
		}

		// y_t(u) is the yield at time t over the interval [t, u].
		// D_t(u) = exp(-(u - t) y_t(u)).
		F yield(T u, T t = 0) const
		{
			return _yield(u, t);
		}

		// parallel shift by s
		base& shift(F s)
		{
			return _shift(s);
		}

		// set value to use for bootstrap.
		base& extrapolate(F _f)
		{
			return _extrapolate(_f);
		}
	private:
		virtual	std::pair<T, F> _back() const = 0;
		virtual F _forward(T u) const = 0;
		virtual F _discount(T u, T t) const = 0;
		virtual F _yield(T u, T t) const = 0;
		virtual base& _shift(F s) = 0;
		virtual base& _extrapolate(F _f) = 0;
	};

	// Constant curve.
	template<class T = double, class F = double>
	class constant : public base<T,F> {
		F _f;
	public:
		constexpr constant(F f = std::numeric_limits<F>::quiet_NaN())
			: _f(f)
		{ }
		constexpr constant(const constant&) = default;
		constexpr constant& operator=(const constant&) = default;
		constexpr ~constant() = default;
		
		std::pair<T, F> _back() const override
		{
			return std::make_pair(std::numeric_limits<T>::infinity(), _f);
		}
		F _forward([[maybe_unused]] T u) const override
		{
			return _f;
		}
		F _discount(T u, T t) const override
		{
			return std::exp(-(u - t)*_f);
		}
		F _yield([[maybe_unused]] T u, [[maybe_unused]] T t) const override
		{
			return _f;
		}
		base<T,F>& _shift(F s) override
		{
			_f += s;

			return *this;
		}
		base<T,F>& _extrapolate(F f) override
		{
			_f = f;

			return *this;
		}
	};
}