// tmx_dual.h - Dual numbers f(x0 + x1 ε) = f(x0) + f'(x0) x1 ε
#pragma once

namespace tmx::math {

	template<class X>
	struct dual {
		X _[2]; // x[0] + x[1]ε
		constexpr dual(X x) 
			: _{ x, 0 }
		{ }
		constexpr dual(X x0, X x1) 
			: _{ x0, x1 } 
		{ }
		constexpr dual(const dual& x) = default;
		constexpr dual& operator=(const dual& x) = default;
		constexpr ~dual() = default;

		constexpr bool operator==(const dual& x) const noexcept
		{
			return _[0] == x._[0] && _[1] == x._[1];
		}	

		constexpr dual& operator+=(const dual& x)
		{
			_[0] += x._[0];
			_[1] += x._[1];
		
			return *this;
		}
		constexpr dual& operator-=(const dual& x)
		{
			_[0] -= x._[0];
			_[1] -= x._[1];

			return *this;
		}
		constexpr dual& operator-()
		{
			_[0] = -_[0];
			_[1] = -_[1];

			return *this;
		}
		constexpr dual& operator*=(const dual& x)
		{
			_[1] = _[0] * x._[1] + _[1] * x._[0];
			_[0] = _[0] * x._[0];

			return *this;
		}
		constexpr dual& operator/=(const dual& x)
		{
			_[1] = (_[1] * x._[0] - _[0] * x._[1]) / (x._[0] * x._[0]);
			_[0] = _[0] / x._[0];

			return *this;
		}
	};
	static_assert(dual{ 1, 2 } == dual{ 1, 2 });
	static_assert(dual{ 1, 2 } != dual{ 2, 3 });

} // namespace tmx::math

template<class X>
constexpr tmx::math::dual<X> operator+(const tmx::math::dual<X>& x, const tmx::math::dual<X>& y)
{
	return tmx::math::dual<X>(x) += y;
}
template<class X>
constexpr tmx::math::dual<X> operator-(const tmx::math::dual<X>& x, const tmx::math::dual<X>& y)
{
	return tmx::math::dual<X>(x) -= y;
}
template<class X>
constexpr tmx::math::dual<X> operator-(const tmx::math::dual<X>& x)
{
	return -tmx::math::dual<X>(x);
}
template<class X>
constexpr tmx::math::dual<X> operator*(const tmx::math::dual<X>& x, const tmx::math::dual<X>& y)
{
	return tmx::math::dual<X>(x) *= y;
}
template<class X>
constexpr tmx::math::dual<X> operator/(const tmx::math::dual<X>& x, const tmx::math::dual<X>& y)
{
	return tmx::math::dual<X>(x) /= y;
}

// f(x0 + x1 ε) = f(x0) + f'(x0) x1 ε
template<class F, class dF, class X>
constexpr auto make_dual(F&& f, dF&& df)
{
	return [f, df](const tmx::math::dual<X>& x) { return tmx::math::dual{ f(x._[0], df(x._[0]) * x._[1]) }; };
}

static_assert(tmx::math::dual{ 1, 2 } + tmx::math::dual{ 3, 4 } == tmx::math::dual{ 4, 6 });
static_assert(tmx::math::dual{ 1, 2 } - tmx::math::dual{ 3, 4 } == tmx::math::dual{ -2, -2 });
static_assert(-tmx::math::dual{ 1, 2 } == tmx::math::dual{ -1, -2 });
static_assert(tmx::math::dual{ 1, 2 } * tmx::math::dual{ 3, 4 } == tmx::math::dual{ 3, 10 });
static_assert(tmx::math::dual{ 3, 4 } / tmx::math::dual{ 1, 2 } == tmx::math::dual{ 3, -2 });

#ifdef _DEBUG
inline int test_math_dual()
{
	using namespace tmx::math;

	{
		// dx^2/dx = 2x
		constexpr dual d = [](auto x) { return x * x; }(dual{ 2, 1 });
		static_assert(d._[0] == 4);
		static_assert(d._[1] == 4);
	}
	{
		// dx^3/dx = 3x^2
		constexpr dual d = [](auto x) { return x * x * x; }(dual{ 2, 1 });
		static_assert(d._[0] == 8);
		static_assert(d._[1] == 12);
	}

	return 0;
}
#endif // _DEBUG
