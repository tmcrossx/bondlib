# BondLib

Bond pricing and analytics

## Date and Time

See Howard Hinnant's [date library](https://howardhinnant.github.io/date/date.html)
for documentation of the [`<chrono>`](https://en.cppreference.com/w/cpp/chrono)
date and time library. This library uses 
[`std::system_clock`](https://en.cppreference.com/w/cpp/chrono/system_clock)
which represents the system-wide real time wall clock for time points.

Every analytics library needs to convert real world date and time
to a floating point number representing time in years and back again. 

We use the duration `std::chrono::years` to define `tmx::date::dpy` as the number of days per year.

The function `date::add_years(tp, years)` returns a time point with
years rounded to seconds and `date::sub_years(tp1, tp0)` returns `tp1 - tp0` as the number of years
rounded to seconds between two time points.
The invariants are `date_add(date_add(tp, years), -years))` is within one second of `tp`
and `date_sub(date_add(tp, years), tp)` is less than `1/(dpy * 86400)`.

Time points can be either _serial based_ or _field based_. 
All day count fractions use the field based 
[`std::chrono::year_month_day`](https://en.cppreference.com/w/cpp/chrono/year_month_day)
with resolution to one day.

## Discount 

Let $f_t$ to be the _(continuously compounded) forward rate_ at time $t$.
The _stochastic discount_ is $D_t = \int_0^t f_s\,ds$.
The _forward curve_ $f(t)$ is defined by $D(t) = E[D_t] = \int_0^t f(s)\,ds$.

The price at $t$ of a zero coupon bond maturing at $u$ is 
$D_t(u) = E_t[D_u]/D_t = E_t[\exp(-\int_t^u f_t(s) ds)]$,
where $f_t(u)$ is the forward curve at time $t$. Note $f(t) = f_0(t)$.

The _forward yield_ at $t$ defined by $D_t(u) = \exp(-(u - t)y_t(u)$.

The implementation uses [piecewise flat](tmx_pwflat.h) forwards.

## Fixed Income

Fixed cash flows $(u_j, c_j)$ where $u_j$ is the time in years
of the $j$th cash flow and $c_j$ is the amount.

## Value

Present value at $t$ is $\sum_{u_j > t} c_j D_t(u_j)$.

Duration at $t$ is the derivative with respect to a parallel shift 
in the forward curve $-\sum_{u_j > t} u_j c_j D_t(u_j)$.

Convexity at $t$ is the second derivative with respect to a parallel shift 
in the forward curve $\sum_{u_j > t} u_j^2 c_j D_t(u_j)$.

Yield is the constant rate that reprices a bond $p(t) = \sum_{u_j > t} c_j \exp(-y(t) (u_j - t))$.

## Bond

A _simple bond_ has indicative data maturity, coupon, frequency (default 2 times per year), and a
day count basis (default 30/360).
The function `tmx::instrument` returns the bond cash flows given the _dated date_.

compounding $(1 + y/n)^n = \exp(f)$

Single call at date and price. Use time-dependent Ho-Lee with constant volatility. 

## Municipal Bond Curve

Municipal bonds are quoted using 5% coupon 10-year non-call par coupons.  
[EMMA](https://emma.msrb.org/ToolsAndResources/ICEYieldCurve?daily=False)
provides daily quotes at maturities for 1 to 30 years.

Bootstrap  
