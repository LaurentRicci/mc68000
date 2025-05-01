parser grammar parser68000;

options 
{
    tokenVocab=lexer68000;
}

prog : (line EOL)* EOF ;

line
    : labelSection? instructionSection COMMENT?  #line_instructionSection
    | labelSection? directiveSection COMMENT?    #line_directiveSection
    | labelSection                               #line_labelSection
    | commentLine                                #line_commentLine
    ;

labelSection
    : LABEL
    | LABEL SEMICOLON
    | SPACES ID SEMICOLON
    | SPACES
    ;

commentLine : COMMENTLINE;

instructionSection
    : abcd                                      #instructionSection_abcd
    | add                                       #instructionSection_add
    | instruction size? arguments?              #instructionSection_instruction
    ;

size returns [uint16_t value]
    : SIZEBYTE { $value = 0; }
    | SIZEWORD { $value = 1; }
    | SIZELONG { $value = 2; }
    ;

directiveSection
    : directive 
    ; 

directive
    : DC size  dataList
    | DCB size?
    | DS size?  number
    | END  address
    | EQU
    | FAIL
    | INCLUDE
    | INCBIN
    | LIST
    | NOLIST
    | MEMORY
    | OPT
    | ORG  number
    | PAGE
    | REG
    | SECTION
    | SET
    | SIMHALT
    ;

instruction
    : ADD
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
    : ABCD dRegister COMMA dRegister                                         #abcd_dRegister
    | ABCD aRegisterIndirectPreDecrement COMMA aRegisterIndirectPreDecrement #abcd_indirect
    ;

    add
    : ADD size? addressingMode COMMA dRegister #add_to_dRegister
    | ADD size? dRegister COMMA addressingMode #add_from_dRegister
    ;

// emptyLine : WS ;

number returns [int32_t value]
    : OCTAL         {$value = 1234; } // std::strtol($OCTAL.text.c_str(), nullptr, 8);}
    | DECIMAL       {$value = std::stoi($DECIMAL.text.c_str());}
    | HEXADECIMAL   {$value = 3456; } // std::strtol($HEXADECIMAL.text.c_str(), nullptr, 16);}
    ;


register
    : DREG { $DREG.text; }
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
aRegisterIndirect : OPENPAREN AREG CLOSEPAREN ;
aRegisterIndirectPostIncrement : OPENPAREN  AREG  CLOSEPAREN  PLUS ;
aRegisterIndirectPreDecrement : DASH  OPENPAREN  AREG  CLOSEPAREN ;
aRegisterIndirectDisplacement : number  OPENPAREN  AREG  CLOSEPAREN ;
aRegisterIndirectIndex : number  OPENPAREN  AREG  COMMA  dRegister  CLOSEPAREN ;
absoluteShort : number ;
absoluteLong : number SIZELONG;
pcIndirectDisplacement : number  OPENPAREN  PC  CLOSEPAREN ;
pcIndirectIndex : number  OPENPAREN  PC  COMMA  dRegister  CLOSEPAREN ;
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