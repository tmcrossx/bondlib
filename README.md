---
title: Instrument Library
---

# bondlib

Fixed income instruments pay fixed cash flows $(c_j)$ at fixed times $(u_j)$.
The simplest fixed income instrument is a zero coupon bond that pays 1 unit at _maturity_ $u$.
The _price_ of a zero coupon bond $D(u)$ is the _discount_ to time $u$.

Discounts are _quoted_ using rates.
The _continuously compounded spot rate_ $r(t)$ is defined by $D(t) = \exp(-t r(t))$.
The _continuously compounded forward_ rate $f(t)$ is
defined by $D(t) = \exp(-\int_0^t f(s)\,ds)$.
This implies the spot $r(t) = (1/t)\int_0^t f(s)\,ds$
is the average forward rate over the interval $[0, t]$.
This also shows the forward $f(t) = r(t) + t r'(t)$.
We use a forward curve to implement spot and discount because
integration is numerically more stable than differentiation.

Market rates are quoted using _compounding frequencies_.
If rate $r_n$ is compounded $n$ times per year then 
$(1 + r_n/n)^n = \exp(r)$ so $r_n = n(\exp(r/n) - 1)$

The _continuously compounded yield_ of a fixed income security given a price $p$ 
is the constant $y$ with $p = \sum_j c_j \exp(-y u_j)$. 
The yield does not require a discount curve.

The _present value_ of a fixed income security is 
the sum of discounted future cash flows, $p = \sum_j c_j D(u_j)$.
The _duration_ of a fixed income security is the derivative of the present value 
with respect to a parallel shift in the forward curve. 
The _convexity_ is the second derivative.

## [Date](date/tmx_date.h)

A fundamental problem when implementing date and time is how to convert two
calendar dates to a time duration and a date plus a time duration back to a calendar date.
We must preserve $d_0 + (d_1 - d_0) = d_1$ for any dates $d_0$ and $d_1$. 
There is no canonical solution. We define the difference of two dates
to be the time in seconds between the two dates divided by the number of seconds in a year.
The number of seconds between two dates is well-defined but the number of seconds in a year is not.
We use `constexpr time_t seconds_per_year = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::years{ 1 }).count();`
from the C++ [`<chrono>`](https://en.cppreference.com/w/cpp/chrono) library.

The [`tmx::date`](date/tmx_date.h) namespace uses `tmx::date::ymd` as an alias
for [`std::chrono::year_month_day`](https://en.cppreference.com/w/cpp/chrono/year_month_day)
to represent calendar dates
and [`time_t`](https://en.cppreference.com/w/c/chrono/time_t) to represent time durations.
The function overloads `ymd operator+(ymd, time_t)` and `time_t operator-(ymd, ymd)`
satisfy $(d + t) - d = t$ and $d_0 + t = d_1$ when $t = d_1 - d_0$.
The functions `ymd addyears(ymd, double)` and `double diffyears(ymd, ymd)` are
implemented using these by converting a `time_t` to a `double` using `seconds_per_year`.

### [Day Count](date/tmx_date_day_count.h)

The file [`date/tmx_date_daycount.h`](date/tmx_date_daycount.h) implements the most common day count fraction
conventions. The unit tests are from the [Bloomberd BDE library](https://github.com/bloomberg/bde).

### [Holiday](date/tmx_date_holiday.h)

The file [`tmx_date_holiday.h`](date/tmx_date_holiday.h) implements tests for common holidays.
Instead of a database of holidays it uses a function that returns `true` if a date is a holiday.
This is the approach taken by the [QuantLib](https://www.quantlib.org/) library.

### [Holiday Calendar](date/tmx_date_holiday_calendar.h)

The file [`date/tmx_date_holiday_calendar.h`](date/tmx_date_holiday_calendar.h) implements holiday calendars.
It currently has only the SIFMA, NYSE, and FED calendars.

### [Business Day](date/tmx_date_business_day.h)

The file [`tmx_date_business_day.h`](date/tmx_date_business_day.h) implements the most common business day 
rolling conventions. Dates falling on holidays must be adjusted to a nearby business day.

## [Cash Flow](instrument/tmx_cash_flow.h)

A [`tmx::cashflow`](instrument/tmx_cash_flow.h) is a pair of time and amount.

## [Instrument](instrument/tmx_instrument.h)

A [`tmx::instrument::iterable`](instrument/tmx_instrument.h) is constructed
from a pair of time and amount iterables.
Its `value_type` is a [`tmx::cashflow`](instrument/tmx_cash_flow.h).

## [Curve](curve/tmx_curve.h)

The [`tmx::curve::interface`](curve/tmx_curve.h#:~:text=class%20interface) class provides an interface to
discount, forward, and spot. C++ does not have
a notion of interface but the 
[NVI idiom](https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Non-Virtual_Interface)
can be used to specify invariants for subclasses and help the compiler eliminate virtual function tables.

Subclasses must override the pure virtual `_forward` and `_integral` functions.
Spot and discount are implemented in terms of these.
The `tmx::curve::constant`, `tmx::curve::exponential`, `tmx::curve::bump`
and `tmx::curve::extrapolate` classes
are examples of how to do this.

The `tmx::curve::plus` class adds two curves. It uses const references to avoid copying
any data used in the implementation of the `curve::interface` class. 
The referenced data are required to outlive the `curve::plus` object.

### [Piecewise Flat](curve/tmx_pwflat.h)

[`tmx::curve::pwflat`](curve/tmx_curve_pwflat.h) implements `tmx::curve::interface`. 
It uses stand-alone functions from [tmx::pwflat](curve/tmx_pwflat.h).
A piecewise flat curve is determined by a set of times and rates, $(t_i, f_i)$,
and an _extrapolation_ value $\bar{f]$. The value of the curve is
undefined for $t < 0$, is $f_0$ for $0 \le t \le f_1$, $f_j$
for $t_{j-1} < t \le t_j$ and $\bar{f}$ for $t > t_n$.
Note $f(t_j) = f_j$.

## [Valuation](value/tmx_valuation.h)

The functions in the [`tmx::value`](value/tmx_valuation.h) namespace calculate 
fixed income analytics given an instrument and curve interface.

The functions [`compound_yield`](tmx_valuation.h#:~:text=compound_yield) 
and [`continuous_yield`](tmx_valuation.h#:~:text=continuous_yield) convert between
the continuously compounded yield used in the internal implementation
and the compounded yields quoted in the market.

There are also functions for computing the present value, duration, convexity, yield
and option adjusted spread of a fixed income security.

## [Bootstrap](bootstrap/tmx_bootstrap.h)

Given a set of fixed income instruments and their prices how can we find a discount curve
that reprices them? This is a underdetermined problem but the _bootstrap_ algorithm
provides a unique solution.

Order the instruments by maturity and find a yield that reprices the first instrument.
This determines the segment of the piecewise flat forward curve to the shortest maturity.
Keeping that fixed and find the extrapolated yield that reprices the second instrument.
Repeat until all instruments are repriced.

It is popular to use splines to get a smooth forward curve, but that introduces
mathematical artifacts that have nothing to do with market prices.
It is better to introduce interpolated _instruments_ with prices that
make sense to traders.

<!--
Let $T$ be a totally ordered set of trading times.

Let $\Omega$ be the set of all possible outcomes. 

A set of subsets of $\Omega$ is an _algebra_ if it is closed under complement and union.
For $\omega\in\Omega$ let $`\overline{\omega} = \cap\{A\in\mathcal{A}\mid\omega\in A\}`$ be the _atom_ containing $\omega$.
Let $`\overline{\mathcal{A}} = \{\overline{\omega}\mid \omega\in\Omega\}`$ be the set of atoms of $\mathcal{A}$.
The atoms of an algebra form a partition of $\Omega$ and represent partial information.

A function $X\colon\Omega\to\boldsymbol{R}$ is $\mathcal{A}$-_measurable_ 
if and only if it is constant on the atoms of $\mathcal{A}$.
We write this as $`X\colon\overline{\mathcal{A}}\to\boldsymbol{R}`$.

Let $`(\mathcal{A}_t)_{t\in T}`$, be finite algebras on $\Omega$ representing information available at time $t\in T$.

Let $I$ be the set of market instruments.

Let $X_t\colon\overline{\mathcal{A_t}}\to\boldsymbol{R}^I$ be the prices of instruments at time $t\in T$.

Let $C_t\colon\overline{\mathcal{A_t}}\to\boldsymbol{R}^I$ be the cash flows of instruments at time $t\in T$.

There is no arbitrage if there exist a positive measures $D_t$ on $\mathcal{A}_t$ such that

$$
X_t D_t = (X_u D_u + \sum_{t < s \le u} C_s D_s)|_{\mathcal{A}_t}, \quad t < u. \label{eq1}
$$

A _repurchase agreement_, $f_t$, is an instrument with price $1$ at time $t$ and cash flow $\exp(f_t\,dt)$ at time $t+dt$.

The _canonical deflator_ is $D_t = \exp(-\int_0^t f_s\,ds)P$ where $P$ is a probability measure on $\Omega$.

A _stopping time_ is a random variable $\tau:\Omega\to T$ such that $`\{\tau \le t\} \in \mathcal{A}_t`$ for all $t\in T$.
The algebra $`\mathcal{A}_\tau = \{A\subseteq\Omega\mid A\cap\{\tau \le t\}\in\mathcal{A}_t, t\in T\}`$.
If trading times and algebras are finite then $\mathcal{A}_\tau$ is the partition
$`\{\{\tau = t\}\mid t\in T\}`$.

A _trading strategy_ is a finite sequence $(\tau_j, \Gamma_j)$ of strictly increasing stopping times $\tau_j$
and trades $\Gamma_j\colon\mathcal{A}_{\tau_j}\to(\boldsymbol{R}^I)^*$.
The _position_ at time $t$ is $`\Delta_t = \sum_{\tau_j < t} \Gamma_j = \sum_{s < t} \Gamma_s`$
where $`\Gamma_s(\omega) = \sum_j \Gamma_j(\omega)1(\tau_j(\omega) = s)`$.

The _value_, or _mark-to-market_, of a trading strategy at time $t$ is $V_t = (\Delta_t + \Gamma_t)\cdot X_t$.

The _amount_ $`A_t = \Delta_t\cdot C_t - \Gamma_t\cdot X_t`$ shows up in the trading account at time $t$.

These definitions imply

$$
V_t D_t = (V_u D_u + \sum_{t < s \le u} A_s D_s)|_{\mathcal{A}_t}, \quad t < u. \label{eq2}
$$

An _instrument_ is a sequence $(\upsilon_j, c_j)$ of strictly increasing stopping times $\upsilon_j$ and payments 
$`c_j\colon\mathcal{A}_{\upsilon_j}\to\boldsymbol{R}`$.

## Bond pricing and analytics.

## Forward values

The forward over the interval $[t, u]$ i

## Datetime

Every analytics library needs to convert a real world date and time
to a floating point number representing time in years and back again. 

A _clock_ has a starting point (or epoch) and a tick rate.
We use the _time point_ [`time_t`](https://en.cppreference.com/w/c/chrono/time_t) for this,
an unsigned integer with epoch midnight January 1, 1970 
[UTC](https://www.ipses.com/eng/in-depth-analysis/standard-of-time-definition/)
with a tick rate of 1 second.

A _duration_ is the difference of clock time points.
the standard library function 
[`difftime`](https://en.cppreference.com/w/c/chrono/difftime)
returns the difference in seconds between two time points as a `double`.
We implement the function `datetime::diffyears` returning the difference
in years by dividing by the number of seconds in a year.
We use the duration `std::chrono::years` to define `datetime::days_per_year`
as the number of days per year. Every day has 86400 = 24 * 60 * 60 seconds
so `datetime::seconds_per_year = 86400 * datetime::days_per_year`.

The function `time_t datetime::add_year(time_t t, double y)` adds `y` years to `t`.
The invariants are `diffyears(add_years(t, y), t) == y`
and `add_years(t0, diffyears(t1, t0) == t1`.

## Date

The type `date::ymd` represents a calendar date with resolution to one day.
Converting a `time_t` to a `date::ymd` depends on the time zone specified
in the `TZ` environment variable. 

// TODO: 
ymd -> (y, m d) -> ymd
add/sub months, days

The functions `ymd date::add_months(ymd d, int m)` and
`ymd date::add_days(ymd d, int d)` add months and days to a date.
The functions `ymd date::add_months(ymd d, int m)` and
`ymd date::add_days(ymd d, int d)` add months and days to a date.

## Discount 

Let $f_t$ to be the _(continuously compounded) forward rate_ at time $t$.
The _stochastic discount_ is $D_t = \int_0^t f_s\,ds$.
The _forward curve_ $f(t)$ is defined by $D(t) = E[D_t] = \int_0^t f(s)\,ds$.

The price at $t$ of a zero coupon bond maturing at $u$ is 
$D_t(u) = E_t[D_u]/D_t = E_t[\exp(-\int_t^u f_t(s) ds)]$,
where $f_t(u)$ is the forward curve at time $t$. Note $f(t) = f_0(t)$.

The _forward yield_ at $t$ defined by $D_t(u) = \exp(-(u - t)y_t(u)$.

The implementation uses [piecewise flat](tmx_curve_pwflat.h) forwards.

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
To construct a forward curve we must determine the option values past 10 years.

Bootstrap  


-->
