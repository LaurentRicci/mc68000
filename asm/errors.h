#pragma once
#include <string>
#include <vector>

namespace mc68000
{
	class error
	{
	public:
		// Constructor
		error(const std::string& message, int line, int column)
			: message(message), line(line), column(column)
		{
		}
	private:
		std::string message; // Error message
		int line;            // Line number where the error occurred
		int column;          // Column number where the error occurred
	};


	class errors
	{
	public:
		// Constructor
		errors() = default;
		// Add an error message
		void add(const std::string& message, int line, int column)
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