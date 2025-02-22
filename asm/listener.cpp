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
/*
void listener::enterAbcd(parser68000::AbcdContext* ctx)
{
	std::cout << "enterAbcd" << std::endl;
	auto t1 = ctx->children[0]->getTreeType();
	auto t2 = ctx->children[1]->getTreeType();

	auto d1 = ctx->dRegister();
	auto d2 = ctx->aRegisterIndirectPreDecrement();
	if (!d1.empty())
	{
		auto s1 = d1[0]->DREG();
		auto s2 = d1[1]->DREG();
	}
}
void listener::exitAbcd(parser68000::AbcdContext* ctx)
{
	std::cout << "exitAbcd" << std::endl;
}
*/
void listener::enterAbcd_dRegister(parser68000::Abcd_dRegisterContext* ctx) 
{
	std::cout << "enterAbcd_dRegister" << std::endl;
	//parser68000::DRegisterContext* d1 = ctx->children[1];
	auto dd1 = ctx->children[1];
	auto d1 = ctx->dRegister(0);
	auto d2 = ctx->dRegister(1);
	auto t1 = d1->getText();
	auto t2 = d2->getText();
}
void listener::exitAbcd_dRegister(parser68000::Abcd_dRegisterContext* ctx)
{
	std::cout << "exitAbcd_dRegister" << std::endl;
}

void listener::enterAbcd_indirect(parser68000::Abcd_indirectContext* ctx)
{
	std::cout << "enterAbcd_indirect" << std::endl;
}
void listener::exitAbcd_indirect(parser68000::Abcd_indirectContext* ctx)
{
	std::cout << "exitAbcd_indirect" << std::endl;
}

