/**************************************************************************
 *      calc: a command-line calculator program                           *
 *      Copyright (c) 1988 Wayne Mesard                                   *
 *                                                                        *
 *      This is free software.  It may be reproduced, retransmitted,      *
 *      redistributed and otherwise propogated at will, provided that     *
 *      this notice remains intact and in place.                          *
 *                                                                        *
 *      Please direct bug reports, code enhancements and comments         *
 *      to mesard@BBN.COM.                                                *
 *                                                                        *
 **************************************************************************/

/**************************************************************************
 *                                                                        *
 *      changed into a linkable file by Andrew Houghton on 4/6/95         *
 *      I decided it wasn't worth my time to change the iffy algorithm -  *
 *      see the comments for 'calc()', below.                             *
 *                                                                        *
 *      changed getwhitlessline() to read from a string, too.             *
 *                                                                        *
 **************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <setjmp.h>

#include <values.h>

#define PIdiv180 0.017453292519943295
#define TwoToTheNLessOne    HIBITL

#define OP "{(["
#define CP "})]"
#define SYMBOLS "{([+-/xX*%:&^|<>~gnpst" /* Used to distinguish unary from
					binary minus.  Letters at end of 
					string are ends of function names. */

#define SYMBOL_LOWERS "xabcdef"     /* Used to detect implied multiplication.
				     * Identifies these as symbols and not
				     * part of a function name.
				     */

#define DECFMT "\t% .16g\n"
#define BINFMT "\tb%*s\n"
#define FLTFMT "\t% .*f\n"
#define HEXFMT "\th%lX\n"
#define OCTFMT "\to%lo\n"
#define ASCFMT "\t@%c\n"

extern jmp_buf err_recover;
int abort_on_err = 0;
double answer = 0.0;

/* Recursively, parse an equation string.
 * This is hideously inefficient, since findop() is called on each invokation.
 * O(n) would be possible if findop() where modified to walk through the string
 * once and build a priority queue for evaluation of operators.
 *    But hey, the kids love it, and I know for a fact that my Data Structures
 * prof never wrote a linear algorithm in his life:-)
 */

double calc(char *eqn)
{
    double  vleft, temp;
    long    tempi;
    char    left[MAXLINE], eqncp[MAXLINE];
    char    *opptr;
    
    while (*eqn == ' ' || *eqn == '\t')
	eqn++;
    
    if (!*eqn)
	error("missing expression");

    else if (opptr = findop(eqn)) {
	strncpy (left, eqn, opptr - eqn);
	left[opptr - eqn] = '\0';
	vleft = calc(left);
	switch (*opptr) {
	    case '+': 
		return(vleft + calc(++opptr));
	    case '-': 
		return(vleft - calc(++opptr));
	    case '/': 
		if ((temp = calc(++opptr)) == 0.0)
		    error ("division by zero");
		else
		    return(vleft / temp);
	    case 'x': 
	    case 'X': 
	    case '*': 
		return(vleft * calc(++opptr));
	    case '%': 
		if ((temp = fabs(floor (calc(++opptr)+0.5))) == 0.0 ||
			    temp > (TwoToTheNLessOne-1))
		    error("bad argument to modulo");
		return((double)((long) (floor (vleft) + 0.5) %
		                (long) temp));
	    case ':': 
		return(pow(vleft, calc(++opptr)));
	    case '&': 
		return((double)((unsigned long)vleft &
				(unsigned long)calc(++opptr)));
	    case '^': 
		return((double)((unsigned long)vleft ^
				(unsigned long)calc(++opptr)));
	    case '|': 
		return((double)((unsigned long)vleft |
				(unsigned long)calc(++opptr)));
	    case '<': 
		return((double)((unsigned long)vleft <<
				(unsigned long)calc(++opptr)));
	    case '>': 
		return((double)((unsigned long)vleft >>
				(unsigned long)calc(++opptr)));

	    default: /* implied multiplication */
		return(vleft * calc(opptr));
	    }
	}

	else if (index (OP, *eqn)) {
	    strcpy(eqncp, ++eqn);
	    eqncp[strlen (eqncp) - 1] = '\0';
	    return(calc (eqncp));
	}

	else if (*eqn == '+')
	    return(calc(eqn+1));
	else if (*eqn == '-')
	    return(-1.0 * calc(eqn+1));
        else if (*eqn == '~')
	    return((double)(~ (unsigned long)calc(eqn+1)));

	else if (strncmp(eqn, "sin", 3) == 0)
	    return(sin (calc(eqn+3) * PIdiv180));
	else if (strncmp(eqn, "cos", 3) == 0)
	    return(cos (calc(eqn+3) * PIdiv180));
	else if (strncmp(eqn, "tan", 3) == 0)
	    return(tan (calc(eqn+3) * PIdiv180));
	else if (strncmp(eqn, "atan", 4) == 0)
	    return(atan (calc(eqn+4)) / PIdiv180);

	else if (strncmp(eqn, "sqrt", 4) == 0)
	    return(sqrt (calc(eqn+4)));
	else if (strncmp(eqn, "log", 3) == 0)
	    return(log10 (calc(eqn+3)));
	else if (strncmp(eqn, "ln", 2) == 0)
	    return(log (calc(eqn+2)));
	else if (strncmp(eqn, "exp", 3) == 0)
	    return(exp (calc(eqn+3)));
	else if (strncmp(eqn, "pi", 2) == 0 || strncmp(eqn, "PI", 2) == 0)
	    return(PI);
	else if (strncmp(eqn, "prev", 4) == 0)
	    return(answer);

	else if (*eqn == 'h') {
	    sscanf(eqn+1, "%lx", &tempi);
	    return((double) tempi);
	}
	else if (*eqn == 'o') {
	    sscanf(eqn+1, "%lo", &tempi);
	    return((double) tempi);
	}
	else if (*eqn == 'b')
	    return(btof(eqn+1));
        else if (*eqn == '@')
	    return((double) *(eqn+1));
	else if (!(isdigit(*eqn) || *eqn == '.') )
	    error("illegal expression");
	else
	    return(atof (eqn));
}



/*
 * Takes a parenthesized expression and returns a pointer to the closing paren.
 */

char *findclose(char *s)
{
    register int lev = 0;

    for (; *s && !(lev==1 && index(CP, *s)); s++)
	if (index(OP, *s))
	    lev++;
	else if (index(CP, *s))
	    lev--;

    if (!*s)
	error("unmatched open paren");
    else
	return(s);
}


/** Precedence levels for binary operators **/

#define OPTYPE int
#define NONOP  0
#define NULLOP 1
#define EXP    3
#define MULT   5
#define DIV    5
#define MOD    6
#define ADD    7
#define SUBTR  7
#define LSHIFT 8
#define RSHIFT 8
#define BAND   9
#define BXOR   10
#define BOR    11

char *findop(char *s)
{
    register OPTYPE op;
    OPTYPE bestop = NULLOP;
    char *bestptr = 0;
    register char last = '\0';
    
    while (*s) {
	op = NONOP;
/* Don't let lasts get assigned to space */
	if (*s == ' ' || *s == '\t') { 
	    s++;
	    continue;
	}
	else {
	    switch (*s) {
	    case ':': 
		op = EXP;
		break;
	    case '%':
		op = MOD;
		break;
	    case 'x': 
	    case 'X': 
	    case '*': 
		if (!(*s=='x' && last=='e' && *(s+1)=='p')) /* exp() function */
		    op = MULT;
		break;
	    case '/': 
		op = DIV;
		break;
	    case '+': 
		/* "+" means unary plus (not add) if it follows a
		 * symbol or a function name.
		 */
		if (!index(SYMBOLS, last))
		    op = ADD;
		break;
	    case '-': 
		/* "-" means unary minus (not subtract) if it follows a
		 * symbol or a function name.
		 */
		if (!index(SYMBOLS, last))
		    op = SUBTR;
		break;
	    case '<':
		op = LSHIFT;
		break;
	    case '>':
		op = RSHIFT;
		break;
	    case '&':
		op = BAND;
		break;
	    case '^':
		op = BXOR;
		break;
	    case '|':
		op = BOR;
		break;
	    default:
		/* Implied multiplication occurs when a digit or a
		 * close paren is followed by a func-call, or an open
		 * paren.  The check for "co" and "at" is to distinguish
		 * 'c' and 'a' as hex digits and their appearance in
		 * "cos" and "atan".
		 */
		if ((last && (isdigit(last) || index(CP, last))) && 
		    ((islower(*s)  || index(OP, *s)) ||
		     (!isdigit(last) && isdigit(*s))) &&
		    (!index(SYMBOL_LOWERS, *s) ||
		     !strncmp("co", s, 2) || !strncmp("at", s, 2)))
		    op = MULT;
	    }
	    
	    if (op >= bestop) {
		bestop = op;
		bestptr = s;
	    }
	}
	
	if (index(OP, *s))
	    s = findclose(s);
	
	last = *s++;
    }
    return(bestptr);
}



/*
 * Places a binary representation of "val" in the string "s" and returns
 * a pointer to the start of that string.  "val" should be (or will be coerced
 * to )an integer between +/- 2^n, where n is the number of bits in a long int.
 */

char *ftob(char *s, double val)
{
    unsigned long lval = (unsigned long) (val < 0.0 ? -val : val);
    unsigned long i;
    char *s0 = s;
    
    if (lval == 0)
	*s++ = '0';
    else 
	for (i = TwoToTheNLessOne; i; i>>=1)
	    if (lval & i)
		*s++ = '1';
	    else {
		*s = '0';
		if (s != s0)
		    s++;
	    }
    
    *s = '\0';
    return(s0);
}



/*
 * Takes a string containing a binary number and returns its 
 * decimal equivalant.
 */

double btof(char *s)
{
    unsigned long i, val = 0;
    
    for (i = (unsigned long)1<<(strlen(s)-1); i; i>>=1, s++)
	if (*s == '1')
	    val |= i;
	else if (*s != '0')
	    error("bad binary digit");
    
    
    return((double) val);
}

int getwhitlessline(char *s, char *in)
{
    register int i;
    char c;

    for (i = 0; ((c = *in++) != '\n') && (c!=EOF); i+=((c!=' ') && (c!='\t')))
	s[i] = c;
    s[i] = '\0';
    return(i);
}



/*
 * Displays an error message and exits unless a jmp_buf has been
 * set to return to in just such emergencies.  (Capt. Kirk always
 * defined a jmp_buf.)
 */

void error(char *msg)
{
    printf("calc: error--%s\n", msg);
    if (abort_on_err)
	exit(1);
    else
	longjmp(err_recover, 0);
}

