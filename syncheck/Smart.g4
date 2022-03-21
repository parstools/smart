grammar Smart;

compilationUnit
	: class_+ EOF
	;

visibility
    : 'public'
    | 'internal'
    | 'protected'
    | 'private'
    ;

modifier
    : 'static'
    ;

class_
	: visibility? modifier? 'class' Identifier LeftBrace classMember* RightBrace
	;

classMember
   : visibility? modifier? ( declField | method | constructor)
   | destructor
   ;

declField
    : visibility? modifier? Const? typeSpecifier initFieldDeclarator Semicolon
    //^semantic error when is Const without initDeclarator
    ;

declLocal
    : modifier? Const? typeSpecifier initLocalDeclarator (Comma initLocalDeclarator)* Semicolon
    ;

declFor
    : typeSpecifier forDeclarator (Comma forDeclarator)*
    ;

forDeclarator
    :  Identifier Assign prec12
    ;


initFieldDeclarator
    : Identifier Assign prec12
    | Identifier
    ;

initLocalDeclarator
    : Identifier Assign prec14
    | Identifier
    ;

method
	: typeSpecifier methodDefBody
	;

methodDefBody
    : Identifier LeftParen formalParameters? RightParen compoundStatement
    ;


constructor
	: methodDefBody
	;

destructor
	: Tilde Identifier LeftParen RightParen compoundStatement
	;

formalParameters
	: parameterDeclarator (Comma parameterDeclarator)*
	;

actualParameters
	: prec12 (Comma prec12)*
	;

parameterDeclarator
    : typeSpecifier Identifier
    ;

typeSpecifier
    : typeSpecifier LeftBracket RightBracket
    | Identifier
    | Void
    ;

jumpStatement
    :   (Continue| Break)
    |   Return prec15?
    ;

expressionStatement
    :   prec15 Semicolon
    ;

newExpression
    : New Identifier LeftParen actualParameters? RightParen
    | New Identifier LeftBracket prec13 RightBracket
    ;

statement
    :   compoundStatement
    |   expressionStatement
    |   selectionStatement
    |   iterationStatement
    |   jumpStatement Semicolon
    ;

selectionStatement
    :   'if' '(' prec15 ')' statement ('else' statement)?
    |   'switch' '(' prec15 ')' statement
    ;

iterationStatement
    :   While '(' prec15 ')' statement
    |   Do statement While '(' prec15 ')' ';'
    |   For '(' forCondition ')' statement
    ;

forCondition
	:  (declFor|prec15)?  ';' prec12? ';' prec15
	;

compoundStatement
    :   LeftBrace blockItemList? RightBrace
    ;

blockItemList
    :   blockItem+
    ;

blockItem
    :   declLocal
    |   statement
    ;

atom
    : Identifier
    | constant
    | StringLiteral
    ;


//https://en.cppreference.com/w/c/language/operator_precedence

//some combinations
// will be denied not by syntex but semantics
prec1
    : prec1 '++'
    | prec1 '--'
    | prec1 '(' actualParameters? ')'
    | prec1 LeftBracket prec13 RightBracket
    | prec1 '.' Identifier
    | atom
    ;

prec2
    : '++' prec2
    | '--' prec2
    | '+' prec2
    | '-' prec2
    | '!' prec2
    | '~' prec2
    | '*' prec2
    | '&' prec1
    | 'sizeof' '(' prec2 ')'
    | prec1
    ;

prec3
    : prec3 '*' prec2
    | prec3 '/' prec2
    | prec3 '&' prec2
    | prec2
    ;

prec4
    : prec4 '+' prec3
    | prec4 '-' prec3
    | prec3
    ;

prec5
    : prec5 '<<' prec4
    | prec5 '>>' prec4
    | prec4
    ;

prec6
    : prec6 '<' prec5
    | prec6 '<=' prec5
    | prec6 '>' prec5
    | prec6 '>=' prec5
    | prec5
    ;

prec7
    : prec7 '==' prec6
    | prec7 '!=' prec6
    | prec6
    ;


prec8
    : prec8 '&' prec7
    | prec7
    ;

prec9
    : prec9 '^' prec8
    | prec8
    ;

prec10
    : prec10 '|' prec9
    | prec9
    ;

prec11
    : prec11 '&&' prec10
    | prec10
    ;

prec12
    : prec12 '||' prec11
    | prec11
    ;

prec13
    : prec12 '?' prec12 ':' prec12
    | prec12
    ;


prec14
    : prec1 assignmentOperator prec14
    | newExpression
    | prec13
    ;

assignmentOperator
    :   '=' | '+='| '-=' | '*=' | '/=' | '%='| '<<=' | '>>=' | '&=' | '^=' | '|='
    ;

prec15
    : prec15 ',' prec14
    | prec14
    ;

constant
    :   integerConstant
    |   floatingConstant
    //|   EnumerationConstant
    |   CharacterConstant
    ;

floatingConstant
    :   decimalFloatingConstant
    |   HexadecimalFloatingConstant
    ;

HexadecimalFloatingConstant
    :   HexadecimalPrefix (HexadecimalFractionalConstant | HexadecimalDigitSequence) BinaryExponentPart FloatingSuffix?
    ;

decimalFloatingConstant
    :   FractionalConstant ExponentPart? FloatingSuffix?
    |   DigitSequence ExponentPart FloatingSuffix?
    ;

integerConstant
    :   DecimalConstant IntegerSuffix?
    |   OctalConstant IntegerSuffix?
    |   HexadecimalConstant IntegerSuffix?
    |	BinaryConstant
    ;

LeftParen : '(';
RightParen : ')';
LeftBracket : '[';
RightBracket : ']';
LeftBrace : '{';
RightBrace : '}';
Tilde: '~';
Comma: ',';
Semicolon: ';';
Assign: '=';
New: 'new';
Void: 'void';
Const: 'const';
Continue: 'continue';
Break: 'break';
Return: 'return';
While: 'while';
Do: 'do';
For: 'for';

Identifier
    :   IdentifierNondigit
        (   IdentifierNondigit
        |   Digit
        )*
    ;

fragment
IdentifierNondigit
    :   Nondigit
    ;

fragment
Nondigit
    :   [a-zA-Z_]
    ;

fragment
Digit
    :   [0-9]
    ;

fragment
UniversalCharacterName
    :   '\\u' HexQuad
    |   '\\U' HexQuad HexQuad
    ;

fragment
HexQuad
    :   HexadecimalDigit HexadecimalDigit HexadecimalDigit HexadecimalDigit
    ;

DecimalConstant
    :   '0'
    | NonzeroDigit Digit*
    ;

DigitSequence
    :   Digit+
    ;


BinaryConstant
	:	'0b' [0-1]+
	;

OctalConstant
    :   '0' OctalDigit*
    ;

HexadecimalConstant
    :   HexadecimalPrefix HexadecimalDigit+
    ;

HexadecimalPrefix
    :   '0x'
    ;

fragment
NonzeroDigit
    :   [1-9]
    ;

fragment
OctalDigit
    :   [0-7]
    ;

fragment
HexadecimalDigit
    :   [0-9a-fA-F]
    ;

IntegerSuffix
    :   UnsignedSuffix LongSuffix?
    |   UnsignedSuffix LongLongSuffix
    |   LongSuffix UnsignedSuffix?
    |   LongLongSuffix UnsignedSuffix?
    ;

fragment
UnsignedSuffix
    :   [uU]
    ;

fragment
LongSuffix
    :   [lL]
    ;

fragment
LongLongSuffix
    :   'll' | 'LL'
    ;

FractionalConstant
    :   DigitSequence? '.' DigitSequence
    |   DigitSequence '.'
    ;

ExponentPart
    :   [eE] Sign? DigitSequence
    ;

fragment
Sign
    :   [+-]
    ;

fragment
HexadecimalFractionalConstant
    :   HexadecimalDigitSequence? '.' HexadecimalDigitSequence
    |   HexadecimalDigitSequence '.'
    ;

BinaryExponentPart
    :   [pP] Sign? DigitSequence
    ;

HexadecimalDigitSequence
    :   HexadecimalDigit+
    ;

FloatingSuffix
    :   [flFL]
    ;

CharacterConstant
    :   '\'' CCharSequence '\''
    |   'L\'' CCharSequence '\''
    |   'u\'' CCharSequence '\''
    |   'U\'' CCharSequence '\''
    ;

fragment
CCharSequence
    :   CChar+
    ;

fragment
CChar
    :   ~['\\\r\n]
    |   EscapeSequence
    ;

fragment
EscapeSequence
    :   SimpleEscapeSequence
    |   OctalEscapeSequence
    |   HexadecimalEscapeSequence
    |   UniversalCharacterName
    ;

fragment
SimpleEscapeSequence
    :   '\\' ['"?abfnrtv\\]
    ;

fragment
OctalEscapeSequence
    :   '\\' OctalDigit OctalDigit? OctalDigit?
    ;

fragment
HexadecimalEscapeSequence
    :   '\\x' HexadecimalDigit+
    ;

StringLiteral
    :   EncodingPrefix? '"' SCharSequence? '"'
    ;

fragment
EncodingPrefix
    :   'u8'
    |   'u'
    |   'U'
    |   'L'
    ;

fragment
SCharSequence
    :   SChar+
    ;

fragment
SChar
    :   ~["\\\r\n]
    |   EscapeSequence
    ;

Whitespace
    :   [ \t]+
        -> skip
    ;

Newline
    :   (   '\r' '\n'?
        |   '\n'
        )
        -> skip
    ;

BlockComment
    :   '/*' .*? '*/'
        -> skip
    ;

LineComment
    :   '//' ~[\r\n]*
        -> skip
    ;
