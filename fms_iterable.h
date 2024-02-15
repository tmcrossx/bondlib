// fms_iterable.h - iterable_iterator with operator bool()
#pragma once
#ifdef _DEBUG
#include <cassert>
#endif
#include <concepts>
#include <iterator>

namespace fms::iterable {

	template<class I, class T = typename I::value_type>
	concept input = std::forward_iterator<I> && requires(I i) {
		{ i.operator bool() } -> std::same_as<bool>;
	};

} // namespace fms::iterable
