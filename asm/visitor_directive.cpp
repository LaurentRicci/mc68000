#include "visitor.h"

using namespace std;
using namespace mc68000;

any visitor::visitLine_directiveSection(parser68000::Line_directiveSectionContext* ctx)
{
	size = 1;
	any result = ctx->children[1]->accept(this);
	return result;
}

any visitor::visitDc(parser68000::DcContext* ctx)
{
	size = any_cast<uint16_t>(visit(ctx->children[1]));
	visit(ctx->children[2]);
	if (size == 0)
	{
		size_t start = 0;
		if (incompleteBinary)
		{
			// complete the last word with the 1st byte of the dcBytes
			code.back() = code.back() | dcBytes[0];
			start = 1;
			incompleteBinary = false;
		}
		// move the binary data to the code
		for (size_t i = start; i < dcBytes.size(); i += 2)
		{
			uint16_t word = dcBytes[i];
			if (i + 1 < dcBytes.size())
			{
				word = (word << 8) | dcBytes[i + 1];
			}
			else
			{
				word = (word << 8);
				incompleteBinary = true;
			}
			code.push_back(word);
		}
		dcBytes.clear();
		return *code.rbegin();
	}
	return *code.rbegin();
}

any visitor::visitDataList(parser68000::DataListContext* ctx)
{
	size_t n = ctx->children.size();
	switch (size)
	{
	case 0: // byte
		for (size_t i = 0; i < n; i += 2)
		{
			any data = visit(ctx->children[i]);
			uint32_t target;
			if (data.type() == typeid(std::string))
			{
				string chars = any_cast<string>(data);
				string::iterator start = chars.begin() + 1;
				string::iterator end = chars.end() - 1;

				dcBytes.insert(dcBytes.end(), start, end);
			}
			else
			{
				uint8_t byte = static_cast<uint8_t>(any_cast<int>(data));
				dcBytes.push_back(byte);
			}
		}
		return dcBytes.size();
		break;
	case 1: // word
		for (size_t i = 0; i < n; i += 2)
		{
			any data = visit(ctx->children[i]);
			uint32_t target;
			if (data.type() == typeid(std::string))
			{
				string chars = any_cast<string>(data);
				// The quotes are included in the string so we skip them
				for (size_t i = 1; i < chars.size()-1; i += 2)
				{
					uint16_t word = chars[i] & 0xff;
					if (i + 1 < chars.size()-1)
					{
						word = (word << 8) | (chars[i + 1] & 0xff);
					}
					else
					{
						word = (word << 8);
					}
					code.push_back(word);
				}
			}
			else
			{
				uint16_t number = static_cast<uint16_t>(any_cast<int>(data));
				code.push_back(number);
			}
		}
		return 0;
		break;
	case 2: // long
		for (size_t i = 0; i < n; i += 2)
		{
			any data = visit(ctx->children[i]);
			uint32_t target;
			if (data.type() == typeid(std::string))
			{
				string chars = any_cast<string>(data);
				size_t begin = 1;
				size_t end = chars.size() - 1;
				// The quotes are included in the string so we skip them
				for (size_t i = begin; i < end; i += 4)
				{
					uint32_t codelong = chars[i] & 0xff;
					for(size_t j = 1; j < 4; j++)
					{
						if (i + j < end)
						{
							codelong = (codelong << 8) | (chars[i + j] & 0xff);
						}
						else
						{
							codelong = (codelong << 8);
						}
					}
					code.push_back(codelong >> 16);
					code.push_back(codelong & 0xffff);
				}
			}
			else
			{
				uint32_t number = static_cast<uint32_t>(any_cast<int>(data));
				code.push_back(number >> 16);
				code.push_back(number & 0xffff);
			}
		}
		return 0;
		break;
	}
	return dcBytes.size();
}
