#pragma once
#include "parser68000BaseListener.h"

class listener : public parser68000BaseListener
{
public:
	virtual void enterLine(parser68000::LineContext* ctx) ;
	virtual void exitLine(parser68000::LineContext* ctx) ;
};