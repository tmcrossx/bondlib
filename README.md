# BondLib

Bond pricing and analytics

## Date and Time

Every analytics library needs to convert real world date and time
to a floating point number representing time in years, and back again. 
A _clock_ produces `time_points` that correspond to a date and a time.
Every clock uses a signed arithmetic type and an _epoch_
that is represented by the value 0. 
Time points have a `time_since_epoch` member function for the clock
that returns the number of ticks since the epoch.

The invariant is `(date + years) + (-years) = date`.

Date and time is represented by a [`time_point`](https://en.cppreference.com/w/cpp/chrono/time_point).
A _clock_ produces time points 
Differences of dates is a [`duration`](https://en.cppreference.com/w/cpp/chrono/duration)

The `<chrono>` library has two way of representing time points.
The simplest way is a _serial-based_. This is a count of some unit of time
using a _period_. There are also _field-based_ time points represented
with human friendly fields.

Implement day count fractions using `year_month_day`: 30/360, Actual/360, ...  

## Curve 

Discount $D(t) = \exp(-\int_0^t f(s) ds)$.  
Forward discount $D_t(u) = D(u)/D(t) = \exp(-\int_t^u f_t(s) ds)$ if 0 vol.

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

Single call at date and price. Use time-dependent Ho-Lee with constant volatility. 

$f_t = r(t) + \sigma B_t$.  
$f(t) = r(t) - \sigma^2 t/2$.  
$f_t(u) =  r(u - t) - \sigma^2 (u - t)^2/2$.  

## Municipal Bond Curve

Municipal bonds are quoted using 5% coupon 10-year non-call par coupons.  
[EMMA](https://emma.msrb.org/ToolsAndResources/ICEYieldCurve?daily=False)
provides daily quotes at maturities for 1 to 30 years.

Bootstrap  
