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

inline void validate_noErrors(const asmparser& parser)
{
	BOOST_CHECK_EQUAL(0, parser.getErrors().get().size());
}

inline void validate_errorsCount(const asmparser& parser, size_t expectedCount)
{
	BOOST_CHECK_EQUAL(expectedCount, parser.getErrors().get().size());
}

inline void validate_labelsCount(const asmparser& parser, size_t expectedCount)
{
	BOOST_CHECK_EQUAL(expectedCount, parser.getLabels().size());
}

inline const std::vector<uint16_t>& validate_codeSize(const asmparser& parser, size_t expectedSize)
{
	const std::vector<uint16_t>& code = parser.getCodeBlocks()[0].code;
	BOOST_CHECK_EQUAL(expectedSize, code.size());
	return code;
}
