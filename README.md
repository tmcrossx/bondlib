# BondLib

Bond pricing and analytics

## Date

Date is a [`time_point`](https://en.cppreference.com/w/cpp/chrono/time_point).

Difference of dates is a [`duration`](https://en.cppreference.com/w/cpp/chrono/duration)

Only need day resolution so use [`year_month_day`](https://en.cppreference.com/w/cpp/chrono/year_month_day)

sub_years(add_years(d, t), d) = t

add_years(d0, sub_years(d1, d0)) = d1

Implement day count fractions: 30/360, Actual/360, ...  

## Curve 

Discount $D(t) = \exp(-\int_0^t f(s) ds)$, forward discount 
$D_t(u) = D(u)/D(t) = \exp(-\int_t^u f(s) ds)$ if 0 vol.

Use piecewise flat forwards for $f$.

shift rates

translate times

use internal offset to minimize copying

## Fixed Income

Convert from dates to years from some effective/dated date.

Fixed cash flows $(u_j, c_j)$.

## Value

Present value at $t$ is $\sum_{u_j > t} c_j D_t(u_j)$.

Duration at $t$ is the derivative with respect to a parallel shift 
in the forward curve $-\sum_{u_j > t} u_j c_j D_t(u_j)$.

Yield is the constant rate that reprices a bond $p = \sum_j c_j \exp(-y u_j)$.

## Bond

compounding $(1 + y/n)^n = \exp(f)$

bond: dated, maturity, coupon, frequency (= 2), day count basis (= 30/360)

Single call at date and price. Use time-dependent Ho-Lee with constant volatility. $f_t = r(t) (1 + sigma B_t)$.

## Municipal Bond Curve

Municipal bonds are quoted using 5% coupon 10-year non-call par coupons.  
[EMMA](https://emma.msrb.org/ToolsAndResources/ICEYieldCurve?daily=False)
provides daily quotes at maturities for 1 to 30 years.

Bootstrap constant annual forwards. 
