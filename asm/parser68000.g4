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
    | toCCR
    | toSR
    | aslAsr
    | bcc
    | bit
    | chk
    | clr
    | cmp
    | cmpa
    | cmpm
    | dbcc
    | divs
    | divu
    | eor
    | exg
    | muls
    | mulu
    | nop
    | immediate
    | instruction size? arguments?
    ;

immediate
    : immediateInstruction size? immediateData COMMA addressingMode
    ;

toCCR
    : toCCRorSRInstruction immediateData COMMA CCR
    ;

toSR
    : toCCRorSRInstruction immediateData COMMA SR
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
    : EXT
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

toCCRorSRInstruction returns  [uint16_t value]
    : ANDI { $value = 0b0010;}
    | EORI { $value = 0b1010;}
    | ORI  { $value = 0b0000;}
    ;

shiftInstruction returns [uint16_t value]
    : ASL { $value = 1; }
    | ASR { $value = 0; }
    ;

bccInstruction returns [uint16_t value]
    : BCC { $value = 0b0100; }
    | BCS { $value = 0b0101; }
    | BEQ { $value = 0b0111; }
    | BNE { $value = 0b0110; }
    | BGE { $value = 0b1100; }
    | BGT { $value = 0b1110; }
    | BHI { $value = 0b0010; }
    | BLE { $value = 0b1111; }
    | BLS { $value = 0b0011; }
    | BLT { $value = 0b1101; }
    | BMI { $value = 0b1011; }
    | BPL { $value = 0b1010; }
    | BVC { $value = 0b1000; }
    | BVS { $value = 0b1001; }
    | BRA { $value = 0b0000; }
    | BSR { $value = 0b0001; }
    ;

dbccInstruction returns [uint16_t value]
    : DBCC { $value = 0b0100; }
    | DBCS { $value = 0b0101; }
    | DBEQ { $value = 0b0111; }
    | DBNE { $value = 0b0110; }
    | DBGE { $value = 0b1100; }
    | DBGT { $value = 0b1110; }
    | DBHI { $value = 0b0010; }
    | DBLE { $value = 0b1111; }
    | DBLS { $value = 0b0011; }
    | DBLT { $value = 0b1101; }
    | DBMI { $value = 0b1011; }
    | DBPL { $value = 0b1010; }
    | DBVC { $value = 0b1000; }
    | DBVS { $value = 0b1001; }
    | DBRA { $value = 0b0001; }
    | DBF  { $value = 0b0001; }
    | DBT  { $value = 0b0000; }
    ;

bitInstruction returns [uint16_t value]
    : BCHG { $value = 0b001; }
    | BCLR { $value = 0b010; }
    | BSET { $value = 0b011; }
    | BTST { $value = 0b000; }
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

aslAsr
    : shiftInstruction size? dRegister COMMA dRegister       #aslAsr_dRegister
    | shiftInstruction size? HASH number COMMA dRegister     #aslAsr_immediateData
    | shiftInstruction size? addressingMode                  #aslAsr_addressingMode
    ;

bcc
    : bccInstruction address
    ;

bit
    : bitInstruction dRegister COMMA addressingMode     #bit_dRegister
    | bitInstruction HASH number COMMA addressingMode   #bit_immediateData
    ;

chk
    : CHK addressingMode COMMA dRegister
    ;

clr
    : CLR size? addressingMode
    ;

cmp
    : CMP size? addressingMode COMMA dRegister
    ;

cmpa
    : CMPA size? addressingMode COMMA aRegister
    ;

cmpm
    : CMPM size? aRegisterIndirectPostIncrement COMMA aRegisterIndirectPostIncrement
    ;

dbcc
    : dbccInstruction dRegister COMMA address
    ;

divs
    : DIVS addressingMode COMMA dRegister
    ;

divu
    : DIVU addressingMode COMMA dRegister
    ;

eor
    : EOR size? dRegister COMMA addressingMode
    ;

exg
    : EXG adRegister COMMA adRegister
    ;

muls
    : MULS addressingMode COMMA dRegister
    ;

mulu
    : MULU addressingMode COMMA dRegister
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