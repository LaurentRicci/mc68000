lexer grammar lexer68000;

COMMENTLINE
    : '*' ~[\r\n]* 
    ;

ABCD options { caseInsensitive=true; } : 'abcd' ;
ADD  options { caseInsensitive=true; } : 'add'  ;

ID  : [a-zA-Z_] [a-zA-Z0-9_]* ;

EOL : [\r\n]+ ;

WS  : [ \t]+ ;

SEMICOLON : ':' ;