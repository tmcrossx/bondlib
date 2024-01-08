# BondLib

Bond pricing and analytics

## Date and Time

Every analytics library needs to convert a real world date and time
to a floating point number representing time in years and back again. 

We use the C++ [`<chrono>`](https://en.cppreference.com/w/cpp/chrono) library for that.
The solution is not simple.
A _clock_ consists of a starting point (or epoch) and a tick rate.
In the the old days people used
an unsigned integer [`time_t`](https://en.cppreference.com/w/c/chrono/time_t) for this.
Its epoch is 1970-01-01 00:00:00 UTC with a tick rate of 1 second.

C++ makes several clocks available with finer resolution.
We use `std::system_clock` as the default clock. 
That usually has a tick rate of a microsecond or better.
A [`time point`](https://en.cppreference.com/w/cpp/chrono/time_point)
is a duration of time that has passed since the epoch of a specific clock.
A [`duration`](https://en.cppreference.com/w/cpp/chrono/duration)
consists of a span of time, defined as some number of ticks of some time unit.

Every clock can be converted to a time point. The difference between two time points
is a duration. 


See Howard Hinnant's [documentation](https://howardhinnant.github.io/date/date.html)
for all the gory details.



date and time library. This library uses 
[`std::system_clock`](https://en.cppreference.com/w/cpp/chrono/system_clock)
which represents the system-wide real time wall clock for time points.

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

$\Omega$, $\mathcal{A}_t$, $t\in T$, is a partition of $\Omega$, 
$X\colon\mathcal{A}_t\to\bm{R}$ is a measurable function,
$D\colon\mathcal{A}_t\to\bm{R}$ is a measure.

`pairing(X, D, A_t)` is $\int_\Omega X\,dD$

E[max{k - f exp(s X - \kappa(s)), 0}]






