# BondLib

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
To construct a forward curve we must determine the option values past 10 years.

Bootstrap  


