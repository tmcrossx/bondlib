# BondLib

Bond pricing and analytics

## Date

Date is a [`time_point`](https://en.cppreference.com/w/cpp/chrono/time_point).

Difference of dates is a [`duration`](https://en.cppreference.com/w/cpp/chrono/duration)

Only need day resolution so use [`year_month_day`](https://en.cppreference.com/w/cpp/chrono/year_month_day)

`ymd add_years(ymd d, double y)` add years based on days per year.

`double dcf_years(ymd d0, ymd d1)` duration as double based on days per year.

`dcf_years(ymd d0, date_add(d0, y)) == y` within one day

Implement day count fractions: 30/360, Actual/360, ...  

## Curve 

Discount $D(t) = \exp(-\int_0^t f(s) ds)$, forward discount 
$D_t(u) = D(u)/D(t) = \exp(-\int_t^u f(s) ds)$ if 0 vol.

Use piecewise flat forwards for $f$.

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

call/put schedule: time, amount


