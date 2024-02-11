// fms_iterable.h - iterable_iterator with operator bool()
#pragma once
#ifdef _DEBUG
#include <cassert>
#include <vector>
#endif
#include <concepts>
#include <iterator>
#include <span>

namespace fms::iterable {

	template<class I, class T>
	concept input = std::input_iterator<I> && requires(I i) {
		{ i.operator bool() } -> std::same_as<bool>;
		{ *i } -> std::convertible_to<T>;
		{ ++i } -> std::same_as<I&>;
		{ i++ } -> std::same_as<I>;
	};

} // namespace fms
