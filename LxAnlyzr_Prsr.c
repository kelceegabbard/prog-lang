/*
This is a simple lexical analyzer and parser for arithmetic expressions. Input is taken from 'input.in' and outputs to 'output.txt'.

1. Syntax grammar: This program uses a simple BNF grammar to parse arithmetic expressions.
    The grammar in this program is not explicitly written in formal notation like BNF or EBNF but follows similar principles internally. 
    The functions 'expr()', 'term()', and 'factor()' handle the grammar rules implicitly:
        'expr()' follows: '<expr> -> <term> {(+ | -) <term>}'
        'term()' follows: '<term> -> <factor> {(* | /) <factor>}'
        'factor()' follows: '<factor> -> id | int_literal | ( <expr> )'
    
2. This program follows a POP (Procedure-Oriented Programming) approach. 
    It is divided into procedures (or functions) that perform specific tasks in sequence to achieve the parsing functionality. 
    Procedures in the implementation:
        'getChar()': Gets the next character from the input and determines its character class. 
        'addChar()': Adds the current character to the lexeme being analyzed.
        'lookup()': Looks up operators and parentheses to return the corresponding token.
        'getNonBlank()': Skips blank space characters in the input. 
        'lex()': The core lexical analyzer that determines the token type based on the character class. 
        'expr()', 'term()', 'factor()': Procedures to parse expressions, terms, and factors according to the implicit grammar. 

3. Our variables and functions follow standard naming conventions.
    The program adheres to standard naming conventions for variables, functions, and constants to ensure readability and consistency.
        Variables: Named in camel case, (e.g., 'charClass', 'lexeme', 'nextChar'). 
        Functions: Named with verbs and in camel case, (e.g., 'getChar()', 'addChar()', 'lex()'). 
        Constants: 
            'LETTER', 'DIGIT', 'UNKNOWN': Constants for character classes.
            'INT_LIT', 'IDENT', 'ASSIGN_OP', 'ADD_OP', 'SUB_OP', 'MULT_OP', 'DIV_OP', 'LEFT_PAREN', 'RIGHT_PAREN': Constants for token types.
*/

#include <stdio.h> 
#include <ctype.h>

/* Global declarations */
int charClass; /* Class of the current character */
char lexeme[100]; /* Current lexeme being analyzed */ 
char nextChar; /* Next character to input */
int lexLen; /* Length of lexeme[] */
int token; /* Current token */
int nextToken; /* next token  */
FILE *in_fp, *fopen(), *out_fp; /* input and output files */

/* function declarations */

void getChar();
void addChar();
int lookup(char ch);
void getNonBlank();
int lex();
void expr();
void term();
void factor();

/* character classes */
#define LETTER 0
#define DIGIT 1
#define UNKNOWN 99

/* token codes */
#define INT_LIT 10
#define IDENT 11
#define ASSIGN_OP 20
#define ADD_OP 21
#define SUB_OP 22
#define MULT_OP 23
#define DIV_OP 24
#define LEFT_PAREN 25
#define RIGHT_PAREN 26

int indent = 0; /* For indentation in output */

/***********************************/
/* MAIN DRIVER */
int main() {
    /* open the input data file and proces its contents */
    if ((in_fp = fopen("input.in", "r")) == NULL)
        printf("ERROR - cannot open input.in \n");
    else {
        if ((out_fp = fopen("output.txt", "w")) == NULL) {
            printf("ERROR - cannot open output.txt\n");
            
            return 1; // Return an error code
        }
        getChar();
        do {
            lex(); 
            expr();
        } while (nextToken != EOF);
        // Close files
        return 0;
    }
}

/* lookup- a function to lookup operators and parentheses and return the token */
int lookup(char ch) {
    switch (ch) {
        case '(':
            addChar();
            nextToken = LEFT_PAREN;
            break;
        case ')':
            addChar();
            nextToken = RIGHT_PAREN;
            break;
        case '+':
            addChar();
            nextToken = ADD_OP;
            break;
        case '-':
            addChar();
            nextToken = SUB_OP;
            break;
        case '*':
            addChar();
            nextToken = MULT_OP;
            break;
        case '/':
            addChar();
            nextToken = DIV_OP;
            break;

        default:
            addChar();
            nextToken = EOF; 
            break; 
    }
    return nextToken; 
}

/*addChar - a function to add nextChar to lexeme */
void addChar() {

    if (lexLen <= 98) {
        lexeme[lexLen++] = nextChar;
        lexeme[lexLen] = 0;
    }
    else 
        printf("Error - a lexeme is too long \n");
}

/* getChar - a function to get the next character of input and determine its character class */
void getChar() {
    nextChar = getc(in_fp);

    if (nextChar != EOF) {
        // If nextChar is letter, belongs to LETTER charClass
        if (isalpha(nextChar))
            charClass = LETTER;
        // If nextChar is number, belongs to DIGIT charClass
        else if (isdigit(nextChar))
            charClass = DIGIT;
        // If nextChar is something else, belongs to UNKNOWN charClass
        else
            charClass = UNKNOWN;
    } else {
        charClass = EOF;
    }
}


/* getNonBlank - a function to call getChar until it returns a non-whitespace character */
void getNonBlank() {
    while (isspace(nextChar))
        getChar();
}


/* lex - a simple lexical analyzer for arithmetic expressions */
int lex() {
    lexLen = 0; 
    getNonBlank();
    switch (charClass) {
        /* parse identifiers */
        case LETTER:
            addChar();
            getChar();
            while (charClass == LETTER || charClass == DIGIT) {
                addChar();
                getChar(); 
            }
        nextToken = IDENT; 
        break;

        /*parse integer literals */
        case DIGIT:
            addChar();
            getChar(); 
            while (charClass == DIGIT) {
                addChar();
                getChar();
            }
        nextToken = INT_LIT;
        break; 

        /*parentheses and operators */
        case UNKNOWN:
            lookup(nextChar);
            getChar();
            break; 
        
        /*EOF*/
        case EOF:
            nextToken = EOF; 
            lexeme[0] = 'E';
            lexeme[1] = 'O';
            lexeme[2] = 'F';
            lexeme[3] = 0;
            break;
    } /* end of switch */
    //fprintf(out_fp,"Next token is: %d, Next lexeme is %s\n",nextToken, lexeme);
    
    return nextToken;
} /* end of function lex */

/* expr
Parses strings in the language generated by the rule:
<expr> -> <term> {(+ | -) <term>}
*/
void expr() {
    
    fprintf(out_fp, "%*s[expr\n", indent, "");
    indent += 4;
    /* Parse the first term */
    term();
    /* As long as the next token is + or -, get the next token and parse the next term */
    while (nextToken == ADD_OP || nextToken == SUB_OP) {
        fprintf(out_fp, "%*s[%s]\n", indent, "", lexeme);
        lex();
        term();
    }
    indent -= 4;
    fprintf(out_fp, "%*s]\n", indent, "");
    
    
}

/* term()
Parses strings in the language generated by the rule:
<term> -> <factor> {(* | /) <factor>}
*/
void term() {
    
    fprintf(out_fp, "%*s[term\n", indent, "");
    indent += 4;
    /* Parse the first factor */
    factor();
    /* As long as the next token is * or /, get the next token and parse the next factor */
    while (nextToken == MULT_OP || nextToken == DIV_OP) {
        fprintf(out_fp, "%*s[%s]\n", indent, "", lexeme);
        lex();
        factor();
    }
   indent -= 4;
   fprintf(out_fp, "%*s]\n", indent, "");

}

/* factor()
Parses strings in the lnaguage generated by the rule:
<factor> -> id | int_literal | ( <expr> )
*/
void factor() {
    
    fprintf(out_fp, "%*s[factor\n", indent, "");
    indent += 4;
    /* Determine which RHS */
    if (nextToken == IDENT || nextToken == INT_LIT) {
        /* Get the next token */
        fprintf(out_fp, "%*s[id [%s]]\n", indent, "", lexeme);
        lex();
    }
    /* If the RHS is ( <expr> ), call lex to pass over the left parenthesis, call expr, and check for the right parenthesis */
    else {
        if (nextToken == LEFT_PAREN) {
            fprintf(out_fp, "%*s[LEFT_PAREN]\n", indent, "");
            lex();
            expr();
            if (nextToken == RIGHT_PAREN) {
                fprintf(out_fp, "%*s[RIGHT_PAREN]\n", indent, "");
                lex();
            }
            else {
                fprintf(out_fp, "Error - missing right parenthesis\n");
            }
        }
        else {
            
            fprintf(out_fp, "Error - invalid factor\n");
        }
        
    } /* End of else */
    indent -= 4;
        fprintf(out_fp, "%*s]\n", indent, "");
}

