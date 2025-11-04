lexer grammar lexer68000;


SPACES  : [ \t]+                   -> mode(NORMAL);
LABEL   : [a-zA-Z_] [a-zA-Z0-9_]*  -> mode(NORMAL);
COMMENTLINE : '*' ~[\r\n]*         -> mode(NORMAL) ;

mode NORMAL;

//
// Instructions
//
ABCD    options { caseInsensitive=true; } : 'abcd' -> mode(ARGUMENT);
ADD     options { caseInsensitive=true; } : 'add' -> mode(ARGUMENT);
ADDA    options { caseInsensitive=true; } : 'adda' -> mode(ARGUMENT);
ADDI    options { caseInsensitive=true; } : 'addi' -> mode(ARGUMENT);
ADDQ    options { caseInsensitive=true; } : 'addq' -> mode(ARGUMENT);
ADDX    options { caseInsensitive=true; } : 'addx' -> mode(ARGUMENT);
AND     options { caseInsensitive=true; } : 'and' -> mode(ARGUMENT);
ANDI    options { caseInsensitive=true; } : 'andi' -> mode(ARGUMENT);
ASL     options { caseInsensitive=true; } : 'asl' -> mode(ARGUMENT);
ASR     options { caseInsensitive=true; } : 'asr' -> mode(ARGUMENT);
BCC     options { caseInsensitive=true; } : 'bcc' -> mode(ARGUMENT);
BCHG    options { caseInsensitive=true; } : 'bchg' -> mode(ARGUMENT);
BCLR    options { caseInsensitive=true; } : 'bclr' -> mode(ARGUMENT);
BCS     options { caseInsensitive=true; } : 'bcs' -> mode(ARGUMENT);
BEQ     options { caseInsensitive=true; } : 'beq' -> mode(ARGUMENT);
BGE     options { caseInsensitive=true; } : 'bge' -> mode(ARGUMENT);
BGT     options { caseInsensitive=true; } : 'bgt' -> mode(ARGUMENT);
BHI     options { caseInsensitive=true; } : 'bhi' -> mode(ARGUMENT);
BLE     options { caseInsensitive=true; } : 'ble' -> mode(ARGUMENT);
BLS     options { caseInsensitive=true; } : 'bls' -> mode(ARGUMENT);
BLT     options { caseInsensitive=true; } : 'blt' -> mode(ARGUMENT);
BMI     options { caseInsensitive=true; } : 'bmi' -> mode(ARGUMENT);
BNE     options { caseInsensitive=true; } : 'bne' -> mode(ARGUMENT);
BPL     options { caseInsensitive=true; } : 'bpl' -> mode(ARGUMENT);
BRA     options { caseInsensitive=true; } : 'bra' -> mode(ARGUMENT);
BSET    options { caseInsensitive=true; } : 'bset' -> mode(ARGUMENT);
BSR     options { caseInsensitive=true; } : 'bsr' -> mode(ARGUMENT);
BTST    options { caseInsensitive=true; } : 'btst' -> mode(ARGUMENT);
BVC     options { caseInsensitive=true; } : 'bvc' -> mode(ARGUMENT);
BVS     options { caseInsensitive=true; } : 'bvs' -> mode(ARGUMENT);
CHK     options { caseInsensitive=true; } : 'chk' -> mode(ARGUMENT);
CLR     options { caseInsensitive=true; } : 'clr' -> mode(ARGUMENT);
CMP     options { caseInsensitive=true; } : 'cmp' -> mode(ARGUMENT);
CMPA    options { caseInsensitive=true; } : 'cmpa' -> mode(ARGUMENT);
CMPI    options { caseInsensitive=true; } : 'cmpi' -> mode(ARGUMENT);
CMPM    options { caseInsensitive=true; } : 'cmpm' -> mode(ARGUMENT);

DBCC     options { caseInsensitive=true; } : 'dbcc' -> mode(ARGUMENT);
DBCS     options { caseInsensitive=true; } : 'dbcs' -> mode(ARGUMENT);
DBEQ     options { caseInsensitive=true; } : 'dbeq' -> mode(ARGUMENT);
DBF      options { caseInsensitive=true; } : 'dbf'  -> mode(ARGUMENT);
DBGE     options { caseInsensitive=true; } : 'dbge' -> mode(ARGUMENT);
DBGT     options { caseInsensitive=true; } : 'dbgt' -> mode(ARGUMENT);
DBHI     options { caseInsensitive=true; } : 'dbhi' -> mode(ARGUMENT);
DBLE     options { caseInsensitive=true; } : 'dble' -> mode(ARGUMENT);
DBLS     options { caseInsensitive=true; } : 'dbls' -> mode(ARGUMENT);
DBLT     options { caseInsensitive=true; } : 'dblt' -> mode(ARGUMENT);
DBMI     options { caseInsensitive=true; } : 'dbmi' -> mode(ARGUMENT);
DBNE     options { caseInsensitive=true; } : 'dbne' -> mode(ARGUMENT);
DBPL     options { caseInsensitive=true; } : 'dbpl' -> mode(ARGUMENT);
DBRA     options { caseInsensitive=true; } : 'dbra' -> mode(ARGUMENT);
DBT      options { caseInsensitive=true; } : 'dbt'  -> mode(ARGUMENT);
DBVC     options { caseInsensitive=true; } : 'dbvc' -> mode(ARGUMENT);
DBVS     options { caseInsensitive=true; } : 'dbvs' -> mode(ARGUMENT);

SCC     options { caseInsensitive=true; } : 'scc' -> mode(ARGUMENT);
SCS     options { caseInsensitive=true; } : 'scs' -> mode(ARGUMENT);
SEQ     options { caseInsensitive=true; } : 'seq' -> mode(ARGUMENT);
SF      options { caseInsensitive=true; } : 'sf'  -> mode(ARGUMENT);
SGE     options { caseInsensitive=true; } : 'sge' -> mode(ARGUMENT);
SGT     options { caseInsensitive=true; } : 'sgt' -> mode(ARGUMENT);
SHI     options { caseInsensitive=true; } : 'shi' -> mode(ARGUMENT);
SLE     options { caseInsensitive=true; } : 'sle' -> mode(ARGUMENT);
SLS     options { caseInsensitive=true; } : 'sls' -> mode(ARGUMENT);
SLT     options { caseInsensitive=true; } : 'slt' -> mode(ARGUMENT);
SMI     options { caseInsensitive=true; } : 'smi' -> mode(ARGUMENT);
SNE     options { caseInsensitive=true; } : 'sne' -> mode(ARGUMENT);
SPL     options { caseInsensitive=true; } : 'spl' -> mode(ARGUMENT);
SRA     options { caseInsensitive=true; } : 'sra' -> mode(ARGUMENT);
ST      options { caseInsensitive=true; } : 'st'  -> mode(ARGUMENT);
SVC     options { caseInsensitive=true; } : 'svc' -> mode(ARGUMENT);
SVS     options { caseInsensitive=true; } : 'svs' -> mode(ARGUMENT);

DIVS    options { caseInsensitive=true; } : 'divs' -> mode(ARGUMENT);
DIVU    options { caseInsensitive=true; } : 'divu' -> mode(ARGUMENT);
EOR     options { caseInsensitive=true; } : 'eor' -> mode(ARGUMENT);
EORI    options { caseInsensitive=true; } : 'eori' -> mode(ARGUMENT);
EXG     options { caseInsensitive=true; } : 'exg' -> mode(ARGUMENT);
EXT     options { caseInsensitive=true; } : 'ext' -> mode(ARGUMENT);
ILLEGAL options { caseInsensitive=true; } : 'illegal' -> mode(ARGUMENT);
JMP     options { caseInsensitive=true; } : 'jmp' -> mode(ARGUMENT);
JSR     options { caseInsensitive=true; } : 'jsr' -> mode(ARGUMENT);
LEA     options { caseInsensitive=true; } : 'lea' -> mode(ARGUMENT);
LINK    options { caseInsensitive=true; } : 'link' -> mode(ARGUMENT);
LSL     options { caseInsensitive=true; } : 'lsl' -> mode(ARGUMENT);
LSR     options { caseInsensitive=true; } : 'lsr' -> mode(ARGUMENT);
MOVE    options { caseInsensitive=true; } : 'move' -> mode(ARGUMENT);
MOVEA   options { caseInsensitive=true; } : 'movea' -> mode(ARGUMENT);
MOVEM   options { caseInsensitive=true; } : 'movem' -> mode(ARGUMENT);
MOVEP   options { caseInsensitive=true; } : 'movep' -> mode(ARGUMENT);
MOVEQ   options { caseInsensitive=true; } : 'moveq' -> mode(ARGUMENT);
MULS    options { caseInsensitive=true; } : 'muls' -> mode(ARGUMENT);
MULU    options { caseInsensitive=true; } : 'mulu' -> mode(ARGUMENT);
NBCD    options { caseInsensitive=true; } : 'nbcd' -> mode(ARGUMENT);
NEG     options { caseInsensitive=true; } : 'neg' -> mode(ARGUMENT);
NEGX    options { caseInsensitive=true; } : 'negx' -> mode(ARGUMENT);
NOP     options { caseInsensitive=true; } : 'nop' -> mode(ARGUMENT);
NOT     options { caseInsensitive=true; } : 'not' -> mode(ARGUMENT);
OR      options { caseInsensitive=true; } : 'or' -> mode(ARGUMENT);
ORI     options { caseInsensitive=true; } : 'ori' -> mode(ARGUMENT);
PEA     options { caseInsensitive=true; } : 'pea' -> mode(ARGUMENT);
RESET   options { caseInsensitive=true; } : 'reset' -> mode(ARGUMENT);
ROL     options { caseInsensitive=true; } : 'rol' -> mode(ARGUMENT);
ROR     options { caseInsensitive=true; } : 'ror' -> mode(ARGUMENT);
ROXL    options { caseInsensitive=true; } : 'roxl' -> mode(ARGUMENT);
ROXR    options { caseInsensitive=true; } : 'roxr' -> mode(ARGUMENT);
RTE     options { caseInsensitive=true; } : 'rte' -> mode(ARGUMENT);
RTR     options { caseInsensitive=true; } : 'rtr' -> mode(ARGUMENT);
RTS     options { caseInsensitive=true; } : 'rts' -> mode(ARGUMENT);
SBCD    options { caseInsensitive=true; } : 'sbcd' -> mode(ARGUMENT);
STOP    options { caseInsensitive=true; } : 'stop' -> mode(ARGUMENT);
SUB     options { caseInsensitive=true; } : 'sub' -> mode(ARGUMENT);
SUBA    options { caseInsensitive=true; } : 'suba' -> mode(ARGUMENT);
SUBI    options { caseInsensitive=true; } : 'subi' -> mode(ARGUMENT);
SUBQ    options { caseInsensitive=true; } : 'subq' -> mode(ARGUMENT);
SUBX    options { caseInsensitive=true; } : 'subx' -> mode(ARGUMENT);
SWAP    options { caseInsensitive=true; } : 'swap' -> mode(ARGUMENT);
TAS     options { caseInsensitive=true; } : 'tas' -> mode(ARGUMENT);
TRAP    options { caseInsensitive=true; } : 'trap' -> mode(ARGUMENT);
TRAPV   options { caseInsensitive=true; } : 'trapv' -> mode(ARGUMENT);
TST     options { caseInsensitive=true; } : 'tst' -> mode(ARGUMENT);
UNLK    options { caseInsensitive=true; } : 'unlk' -> mode(ARGUMENT);

//
// Directives
//
DC      options { caseInsensitive=true; } : 'dc' -> mode(ARGUMENT);
DCB     options { caseInsensitive=true; } : 'dcb' -> mode(ARGUMENT);
DS      options { caseInsensitive=true; } : 'ds' -> mode(ARGUMENT);
END     options { caseInsensitive=true; } : 'end' -> mode(ARGUMENT);
EQU     options { caseInsensitive=true; } : 'equ' -> mode(ARGUMENT);
FAIL    options { caseInsensitive=true; } : 'fail' -> mode(ARGUMENT);
INCLUDE options { caseInsensitive=true; } : 'include' -> mode(ARGUMENT);
INCBIN  options { caseInsensitive=true; } : 'incbin' -> mode(ARGUMENT);
LIST    options { caseInsensitive=true; } : 'list' -> mode(ARGUMENT);
NOLIST  options { caseInsensitive=true; } : 'nolist' -> mode(ARGUMENT);
MEMORY  options { caseInsensitive=true; } : 'memory' -> mode(ARGUMENT);
OPT     options { caseInsensitive=true; } : 'opt' -> mode(ARGUMENT);
ORG     options { caseInsensitive=true; } : 'org' -> mode(ARGUMENT);
PAGE    options { caseInsensitive=true; } : 'page' -> mode(ARGUMENT);
REG     options { caseInsensitive=true; } : 'reg' -> mode(ARGUMENT);
SECTION options { caseInsensitive=true; } : 'section' -> mode(ARGUMENT);
SET     options { caseInsensitive=true; } : 'set' -> mode(ARGUMENT);
SIMHALT options { caseInsensitive=true; } : 'simhalt' -> mode(ARGUMENT);

//
// Punctuation
//
EOL         : [\r\n]+ -> mode(DEFAULT_MODE);
WS          : [ \t]+  -> skip;
SEMICOLON   : ':' ;

//
// Identifiers
//
ID  : [a-zA-Z_] [a-zA-Z0-9_]* ;

//------------------------------------------------------
mode ARGUMENT;
//------------------------------------------------------
//
// Size specifiers
//
SIZEBYTE    options { caseInsensitive=true; } : '.b' ;
SIZEWORD    options { caseInsensitive=true; } : '.w' ;
SIZELONG    options { caseInsensitive=true; } : '.l' ;

//
// Numbers
//
OCTAL       : '@' [0-7]+       ;
DECIMAL     : '-'?[0-9]+       ;
HEXADECIMAL : '$' [0-9a-fA-F]+ ;

//
// Registers
//
DREG : [dD][0-7] ;
AREG : [aA][0-7] ;
SP options  { caseInsensitive=true; } : 'sp'  ;
PC options  { caseInsensitive=true; } : 'pc'  ;
CCR options { caseInsensitive=true; } : 'ccr' ;
SR options  { caseInsensitive=true; } : 'sr'  ;
USP options { caseInsensitive=true; } : 'usp' ;

//
// Identifiers
//
ID2  : [a-zA-Z_] [a-zA-Z0-9_]* ;

//
// Strings
//
CHARACTER
    : '\'' ( ~['\r\n] | '\'\'' ) '\''
    ;

STRING
    : '\'' ( ~['\r\n] | '\'\'' )* '\''
    ;

COMMENT     : ';' ~[\r\n]* -> mode(NORMAL);

//
// Punctuation
//
EOL2         : [\r\n]+ -> mode(DEFAULT_MODE);
WS2          : [ \t]+  -> skip;
HASH        : '#' ;
COMMA       : ',' ;
DASH        : '-' ;
PLUS        : '+' ;
SLASH       : '/' ;
OPENPAREN   : '(' ;
CLOSEPAREN  : ')' ;
STAR        : '*' ;


