#pragma once
#include "parser68000BaseListener.h"

class listener : public parser68000BaseListener
{
public:
	virtual void enterInstructionSection(parser68000::InstructionSectionContext* ctx) override;
	virtual void exitInstructionSection(parser68000::InstructionSectionContext* ctx) override;

private:
};