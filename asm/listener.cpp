#include "listener.h"
#include <iostream>

void listener::enterLine(parser68000::LineContext* ctx)
{
	//std::cout << "enterLine" << std::endl;
}

void listener::exitLine(parser68000::LineContext* ctx)
{
	std::cout << ctx->start->toString() << std::endl;
}