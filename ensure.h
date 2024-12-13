// ensure.h - Report message using souce location.
#pragma once
#include <stdexcept>
#include <source_location>
#include <string_view>

#define ENSURE_HASH(x) #x
#define ENSURE_STRZ(x) ENSURE_HASH(x)
#define ENSURE(e) if (!(e)) { throw std::runtime_error(report(std::source_location::current(), ENSURE_STRZ(e))); }

namespace {

	// no constexpr int to string in standard library
	constexpr std::string to_string(int i)
	{
		std::string str;

		while (i) {
			str.insert(str.begin(), '0' + i % 10);
			i /= 10;
		}

		return str;
	}

	constexpr std::string report(const std::source_location& loc, const std::string_view& message)
	{
		std::string msg("fail: \"");
		msg.append(message).append("\"\n");
		msg.append("file: ").append(loc.file_name()).append("\n");
		msg.append("line: ").append(to_string(loc.line())).append("\n");
		msg.append("func: ").append(loc.function_name());

		return msg;
	}

} // namespace tmx
