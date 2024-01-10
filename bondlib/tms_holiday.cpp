// tmx_holiday.cpp
#include "tmx_holiday.h"

using namespace tmx;
using namespace tmx::holiday;

bool new_year_day(const date::ymd& d)
{
	//name[std::string(__FUNCTION__)] = new_year_day;

	return month_day(d, std::chrono::January, std::chrono::day(1));
}
