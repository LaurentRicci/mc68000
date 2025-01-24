parser grammar parser68000;

options 
{
    tokenVocab=lexer68000;
}

prog : (line EOL)* EOF ;

line
    : labelSection? instructionSection
    | labelSection
    | commentLine
    | emptyLine
    ;

labelSection
    : ID
    | ID SEMICOLON
    | WS ID SEMICOLON
    | WS
    ;

commentLine : COMMENTLINE;

instructionSection
    : WS instruction
    ;

instruction
    : ABCD
    | ADD
    ;



emptyLine : WS ;

