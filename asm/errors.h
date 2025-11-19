#pragma once
#include <string>
#include <vector>
#include <tuple>

namespace mc68000
{
	class error
	{
	public:
		// Constructor
		error(const std::string& message, size_t line, size_t column)
			: message(message), line(line), column(column)
		{
		}
		std::string toString() const
		{
			return "Line " + std::to_string(line) + ", Column " + std::to_string(column) + ": " + message;
		}
		std::tuple<size_t, size_t> getPosition() const
		{
			return std::make_tuple(line, column);
		}
	private:
		std::string message;    // Error message
		size_t line;            // Line number where the error occurred
		size_t column;          // Column number where the error occurred
	};

	class errors
	{
	public:
		// Constructor
		errors() = default;
		// Add an error message
		void add(const std::string& message, size_t line, size_t column)
		{
			error_messages.push_back(error(message, line, column));
		}
		// Get all error messages
		const std::vector<error>& get() const
		{
			return error_messages;
		}
		// Clear all error messages
		void clear()
		{
			error_messages.clear();
		}
	private:
		std::vector<error> error_messages; // Vector to store error messages
	};
}