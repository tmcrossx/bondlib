// tmx_curve.h - Interest rate curve
#pragma once

namespace tmx {

	// NVI base class for bootstrapped curves.
	template<class T = double, class F = double>
	struct curve {
		virtual ~curve() = default;

		// Last point on the curve.
		std::pair<T, F> back() const
		{
			return _back();
		}

		// f_t(u) is the forward at u seen at time t
		F forward(T u, T t = 0) const
		{
			return _forward(u, t);
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
			return _yield(u, t); // !!!defined in terms of discount
		}

		// parallel shift by s
		curve& shift(F s)
		{
			return _shift(s);
		}

		// set value to use for bootstrap.
		curve& extrapolate(F _f)
		{
			return _extrapolate(_f);
		}
	private:
		virtual	std::pair<T, F> _back() const = 0;
		virtual F _forward(T u, T t) const = 0;
		virtual F _discount(T u, T t) const = 0;
		virtual F _yield(T u, T t) const = 0;
		virtual curve& _shift(F s) = 0;
		virtual curve& _extrapolate(F _f) = 0;
	};

	// Constant curve.
	template<class T = double, class F = double>
	class curve_constant : public curve<T,F> {
		F _f;
	public:
		constexpr curve_constant(F f)
			: _f(f)
		{ }
		constexpr curve_constant(const curve_constant&) = default;
		constexpr curve_constant& operator=(const curve_constant&) = default;
		constexpr ~curve_constant() = default;
		
		std::pair<T, F> _back() const override
		{
			return std::make_pair(std::numeric_limits<T>::infinity(), _f);
		}
		F _forward([[maybe_unused]] T u, [[maybe_unused]] T t) const override
		{
			return _f;
		}
		F _discount(T u, T t) const override
		{
			return exp(-(u - t)*_f);
		}
		F _yield([[maybe_unused]] T u, [[maybe_unused]] T t) const override
		{
			return _f;
		}
		curve<T,F>& _shift(F s) override
		{
			_f += s;

			return *this;
		}
		curve<T,F>& _extrapolate(F f) override
		{
			_f = f;

			return *this;
		}
	};
}