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

labelSection returns [std::string value]
    : LABEL                { $value = $LABEL.text; }
    | LABEL SEMICOLON      { $value = $LABEL.text; }
    | SPACES ID SEMICOLON  { $value = $ID.text; }
    | SPACES               { $value = ""; }
    ;

commentLine : COMMENTLINE;

instructionSection
    : abcd
    | add
    | adda
    | addq
    | addx 
    | and
    | nop
    | immediate
    | instruction size? arguments?
    ;

immediate
    : immediateInstruction size? immediateData COMMA addressingMode
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
    : ASL
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
    | CMPM
    | DBCC
    | DIVS
    | DIVU
    | EOR
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
    | SUBQ
    | SUBX
    | SWAP
    | TAS
    | TRAP
    | TRAPV
    | TST
    | UNLK
    ;

immediateInstruction  returns [uint16_t value]
    : ADDI { $value = 0b0110;}
    | SUBI { $value = 0b0100;}
    | ANDI { $value = 0b0010;}
    | EORI { $value = 0b1010;}
    | ORI  { $value = 0b0000;}
    | CMPI { $value = 0b1100;}
    ;

abcd
    : ABCD dRegister COMMA dRegister                                         #abcd_dRegister
    | ABCD aRegisterIndirectPreDecrement COMMA aRegisterIndirectPreDecrement #abcd_indirect
    ;

add
    : ADD size? addressingMode COMMA dRegister #add_to_dRegister
    | ADD size? dRegister COMMA addressingMode #add_from_dRegister
    ;

adda
    : ADDA size? addressingMode COMMA aRegister
    ;

addq
    : ADDQ size? HASH number COMMA addressingMode
    ;

addx
    : ADDX size? dRegister COMMA dRegister                                         #addx_dRegister
    | ADDX size? aRegisterIndirectPreDecrement COMMA aRegisterIndirectPreDecrement #addx_indirect
    ;

and
    : AND size? addressingMode COMMA dRegister #and_to_dRegister
    | AND size? dRegister COMMA addressingMode #and_from_dRegister
    ;

nop
    : NOP
    ;

// emptyLine : WS ;

number returns [int32_t value]
    : OCTAL         { $value = std::strtol($OCTAL.text.c_str()+1, nullptr, 8);        }
    | DECIMAL       { $value = std::stoi($DECIMAL.text.c_str());                      }
    | HEXADECIMAL   { $value = std::strtol($HEXADECIMAL.text.c_str()+1, nullptr, 16); }
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

adRegisterSize
    : adRegister size?
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
aRegisterIndirectDisplacement
    : number  OPENPAREN  AREG  CLOSEPAREN            #aRegisterIndirectDisplacementOld
    | OPENPAREN number COMMA AREG CLOSEPAREN         #aRegisterIndirectDisplacementNew
    ;
aRegisterIndirectIndex
    : number  OPENPAREN  AREG  COMMA  adRegisterSize  CLOSEPAREN        #aRegisterIndirectIndexOld
    | OPENPAREN number  COMMA AREG  COMMA  adRegisterSize  CLOSEPAREN   #aRegisterIndirectIndexNew
    ;
absoluteShort : address ;
absoluteLong : address SIZELONG;
pcIndirectDisplacement : number  OPENPAREN  PC  CLOSEPAREN ;
pcIndirectIndex : number  OPENPAREN  PC  COMMA  adRegisterSize  CLOSEPAREN ;
immediateData : HASH number ;

dataList
    : dataListElement (COMMA dataListElement)*
    ;

dataListElement
    : number
    | STRING
    ;

address returns [std::any value]
    : number     { $value = $number.value; }
    | ID         { $value = $ID.text; }
    ;