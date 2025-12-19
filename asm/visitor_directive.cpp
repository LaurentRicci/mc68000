#include "visitor.h"

using namespace std;
using namespace mc68000;

any visitor::visitLine_directiveSection(parser68000::Line_directiveSectionContext* ctx)
{
    listing.startLine();

	size = 1;
	labelCtx = ctx->labelSection();
	any result = visit(ctx->directiveSection());

    listing.endLine();

	return result;
}

any visitor::visitDc(parser68000::DcContext* ctx)
{
	visitLabelSection(labelCtx);

	size = any_cast<uint16_t>(visit(ctx->size()));
	visit(ctx->dataList());
	if (size == 0)
	{
		size_t start = 0;
		if (incompleteBinary)
		{
			// complete the last word with the 1st byte of the dcBytes
			code().back() = code().back() | dcBytes[0];
			currentAddress += 1;
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
				currentAddress += 2;
			}
			else
			{
				word = (word << 8);
				incompleteBinary = true;
				currentAddress += 1;
			}
			code().push_back(word);
		}
		dcBytes.clear();
		return code().back();
	}
	return code().back();
}
any visitor::visitDirectiveSection(parser68000::DirectiveSectionContext* ctx)
{
    listing.setAddress(currentAddress);
    listing.setInstruction(listing.sourceText(ctx));

    return parser68000BaseVisitor::visitDirectiveSection(ctx);
}

any visitor::visitEqu(parser68000::EquContext* ctx)
{
	if (labelCtx->value.empty())
	{
		addError("Missing label in EQU directive", ctx);
        return any();
	}
    listing.setLabel(labelCtx->value);
	any value = visit(ctx->expression());
	if (pass == 0)
	{
		if (labels.find(labelCtx->value) != labels.end())
		{
			addPass0Error("Symbol name conflicts with label: " + labelCtx->value, ctx);
            return any();
        }
		if (symbols.find(labelCtx->value) != symbols.end())
		{
            addPass0Error("Duplicate symbol: " + labelCtx->value, ctx);
            return any();
        }
	}
    symbols[labelCtx->value] = value;
    if (value.type() == typeid(int))
    {
        uint32_t val = any_cast<int>(value);
        uint16_t opcode = val >> 16;
        vector<uint16_t> extensions{ uint16_t(val & 0xffff) };
        listing.setBinary(opcode, extensions);
    }

	return any();
}
any visitor::visitDs(parser68000::DsContext* ctx)
{
	if (ctx->size())
	{
		size = any_cast<uint16_t>(visit(ctx->size()));
	}
	uint32_t count = getAddressValue(ctx->address());
	if (count <= 0)
	{
		addError("Storage size should be a positive integer:" + to_string(count), ctx->address());
		count = 1;
	}
	uint32_t bytesNeeded = count * (1 << size);

	if (incompleteBinary)
	{
		if (size == 0)
		{
			// byte storage starts at current location
			visitLabelSection(labelCtx);

			// use one byte to complete the last word
			currentAddress += 1;
			bytesNeeded -= 1;
		}
		else
		{
			// storage must start on a word boundary
			currentAddress += 1;
			visitLabelSection(labelCtx);
		}
		incompleteBinary = false;
	}
	else
	{
		// already on a word boundary
		visitLabelSection(labelCtx);
	}
	for (uint32_t i = 0; i < bytesNeeded; i += 2)
	{
		code().push_back(0);
	}
	if ((bytesNeeded) % 2 != 0)
	{
		incompleteBinary = true;
	}
	currentAddress += bytesNeeded;
	return *code().rbegin();
}

any visitor::visitOrg(parser68000::OrgContext* ctx)
{
	auto blockAddress = ctx->blockAddress();
	uint32_t address = getAddressValue(blockAddress);
	if (pass == 0)
	{
		if (currentAddress == 0)
		{
			// We are at the begining of the first block
			assert(codeBlocks.size() == 1);
			codeBlocks.back().origin = address;
			start = address;
			currentAddress = address;
		}
		else
		{
			// We start a new block
			codeBlock newBlock(address);
			codeBlocks.push_back(newBlock);
		}
    	return any();
	}
    if (memoryStart != 0 || memoryEnd != 0)
    {
        // Validate that the origin is within the addressable memory
        if (address < memoryStart || address > memoryEnd)
        {
            addError("ORG " + to_string(address) + " should be inside addressable space: " + to_string(memoryStart) + " - " + to_string(memoryEnd), ctx);
        }
    }
    return any();
}
/// <summary>
/// MEMORY blockAddress COMMA blockAddress
/// </summary>
any visitor::visitMemory(parser68000::MemoryContext* ctx)
{
    uint32_t memStart = getAddressValue(ctx->blockAddress(0));
    uint32_t memEnd = getAddressValue(ctx->blockAddress(1));

    if (pass != 0)
    {
        return any();
    }
    if (memEnd <= memStart)
    {
        addPass0Error("Invalid MEMORY range: " + to_string(memStart) + " - " + to_string(memEnd), ctx);
        return any();
    }
    if (memoryStart != 0 || memoryEnd != 0)
    {
        addPass0Error("Multiple MEMORY directives are not allowed", ctx);
        return any();
    }
    memoryStart = memStart;
    memoryEnd = memEnd;

    return any();
}

any visitor::visitDataList(parser68000::DataListContext* ctx)
{
    size_t insertionStart = code().size(); // the index where the data will be inserted
	size_t n = ctx->children.size();
	switch (size)
	{
	case 0: // byte
		for (size_t i = 0; i < n; i += 2)
		{
			any data = visit(ctx->children[i]);
			if (data.type() == typeid(std::string))
			{
				string chars = any_cast<string>(data);
				string::iterator start = chars.begin() + 1;
				string::iterator end = chars.end() - 1;

				dcBytes.insert(dcBytes.end(), start, end);
			}
			else
			{
				int value = any_cast<int>(data);
				if (value < -128 || value > 255)
				{
					addError("Value out of range for dc.b: " + to_string(value), ctx->children[i]);
					value = 0;
				}
				uint8_t byte = static_cast<uint8_t>(value);
				dcBytes.push_back(byte);
			}
		}
        listing.setBinary(dcBytes.begin(), dcBytes.end());
		return dcBytes.size();
		break;
	case 1: // word
		for (size_t i = 0; i < n; i += 2)
		{
			any data = visit(ctx->children[i]);
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
					code().push_back(word);
					currentAddress += 2;
				}
			}
			else
			{
				int value = any_cast<int>(data);
				uint16_t number = static_cast<uint16_t>(value);
				if (value < -32768 || value > 65535)
				{
					addError("Value out of range for dc.w: " + to_string(value), ctx->children[i]);
					number = 0;
				}
				code().push_back(number);
				currentAddress += 2;
			}
		}
        listing.setBinary(code().begin() + insertionStart, code().end(), size);
		return 0;
		break;
	case 2: // long
		for (size_t i = 0; i < n; i += 2)
		{
			any data = visit(ctx->children[i]);
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
					code().push_back(codelong >> 16);
					code().push_back(codelong & 0xffff);
					currentAddress += 4;
				}
			}
			else
			{
				uint32_t number = static_cast<uint32_t>(any_cast<int>(data));
				code().push_back(number >> 16);
				code().push_back(number & 0xffff);
				currentAddress += 4;
			}
		}
        listing.setBinary(code().begin() + insertionStart, code().end(), size);
        return 0;
		break;
	}
	return dcBytes.size();
}

any visitor::visitExpression(parser68000::ExpressionContext* ctx)
{
	return visit(ctx->children[0]);
}

int32_t anytoint(any val)
{
	if (val.type() == typeid(int32_t))
	{
		return any_cast<int32_t>(val);
	}
	else if (val.type() == typeid(uint32_t))
	{
		return static_cast<int32_t>(any_cast<uint32_t>(val));
	}
	else
	{
		assert(!"Invalid type in expression");
		return 0;
	}
}
any visitor::visitAdditiveExpr(parser68000::AdditiveExprContext* ctx) 
{
	any lhs = visit(ctx->children[0]);
	if (lhs.type() == typeid(std::string))
	{
		// only one string is allowed in an expression
		assert(ctx->children.size() == 1);
		return lhs;
	}
	int32_t result = anytoint(lhs);

	for(int i=1; i<ctx->children.size(); i+=2)
	{
		string op = ctx->children[i]->getText();
		any anyrhs = visit(ctx->children[i + 1]);
		if (anyrhs.type() == typeid(int32_t) || anyrhs.type() == typeid(uint32_t))
		{
			int32_t rhs = anytoint(anyrhs);
			if (op == "+")
			{
				result += rhs;
			}
			else if (op == "-")
			{
				result -= rhs;
			}
			else
			{
				assert(!"Unknown operator");
			}
		}
		else
		{
			addError("Invalid expression: only numbers are supported in expression", ctx->children[i + 1]);
		}
	}
	return result;
}

any visitor::visitDleNumber(parser68000::DleNumberContext* ctx)
{
	return visit(ctx->children[0]);
}

any visitor::visitDleString(parser68000::DleStringContext* ctx)
{
	return ctx->children[0]->getText();
}

any visitor::visitDleIdentifier(parser68000::DleIdentifierContext* ctx)
{
	std::string label = ctx->ID2()->getText();

	// First check if the identifier is a symbol
	auto symbol = symbols.find(label);
	if (symbol != symbols.end())
	{
		return symbol->second;
	}

	// then search for a label
	uint32_t target;
	auto it = labels.find(label);
	if (it == labels.end())
	{
		// During the 1st pass the label may not be defined yet
		if (pass != 0)
		{
			addError("Label not found: " + label, ctx->children[0]);
		}
		target = 0;
	}
	else
	{
		target = it->second;
	}
	return static_cast<int>(target);
}

any visitor::visitDleStar(parser68000::DleStarContext* ctx)
{
	return static_cast<int32_t>(this->currentAddress + dcBytes.size());
}

any visitor::visitDleExpression(parser68000::DleExpressionContext* ctx)
{
	return visit(ctx->children[1]);
}

uint32_t visitor::getAddressValue(parser68000::BlockAddressContext* ctx)
{
	any address = visit(ctx);
	uint32_t target;
	if (address.type() == typeid(std::string))
	{
		std::string label = any_cast<std::string>(address);
		// search first for a symbol
		auto it = symbols.find(label);
		if (it != symbols.end())
		{
			if (it->second.type() != typeid(int32_t))
			{
				addError("Argument is not an number: " + label, ctx);
				target = 0;
			}
			else
			{
				int32_t value = any_cast<int32_t>(it->second);
				if (value < 0)
				{
					addError("Address cannot be negative: " + label, ctx);
					value = 0;
				}
				target = static_cast<uint32_t>(value);
			}
		}
		else
		{
			// then search for a label
			auto it = labels.find(label);
			if (it != labels.end())
			{
				addError("Labels cannot be used in the ORG directive: " + label, ctx);
			}
			else
			{
				addError("Symbol not found: " + label, ctx);
			}
			target = 0;
		}
	}
	else
	{
		int32_t value = any_cast<long>(address);
		if (value < 0)
		{
			addError("Address cannot be negative: " + std::to_string(value), ctx);
			value = 0;
		}
		target = static_cast<uint32_t>(value);
	}
	return target;
}
