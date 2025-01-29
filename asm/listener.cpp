#include "listener.h"
#include <iostream>

void listener::enterLine(parser68000::LineContext* ctx)
{
	//std::cout << "enterLine" << std::endl;
}

void listener::exitLine(parser68000::LineContext* ctx)
{
//	std::cout << ctx->start->toString() << std::endl;
}

void listener::exitCommentLine(parser68000::CommentLineContext* ctx)
{
	std::cout << ctx->COMMENTLINE()->getText() << std::endl;
}

void listener::exitLabelSection(parser68000::LabelSectionContext* ctx)
{
	// std::cout << ctx->ID()->getText() << ":" << std::endl;
}

void listener::exitInstruction(parser68000::InstructionContext* ctx)
{
	std::cout << "\t" << ctx->getText() << std::endl;
}

void listener::exitDirective(parser68000::DirectiveContext* ctx)
{
	std::cout << "\t" << ctx->getText() << std::endl;
}