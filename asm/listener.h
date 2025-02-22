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
	/*
	virtual void enterAbcd(parser68000::AbcdContext* ctx);
	virtual void exitAbcd(parser68000::AbcdContext* ctx);
	*/
	virtual void enterAbcd_dRegister(parser68000::Abcd_dRegisterContext* ctx);
	virtual void exitAbcd_dRegister(parser68000::Abcd_dRegisterContext* ctx);

	virtual void enterAbcd_indirect(parser68000::Abcd_indirectContext* ctx);
	virtual void exitAbcd_indirect(parser68000::Abcd_indirectContext* ctx);

};