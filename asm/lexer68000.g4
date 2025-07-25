lexer grammar lexer68000;


SPACES  : [ \t]+                   -> mode(NORMAL);
LABEL   : [a-zA-Z_] [a-zA-Z0-9_]*  -> mode(NORMAL);
COMMENTLINE : '*' ~[\r\n]*         -> mode(NORMAL) ;

mode NORMAL;



//
// Instructions
//
ABCD    options { caseInsensitive=true; } : 'abcd' ;
ADD     options { caseInsensitive=true; } : 'add' ;
ADDA    options { caseInsensitive=true; } : 'adda' ;
ADDI    options { caseInsensitive=true; } : 'addi' ;
ADDQ    options { caseInsensitive=true; } : 'addq' ;
ADDX    options { caseInsensitive=true; } : 'addx' ;
AND     options { caseInsensitive=true; } : 'and' ;
ANDI    options { caseInsensitive=true; } : 'andi' ;
ASL     options { caseInsensitive=true; } : 'asl' ;
BCC     options { caseInsensitive=true; } : 'bcc' ;
BCHG    options { caseInsensitive=true; } : 'bchg' ;
BCLR    options { caseInsensitive=true; } : 'bclr' ;
BCS     options { caseInsensitive=true; } : 'bcs' ;
BEQ     options { caseInsensitive=true; } : 'beq' ;
BGE     options { caseInsensitive=true; } : 'bge' ;
BGT     options { caseInsensitive=true; } : 'bgt' ;
BHI     options { caseInsensitive=true; } : 'bhi' ;
BLE     options { caseInsensitive=true; } : 'ble' ;
BLS     options { caseInsensitive=true; } : 'bls' ;
BLT     options { caseInsensitive=true; } : 'blt' ;
BMI     options { caseInsensitive=true; } : 'bmi' ;
BNE     options { caseInsensitive=true; } : 'bne' ;
BPL     options { caseInsensitive=true; } : 'bpl' ;
BRA     options { caseInsensitive=true; } : 'bra' ;
BSET    options { caseInsensitive=true; } : 'bset' ;
BSR     options { caseInsensitive=true; } : 'bsr' ;
BTST    options { caseInsensitive=true; } : 'btst' ;
BVC     options { caseInsensitive=true; } : 'bvc' ;
BVS     options { caseInsensitive=true; } : 'bvs' ;
CHK     options { caseInsensitive=true; } : 'chk' ;
CLR     options { caseInsensitive=true; } : 'clr' ;
CMP     options { caseInsensitive=true; } : 'cmp' ;
CMPA    options { caseInsensitive=true; } : 'cmpa' ;
CMPI    options { caseInsensitive=true; } : 'cmpi' ;
CMPM    options { caseInsensitive=true; } : 'cmpm' ;
DBCC    options { caseInsensitive=true; } : 'dbcc' ;
DIVS    options { caseInsensitive=true; } : 'divs' ;
DIVU    options { caseInsensitive=true; } : 'divu' ;
EOR     options { caseInsensitive=true; } : 'eor' ;
EORI    options { caseInsensitive=true; } : 'eori' ;
EXG     options { caseInsensitive=true; } : 'exg' ;
EXT     options { caseInsensitive=true; } : 'ext' ;
ILLEGAL options { caseInsensitive=true; } : 'illegal' ;
JMP     options { caseInsensitive=true; } : 'jmp' ;
JSR     options { caseInsensitive=true; } : 'jsr' ;
LEA     options { caseInsensitive=true; } : 'lea' ;
LINK    options { caseInsensitive=true; } : 'link' ;
LSL     options { caseInsensitive=true; } : 'lsl' ;
LSR     options { caseInsensitive=true; } : 'lsr' ;
MOVE    options { caseInsensitive=true; } : 'move' ;
MOVEA   options { caseInsensitive=true; } : 'movea' ;
MOVEM   options { caseInsensitive=true; } : 'movem' ;
MOVEP   options { caseInsensitive=true; } : 'movep' ;
MOVEQ   options { caseInsensitive=true; } : 'moveq' ;
MULS    options { caseInsensitive=true; } : 'muls' ;
MULU    options { caseInsensitive=true; } : 'mulu' ;
NBCD    options { caseInsensitive=true; } : 'nbcd' ;
NEG     options { caseInsensitive=true; } : 'neg' ;
NEGX    options { caseInsensitive=true; } : 'negx' ;
NOP     options { caseInsensitive=true; } : 'nop' ;
NOT     options { caseInsensitive=true; } : 'not' ;
OR      options { caseInsensitive=true; } : 'or' ;
ORI     options { caseInsensitive=true; } : 'ori' ;
PEA     options { caseInsensitive=true; } : 'pea' ;
ROL     options { caseInsensitive=true; } : 'rol' ;
ROR     options { caseInsensitive=true; } : 'ror' ;
ROXL    options { caseInsensitive=true; } : 'roxl' ;
ROXR    options { caseInsensitive=true; } : 'roxr' ;
RTE     options { caseInsensitive=true; } : 'rte' ;
RTR     options { caseInsensitive=true; } : 'rtr' ;
RTS     options { caseInsensitive=true; } : 'rts' ;
SBCD    options { caseInsensitive=true; } : 'sbcd' ;
SCC     options { caseInsensitive=true; } : 'scc' ;
STOP    options { caseInsensitive=true; } : 'stop' ;
SUB     options { caseInsensitive=true; } : 'sub' ;
SUBA    options { caseInsensitive=true; } : 'suba' ;
SUBI    options { caseInsensitive=true; } : 'subi' ;
SUBQ    options { caseInsensitive=true; } : 'subq' ;
SUBX    options { caseInsensitive=true; } : 'subx' ;
SWAP    options { caseInsensitive=true; } : 'swap' ;
TAS     options { caseInsensitive=true; } : 'tas' ;
TRAP    options { caseInsensitive=true; } : 'trap' ;
TRAPV   options { caseInsensitive=true; } : 'trapv' ;
TST     options { caseInsensitive=true; } : 'tst' ;
UNLK    options { caseInsensitive=true; } : 'unlk' ;

//
// Size specifiers
//
SIZEBYTE    options { caseInsensitive=true; } : '.b' ;
SIZEWORD    options { caseInsensitive=true; } : '.w' ;
SIZELONG    options { caseInsensitive=true; } : '.l' ;

//
// Directives
//
DC      options { caseInsensitive=true; } : 'dc' ;
DCB     options { caseInsensitive=true; } : 'dcb' ;
DS      options { caseInsensitive=true; } : 'ds' ;
END     options { caseInsensitive=true; } : 'end' ;
EQU     options { caseInsensitive=true; } : 'equ' ;
FAIL    options { caseInsensitive=true; } : 'fail' ;
INCLUDE options { caseInsensitive=true; } : 'include' ;
INCBIN  options { caseInsensitive=true; } : 'incbin' ;
LIST    options { caseInsensitive=true; } : 'list' ;
NOLIST  options { caseInsensitive=true; } : 'nolist' ;
MEMORY  options { caseInsensitive=true; } : 'memory' ;
OPT     options { caseInsensitive=true; } : 'opt' ;
ORG     options { caseInsensitive=true; } : 'org' ;
PAGE    options { caseInsensitive=true; } : 'page' ;
REG     options { caseInsensitive=true; } : 'reg' ;
SECTION options { caseInsensitive=true; } : 'section' ;
SET     options { caseInsensitive=true; } : 'set' ;
SIMHALT options { caseInsensitive=true; } : 'simhalt' ;

//
// Numbers
//
OCTAL       : '@' [0-7]+   ;//           { setText(Integer.toString(Integer.parseInt(getText().substring(1), 8)));  };
DECIMAL     : '-'?[0-9]+       ;//           { setText(Integer.toString(Integer.parseInt(getText())));                  };
HEXADECIMAL : '$' [0-9a-fA-F]+ ;//        { setText(Integer.toString(Integer.parseInt(getText().substring(1), 16))); };

//
// Registers
//
DREG : [dD][0-7] ;
AREG : [aA][0-7] ;
SP options  { caseInsensitive=true; } : 'sp' ;
PC options  { caseInsensitive=true; } : 'pc' ;
CCR options { caseInsensitive=true; } : 'ccr' ;
SR options  { caseInsensitive=true; } : 'sr' ;

//
// Identifiers
//
ID  : [a-zA-Z_] [a-zA-Z0-9_]* {printf("ID\n"); } ;

//
// Strings
//
STRING  : '\u0027' ~['\r\n]* '\u0027' ;

COMMENT     : ';' ~[\r\n]* ;


//
// Punctuation
//
EOL         : [\r\n]+ -> mode(DEFAULT_MODE);
WS          : [ \t]+  -> skip;
SEMICOLON   : ':' ;
HASH        : '#' ;
COMMA       : ',' ;
DASH        : '-' ;
PLUS        : '+' ;
SLASH       : '/' ;
OPENPAREN   : '(' ;
CLOSEPAREN  : ')' ;


