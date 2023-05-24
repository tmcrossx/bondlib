# BondLib

Bond pricing and analytics

## Date

Using `ymd = std::chrono::year_month_day`.  
Implement day count fractions: 30/360, Actual/360, ...  

## Curve

Piecewise-flat forward curve.

`pwflat curve(span<T>{}, span<F>, _f)` is constant curve _f

## Fixed Income

times, cash flows: (u_j, c_j)

## Value

present_value(m, u, c, discount)

yield(ymd pvdate, double price, bond): price = present_value

## Bond

compounded_yield(unsigned n, double rate): (1 + yield/n)^n = exp(rate)

bond: maturity (years?), coupon, frequency (= 2), day count basis (= 30/360)

call/put schedule: time, amount

present_value(ymd, bond, discount)

