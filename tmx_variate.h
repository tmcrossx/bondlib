// tmx_variate.h - Random variates.
#pragma once

namespace tms::variate {

	// Standard variate with mean 0 and variance 1.
	template<class X = double, class S = double>
	struct base {
		virtual ~base()
		{ }

		// Share density function P_s(X = x) dx.
		X pdf(const X& x, const S&s = S(0)) const
		{
			_pdf(x, s);
		}
		// Share distribution function P_s(X <= x).
		X cdf(const X& x, const S& s = S(0)) const
		{
			_cdf(x, s);
		}
		// Cumulant generating function log E[e^{sX}].
		S cgf(const S& s) const
		{
			return _cgf(s);
		}
		// Moment generating function E[e^{sX}].
		S mgf(const S& s) const
		{
			return _cgf(s);
		}
	private:
		virtual X _pdf(const X& x, const S& s) const = 0;
		virtual X _cdf(const X& x, const S& s) const = 0;
		virtual S _cgf(const S& s) const = 0;
		virtual S _mgf(const S& s) const = 0;
	};


} // namespace tms::variate
