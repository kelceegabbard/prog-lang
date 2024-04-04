/*
This is a simple lexical analyzer and parser for arithmetic expressions. Input is taken from 'input.in' and outputs to 'output.txt'.

1. Syntax grammar: This program uses a simple BNF grammar to parse arithmetic expressions.
    The grammar follows BNF principles and eahc token in the expression is recognized according to these rules. 
    The operators, identifiers, integer literals, and parentheses all follow the BNF grammar principles. 
    
2. This program follows a POP (Procedure-Oriented Programming) approach. 
    Each function handles a different part of the parsing process, meaning the program consists of procedures for each process. 
    C language is also a representative of POP. 
    Our procedures include getChar(), addChar(), lookup(), getNonBlank(), lex(), expr(), term(), and factor(). 

3. Our variables and functions follow standard naming conventions.
    Varaible and functions are all in lower camel case.
    Constants are defined using ALL_CAPS. 

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

int indent = 0;

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

