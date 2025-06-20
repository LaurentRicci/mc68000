#pragma once
#include <any>
bool hasFailed(std::any result);

template<typename T>
void validate_hasValue(T expected, std::any result)
{
	auto results = std::any_cast<std::vector<std::any>>(result);
	std::any element = results[0];
	T actual = std::any_cast<T>(element);

	BOOST_CHECK_EQUAL(expected, actual);
}