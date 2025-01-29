#pragma once
#include "parser68000BaseListener.h"

class listener : public parser68000BaseListener
{
public:
	virtual void enterLine(parser68000::LineContext* ctx) ;
	virtual void exitLine(parser68000::LineContext* ctx) ;

	virtual void exitCommentLine(parser68000::CommentLineContext* ctx);
	virtual void exitLabelSection(parser68000::LabelSectionContext* ctx);
	virtual void exitInstruction(parser68000::InstructionContext* ctx);
	virtual void exitDirective(parser68000::DirectiveContext* ctx);

};