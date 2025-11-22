parser grammar parser68000;

options 
{
    tokenVocab=lexer68000;
}

prog : (line (EOL | EOL2))* (SPACES)? EOF ;

line
    : labelSection instructionSection COMMENT?  #line_instructionSection
    | labelSection directiveSection COMMENT?    #line_directiveSection
    | labelSection                              #line_labelSection
    | commentLine                               #line_commentLine
    ;

labelSection returns [std::string value]
    : LABEL                { $value = $LABEL.text; }
    | LABEL SEMICOLON      { $value = $LABEL.text; }
    | SPACES ID SEMICOLON  { $value = $ID.text; }
    | SPACES               { $value = ""; }
    ;

commentLine : COMMENTLINE;

// ============================================
// Instructions
//=============================================
instructionSection
    : abcdSbcd
    | add
    | adda
    | addq
    | addx 
    | andOr
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
    | immediate
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
    | toCCR
    | toSR
    | trap
    | trapv
    | tst
    | unlk
    ;


abcdSbcd
    : abcdSbcdInstruction dRegister COMMA dRegister                                         #abcdSbcd_dRegister
    | abcdSbcdInstruction aRegisterIndirectPreDecrement COMMA aRegisterIndirectPreDecrement #abcdSbcd_indirect
    ;
abcdSbcdInstruction returns [uint16_t value]
    : ABCD { $value = 0b1100; }
    | SBCD { $value = 0b1000; }
    ;


add
    : addSubInstruction size? addressingMode COMMA dRegister #add_to_dRegister
    | addSubInstruction size? dRegister COMMA addressingMode #add_from_dRegister
    ;
addSubInstruction returns [uint16_t value]
    : ADD { $value = 0b1101; }
    | SUB { $value = 0b1001; }
    ;

adda
    : addaSubaInstruction size? addressingMode COMMA aRegister
    ;
addaSubaInstruction returns [uint16_t value]
    : ADDA { $value = 0b1101; }
    | SUBA { $value = 0b1001; }
    ;

addq
    : addqSubqInstruction size? HASH address COMMA addressingMode
    ;
addqSubqInstruction returns [uint16_t value]
    : ADDQ { $value = 0b0; }
    | SUBQ { $value = 0b1; }
    ;


addx
    : addxSubxInstruction size? dRegister COMMA dRegister                                         #addx_dRegister
    | addxSubxInstruction size? aRegisterIndirectPreDecrement COMMA aRegisterIndirectPreDecrement #addx_indirect
    ;
addxSubxInstruction returns [uint16_t value]
    : ADDX { $value = 0b1101; }
    | SUBX { $value = 0b1001; }
    ;

andOr
    : andOrInstruction size? addressingMode COMMA dRegister #andOr_to_dRegister
    | andOrInstruction size? dRegister COMMA addressingMode #andOr_from_dRegister
    ;
andOrInstruction returns [uint16_t value]
    : AND { $value = 0b1100; }
    | OR  { $value = 0b1000; }
    ;


aslAsr
    : shiftInstruction size? dRegister COMMA dRegister       #aslAsr_dRegister
    | shiftInstruction size? HASH address COMMA dRegister     #aslAsr_immediateData
    | shiftInstruction size? addressingMode                  #aslAsr_addressingMode
    ;
shiftInstruction returns [uint16_t value]
    : ASL { $value = 1; }
    | ASR { $value = 0; }
    ;

bcc
    : bccInstruction address
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

bit
    : bitInstruction dRegister COMMA addressingMode     #bit_dRegister
    | bitInstruction HASH address COMMA addressingMode   #bit_immediateData
    ;
bitInstruction returns [uint16_t value]
    : BCHG { $value = 0b001; }
    | BCLR { $value = 0b010; }
    | BSET { $value = 0b011; }
    | BTST { $value = 0b000; }
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

immediate
    : immediateInstruction size? immediateData COMMA addressingMode
    ;
immediateInstruction  returns [uint16_t value]
    : ADDI { $value = 0b0110;}
    | SUBI { $value = 0b0100;}
    | ANDI { $value = 0b0010;}
    | EORI { $value = 0b1010;}
    | ORI  { $value = 0b0000;}
    | CMPI { $value = 0b1100;}
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
    : LINK aRegister COMMA HASH address
    ;

lslLsr
    : logicalShiftInstruction size? dRegister COMMA dRegister                #lslLsr_dRegister
    | logicalShiftInstruction size? HASH address COMMA dRegister              #lslLsr_immediateData
    | logicalShiftInstruction size? addressingMode                           #lslLsr_addressingMode
    ;
logicalShiftInstruction returns [uint16_t value]
    : LSL { $value = 1; }
    | LSR { $value = 0; }
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
    : MOVEQ HASH address COMMA dRegister
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
    | rotateInstruction size? HASH address COMMA dRegister              #RolRor_immediateData
    | rotateInstruction size? addressingMode                           #RolRor_addressingMode
    ;
rotateInstruction returns [uint16_t value]
    : ROL { $value = 1; }
    | ROR { $value = 0; }
    ;

roxlRoxr
    : rotateXInstruction size? dRegister COMMA dRegister                #RoxlRoxr_dRegister
    | rotateXInstruction size? HASH address COMMA dRegister              #RoxlRoxr_immediateData
    | rotateXInstruction size? addressingMode                           #RoxlRoxr_addressingMode
    ;
rotateXInstruction returns [uint16_t value]
    : ROXL { $value = 1; }
    | ROXR { $value = 0; }
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

stop
    : STOP immediateData
    ;

swap
    : SWAP dRegister
    ;

tas
    : TAS addressingMode
    ;

toCCR
    : toCCRorSRInstruction immediateData COMMA CCR
    ;
toCCRorSRInstruction returns  [uint16_t value]
    : ANDI { $value = 0b0010;}
    | EORI { $value = 0b1010;}
    | ORI  { $value = 0b0000;}
    ;

toSR
    : toCCRorSRInstruction immediateData COMMA SR
    ;

trap
    : TRAP HASH address
    ;

trapv
    : TRAPV
    ;

tst
    : TST size? addressingMode
    ;

unlk
    : UNLK aRegister
    ;

// ============================================
// Instructions arguments
//=============================================

size returns [uint16_t value]
    : SIZEBYTE { $value = 0; }
    | SIZEWORD { $value = 1; }
    | SIZELONG { $value = 2; }
    ;

number returns [std::any value]
    : OCTAL         { $value = (uint32_t) std::strtoul($OCTAL.text.c_str()+1, nullptr, 8);        }
    | DECIMAL       { $value = (int32_t)  std::stol($DECIMAL.text.c_str());                      }
    | HEXADECIMAL   { $value = (uint32_t) std::strtoul($HEXADECIMAL.text.c_str()+1, nullptr, 16); }
    | CHARACTER     { $value = (uint32_t) $CHARACTER.text[1];                           }
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
    | SP
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
    | ID2
    ;

address returns [std::any value]
    : number     { $value = $number.value; }
    | ID2        { $value = $ID2.text; }
    | STAR	     { $value = '*'; }
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
aRegister : AREG | SP ;
aRegisterIndirect : OPENPAREN (AREG | SP) CLOSEPAREN ;
aRegisterIndirectPostIncrement : OPENPAREN  (AREG | SP)  CLOSEPAREN  PLUS ;
aRegisterIndirectPreDecrement : DASH  OPENPAREN  (AREG | SP)  CLOSEPAREN ;
aRegisterIndirectDisplacement
    : number  OPENPAREN  (AREG | SP)  CLOSEPAREN            #aRegisterIndirectDisplacementOld
    | OPENPAREN number COMMA (AREG | SP) CLOSEPAREN         #aRegisterIndirectDisplacementNew
    ;
aRegisterIndirectIndex
    : number  OPENPAREN  (AREG | SP)  COMMA  adRegisterSize  CLOSEPAREN        #aRegisterIndirectIndexOld
    | OPENPAREN number  COMMA (AREG | SP)  COMMA  adRegisterSize  CLOSEPAREN   #aRegisterIndirectIndexNew
    ;
absolute : address ;
absoluteShort : address SIZEWORD;
absoluteLong : address SIZELONG;
pcIndirectDisplacement : number  OPENPAREN  PC  CLOSEPAREN ;
pcIndirectIndex : number  OPENPAREN  PC  COMMA  adRegisterSize  CLOSEPAREN ;
immediateData : HASH address ;

// ============================================
// Directives
//=============================================
directiveSection
    : directive 
    ; 

directive
    : dc
    | DCB size?
    | ds
    | END  address
    | equ
    | FAIL
    | INCLUDE
    | INCBIN
    | LIST
    | NOLIST
    | memory
    | OPT identifiers
    | org
    | PAGE
    | REG
    | SECTION
    | SET
    | SIMHALT
    ;

dc  : DC size  dataList ;
equ : EQU expression ;
ds  : DS size? address ;
memory
    : MEMORY blockAddress COMMA blockAddress
    ;
org : ORG blockAddress ;

// ============================================
// Directivess arguments
//=============================================
dataList
    : expression (COMMA expression)*
    ;

expression
    : additiveExpr
    ;

additiveExpr
    : primaryExpr ((PLUS | DASH) primaryExpr)*
    ;

primaryExpr
    : number                           #dleNumber
    | STRING                           #dleString
    | ID2                              #dleIdentifier
    | STAR                             #dleStar
    | OPENPAREN expression CLOSEPAREN  #dleExpression
    ;

identifiers
    : ID2 (COMMA ID2)*
    ;

blockAddress returns [std::any value]
    : OCTAL         { $value = std::stol($OCTAL.text.c_str()+1, nullptr, 8);        }
    | DECIMAL       { $value = std::stol($DECIMAL.text.c_str(), nullptr, 10);       }
    | HEXADECIMAL   { $value = std::stol($HEXADECIMAL.text.c_str()+1, nullptr, 16); }
    | ID2           { $value = $ID2.text; }
    ;