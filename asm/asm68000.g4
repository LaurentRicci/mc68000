grammar asm68000;

INT : [0-9]+ ;
WS : [ \r\t\n]+ -> skip ;

prog : INT * ;