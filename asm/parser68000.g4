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
    : asbcd
    | add
    | adda
    | addq
    | addx 
    | andOr
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
    | ext
    | illegal
    | jmp
    | jsr
    | lea
    | link
    | lslLsr
    | move
    | movea
    | moveusp
    | move2usp
    | movesr
    | move2sr
    | movem
    | movep
    | moveq
    | muls
    | mulu
    | nbcd
    | neg
    | negx
    | nop
    | not
    | pea
    | resetInstruction
    | rolRor
    | roxlRoxr
    | rte
    | rtr
    | rts
    | scc
    | stop
    | swap
    | tas
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
    : TRAP
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

logicalShiftInstruction returns [uint16_t value]
    : LSL { $value = 1; }
    | LSR { $value = 0; }
    ;

rotateInstruction returns [uint16_t value]
    : ROL { $value = 1; }
    | ROR { $value = 0; }
    ;

rotateXInstruction returns [uint16_t value]
    : ROXL { $value = 1; }
    | ROXR { $value = 0; }
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

sccInstruction returns [uint16_t value]
    : SCC { $value = 0b0100; }
    | SCS { $value = 0b0101; }
    | SEQ { $value = 0b0111; }
    | SNE { $value = 0b0110; }
    | SGE { $value = 0b1100; }
    | SGT { $value = 0b1110; }
    | SHI { $value = 0b0010; }
    | SLE { $value = 0b1111; }
    | SLS { $value = 0b0011; }
    | SLT { $value = 0b1101; }
    | SMI { $value = 0b1011; }
    | SPL { $value = 0b1010; }
    | SVC { $value = 0b1000; }
    | SVS { $value = 0b1001; }
    | SRA { $value = 0b0001; }
    | SF  { $value = 0b0001; }
    | ST  { $value = 0b0000; }
    ;

bitInstruction returns [uint16_t value]
    : BCHG { $value = 0b001; }
    | BCLR { $value = 0b010; }
    | BSET { $value = 0b011; }
    | BTST { $value = 0b000; }
    ;

andOrInstruction returns [uint16_t value]
    : AND { $value = 0b1100; }
    | OR  { $value = 0b1000; }
    ;

asbcdInstruction returns [uint16_t value]
    : ABCD { $value = 0b1100; }
    | SBCD { $value = 0b1000; }
    ;

addSubInstruction returns [uint16_t value]
    : ADD { $value = 0b1101; }
    | SUB { $value = 0b1001; }
    ;

addaSubaInstruction returns [uint16_t value]
    : ADDA { $value = 0b1101; }
    | SUBA { $value = 0b1001; }
    ;

addiSubiInstruction returns [uint16_t value]
    : ADDI { $value = 0b0110; }
    | SUBI { $value = 0b0100; }
    ;

addqSubqInstruction returns [uint16_t value]
    : ADDQ { $value = 0b0; }
    | SUBQ { $value = 0b1; }
    ;

addxSubxInstruction returns [uint16_t value]
    : ADDX { $value = 0b1101; }
    | SUBX { $value = 0b1001; }
    ;

asbcd
    : asbcdInstruction dRegister COMMA dRegister                                         #asbcd_dRegister
    | asbcdInstruction aRegisterIndirectPreDecrement COMMA aRegisterIndirectPreDecrement #asbcd_indirect
    ;

add
    : addSubInstruction size? addressingMode COMMA dRegister #add_to_dRegister
    | addSubInstruction size? dRegister COMMA addressingMode #add_from_dRegister
    ;

adda
    : addaSubaInstruction size? addressingMode COMMA aRegister
    ;

addq
    : addqSubqInstruction size? HASH number COMMA addressingMode
    ;

addx
    : addxSubxInstruction size? dRegister COMMA dRegister                                         #addx_dRegister
    | addxSubxInstruction size? aRegisterIndirectPreDecrement COMMA aRegisterIndirectPreDecrement #addx_indirect
    ;

andOr
    : andOrInstruction size? addressingMode COMMA dRegister #andOr_to_dRegister
    | andOrInstruction size? dRegister COMMA addressingMode #andOr_from_dRegister
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

ext
    : EXT size? dRegister
    ;

illegal
    : ILLEGAL
    ;

jmp
    : JMP addressingMode
    ;

jsr
    : JSR addressingMode
    ;

lea
    : LEA addressingMode COMMA aRegister
    ;

link
    : LINK aRegister COMMA immediateData
    ;

lslLsr
    : logicalShiftInstruction size? dRegister COMMA dRegister                #lslLsr_dRegister
    | logicalShiftInstruction size? HASH number COMMA dRegister              #lslLsr_immediateData
    | logicalShiftInstruction size? addressingMode                           #lslLsr_addressingMode
    ;

move
    : MOVE size? addressingMode COMMA addressingMode
    ;

movea
    : MOVEA size? addressingMode COMMA aRegister
    ;

moveusp
    : MOVE USP COMMA aRegister
    ;

move2usp
    : MOVE aRegister COMMA USP
    ;

movesr
    : MOVE SR COMMA addressingMode
    ;

move2sr
    : MOVE addressingMode COMMA SR
    ;

movem
    : MOVEM size? registerList COMMA addressingMode     #movem_toMemory
    | MOVEM size? addressingMode COMMA registerList     #movem_fromMemory
    ;

movep
    : MOVEP size? dRegister COMMA aRegisterIndirectDisplacement         #movep_toMemory
    | MOVEP size? aRegisterIndirectDisplacement COMMA dRegister         #movep_fromMemory
    ;

moveq
    : MOVEQ HASH number COMMA dRegister
    ;

muls
    : MULS addressingMode COMMA dRegister
    ;

mulu
    : MULU addressingMode COMMA dRegister
    ;

nbcd
    : NBCD addressingMode
    ;

neg
    : NEG size? addressingMode
    ;

negx
    : NEGX size? addressingMode
    ;

nop
    : NOP
    ;

not
    : NOT size? addressingMode
    ;

pea
    : PEA addressingMode
    ;

resetInstruction // using reset as name would conflict with the 'reset' used by ANTLR
    : RESET
    ;

rolRor
    : rotateInstruction size? dRegister COMMA dRegister                #RolRor_dRegister
    | rotateInstruction size? HASH number COMMA dRegister              #RolRor_immediateData
    | rotateInstruction size? addressingMode                           #RolRor_addressingMode
    ;

roxlRoxr
    : rotateXInstruction size? dRegister COMMA dRegister                #RoxlRoxr_dRegister
    | rotateXInstruction size? HASH number COMMA dRegister              #RoxlRoxr_immediateData
    | rotateXInstruction size? addressingMode                           #RoxlRoxr_addressingMode
    ;

rte 
    : RTE
    ;

rtr 
    : RTR
    ;

rts
    : RTS
    ;

scc
    : sccInstruction addressingMode
    ;

stop
    : STOP immediateData
    ;

swap
    : SWAP dRegister
    ;

tas
    : TAS addressingMode
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
    : adRegister        #registerListRegister
    | registerRange     #registerListRange
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
    | absolute
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
absolute : address ;
absoluteShort : address SIZEWORD;
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