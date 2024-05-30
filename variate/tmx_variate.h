// tmx_variate.h - Random variates.
#pragma once

namespace tmx::variate {

	// Standard variate with mean 0 and variance 1.
	template<class X = double, class S = double>
	struct interface {
		virtual ~interface()
		{ }

		// Share density function P_s(X = x) dx.
		X pdf(X x, S s = S(0)) const
		{
			return _pdf(x, s);
		}
		// Share distribution function P_s(X <= x).
		X cdf(X x, S s = S(0)) const
		{
			return _cdf(x, s);
		}
		// Cumulant generating function log E[exp(sX)].
		S cgf(S s) const
		{
			return _cgf(s);
		}
		// Moment generating function E[exp(sX)].
		S mgf(S s) const
		{
			return _mgf(s);
		}
	private:
		virtual X _pdf(X x, S s) const = 0;
		virtual X _cdf(X x, S s) const = 0;
		virtual S _cgf(S s) const = 0;
		virtual S _mgf(S s) const = 0;
	};

} // namespace tms::variate
