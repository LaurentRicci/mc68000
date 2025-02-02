parser grammar parser68000;

options 
{
    tokenVocab=lexer68000;
}

prog : (line EOL)* EOF ;

line
    : labelSection? instructionSection WS? COMMENT?
    | labelSection? directiveSection WS? COMMENT?
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
    : WS instruction size? WS? arguments?
    ;

size 
    : SIZEBYTE 
    | SIZEWORD 
    | SIZELONG;

directiveSection
    : WS directive 
    ; 

directive
    : DC size WS dataList
    | DCB size?
    | DS size? WS number
    | END WS address
    | EQU
    | FAIL
    | INCLUDE
    | INCBIN
    | LIST
    | NOLIST
    | MEMORY
    | OPT
    | ORG WS number
    | PAGE
    | REG
    | SECTION
    | SET
    | SIMHALT
    ;

instruction
    : abcd
    | ADD
    | ADDA
    | ADDI
    | ADDQ
    | ADDX
    | AND
    | ANDI
    | ASL
    | BCC
    | BCHG
    | BCLR
    | BCS
    | BEQ
    | BGE
    | BGT
    | BHI
    | BLE
    | BLS
    | BLT
    | BMI
    | BNE
    | BPL
    | BRA
    | BSET
    | BSR
    | BTST
    | BVC
    | BVS
    | CHK
    | CLR
    | CMP
    | CMPA
    | CMPI
    | CMPM
    | DBCC
    | DIVS
    | DIVU
    | EOR
    | EORI
    | EXG
    | EXT
    | ILLEGAL
    | JMP
    | JSR
    | LEA
    | LINK
    | LSL
    | LSR
    | MOVE
    | MOVEA
    | MOVEM
    | MOVEP
    | MOVEQ
    | MULS
    | MULU
    | NBCD
    | NEG
    | NEGX
    | NOP
    | NOT
    | OR
    | ORI
    | PEA
    | ROL
    | ROR
    | ROXL
    | ROXR
    | RTE
    | RTR
    | RTS
    | SBCD
    | SCC
    | STOP
    | SUB
    | SUBA
    | SUBI
    | SUBQ
    | SUBX
    | SWAP
    | TAS
    | TRAP
    | TRAPV
    | TST
    | UNLK
    ;

    abcd
    : ABCD WS? dRegister WS? COMMA WS? dRegister
    | ABCD WS? aRegisterIndirectPreDecrement WS? COMMA WS? aRegisterIndirectPreDecrement
    ;

emptyLine : WS ;

number 
    : OCTAL
    | DECIMAL
    | HEXADECIMAL
    ;


register
    : DREG
    | AREG
    | SP
    | PC
    ;

registerList
    : registerListElement (SLASH registerListElement)*
    ;

registerListElement
    : adRegister
    | registerRange
    ;

adRegister
    : AREG
    | DREG
    ;

registerRange
    : adRegister DASH adRegister
    ;

arguments
    : argument
    | argument COMMA argument
    ;

argument
    : addressingMode
    | registerList
    | ID
    ;

addressingMode
    : dRegister
    | aRegister
    | aRegisterIndirect
    | aRegisterIndirectPostIncrement
    | aRegisterIndirectPreDecrement
    | aRegisterIndirectDisplacement
    | aRegisterIndirectIndex
    | absoluteShort
    | absoluteLong
    | pcIndirectDisplacement
    | pcIndirectIndex
    | immediateData
    ;
dRegister : DREG ;
aRegister : AREG ;
aRegisterIndirect : OPENPAREN WS? AREG WS? CLOSEPAREN ;
aRegisterIndirectPostIncrement : OPENPAREN WS? AREG WS? CLOSEPAREN WS? PLUS ;
aRegisterIndirectPreDecrement : DASH WS? OPENPAREN WS? AREG WS? CLOSEPAREN ;
aRegisterIndirectDisplacement : number WS? OPENPAREN WS? AREG WS? CLOSEPAREN ;
aRegisterIndirectIndex : number WS? OPENPAREN WS? AREG WS? COMMA WS? dRegister WS? CLOSEPAREN ;
absoluteShort : number ;
absoluteLong : number SIZELONG;
pcIndirectDisplacement : number WS? OPENPAREN WS? PC WS? CLOSEPAREN ;
pcIndirectIndex : number WS? OPENPAREN WS? PC WS? COMMA WS? dRegister WS? CLOSEPAREN ;
immediateData : HASH number ;

dataList
    : dataListElement (COMMA dataListElement)*
    ;

dataListElement
    : number
    | STRING
    ;

address
    : number
    | ID
    ;