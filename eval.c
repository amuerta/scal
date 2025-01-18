#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define HCH_PARSER_IMPLEMENTATION
#define HCH_TOKENIZER_IMPLEMENTATION
#include "tokenizer.h"

#define PARSER_ERRSTACK_SZ 1024

#define DEBUG

#ifdef DEBUG
#define debug printf
#endif

#ifndef DEBUG
#define debug //
#endif

// TODO: use this


float eval	(Parser* p);

float expr	(Parser* p);
float term	(Parser* p);
float factor(Parser* p);


static bool USE_EXPR = false;


#if 1
float parse(char* source, bool* had_error) {
	//char* source = "1+2+3+4+5+6+7";
	//char* source = "1+(2*3)*4";
	//char* source = "1+2+(1+(3*4)-1)+4";
	//char* source = "10.5+2+(1+3*4+1)+4";
	//char* source = "1+2+(1+3*4+1)+4";
	float result = 0;
	char* scratch_buffer = malloc(1024);

	Tokenizer t = {
		.source 	= source,
		.src_len 	= strlen(source),
		.position	= 0
	};

	Parser p = {0};

	TokenDefinition dict[] = {
		{"-",  "AddOp"},
		{"+",  "AddOp"},
		{"*",  "MulOp"},
		{"/",  "MulOp"},
		{"(",  "ParenOpen"},
		{")",  "ParenClose"},
	};
	
	tokenize(&t,dict,6,'\'');
	parser_begin(&p,t,true, scratch_buffer);

	for (uint i = 0; i < t.token_count; i++) {
		Token tk = t.tokens[i];
		char* as_str = tkn_get_token_cstr(tk,p.scratch_buffer);
		debug("{%s}  ",as_str);
	}

	debug("\n position -> %lu",p.position);
	debug("\n count 	-> %lu",p.lexer.token_count);
	result =  eval(&p);
	debug("\n eval() => N : %f\n",result);

	parser_reset(&p);
	debug("\n expr() => N : %f\n", expr(&p));


	char* errors = parser_get_errors(&p);
	debug("%s\n",errors);

	*had_error = (strlen(errors) > 0);

	tokenizer_clear(&t);
	parser_clear(&p,true);

	t = (Tokenizer){0};
	p = (Parser){0};

	return result;
}

#endif

void lookup_next(Parser* p, float* vptr) {
		float value = *vptr;
	
		Token t = parser_get_token(p);

		if (tkn_eq_text(t,"+"))
		{
			parser_expect_text(p,"+");
			value += expr(p); 
		} 
		else if (tkn_eq_text(t,"-"))
		{
			parser_expect_text(p,"-");
			value -= expr(p); 
		}
		else if (tkn_eq_text(t,"*"))
		{
			parser_expect_text(p,"*");
			value *= expr(p); 
		}
		else if (tkn_eq_text(t,"/"))
		{
			parser_expect_text(p,"/");
			value /= expr(p); 
		}
		else {
			parser_step(p);
		}

		*vptr = value;
}


/*
  eval	 := [ expr ] $END
  add_op := + | -
  mul_op := * | /
  digits := {+|-} [0..9] {[0..9]}
  expr   := term {add_op term}
  term   := factor {mul_op factor}
  factor := digits | '(' expr ')'
 */


// i thought i could use a trick from
// gist, nuh uh

float eval(Parser* p) {
	float value = 0;
	value = expr(p);
	int i = 0;
	// test thing
	#define MAX_ITERS 25
	while(p->position < p->lexer.token_count && i < MAX_ITERS) {
		debug("\neval pass:  { pos: %lu, : value: %f } \n",
				p->position, value);
		lookup_next(p,&value);
		i++;
	}
	return value;
}


float number(Parser* p) {
	char* literall = tkn_get_token_cstr(
			parser_get_token(p),
			p->scratch_buffer
		);
	if (!literall)
		parser_error(p,"%s","expected number, got NULL");
	return atof(literall);
}

float expr	(Parser* p) {
	float value = 0.0;
	
	value = term(p);
	Token tk = parser_get_token(p);
	
	if (tkn_eq_text(tk,"-")) {
		parser_expect_text(p,"-");
		debug("\nsub");
		value -= term(p);
	}
	else  if (tkn_eq_text(tk,"+")) {
		parser_expect_text(p,"+");
		debug("\nadd");
		value += term(p);
	} 

	return value;
}

float term	(Parser* p) {
	float value = 0;
	
	value = factor(p);
	Token tk = parser_get_token(p);
	
	if (tkn_eq_text(tk,"/")) {
		parser_expect_text(p,"/");
		debug("\ndiv");
		value /= factor(p);
	} 
	else if (tkn_eq_text(tk,"*")) {
		parser_expect_text(p,"*");
		debug("\nmul");
		value *= factor(p);
	} 

	return value;
}

float factor(Parser* p) {
	Token tk = parser_get_token(p);
	float value = 0.0;

	bool is_number = 
		tk.kind == Token_IntegerLiterall || 
		tk.kind == Token_FloatLiterall;

	if (tkn_is_end(tk)) {
		debug("\nend");
	}

	else if (tkn_eq_text(tk,"(")) {


		debug("\nblock begin");
		parser_expect_text(p,"(");
		value = expr(p);
		
		// we have to ignore the step
		// if it doens't match, but without
		// adding error to stack and calling 
		// the expression invalid :x
		if (USE_EXPR) 
			parser_expect_text(p,")");
		else
			parser_step_or_ignore(p,")");
	
		debug("\nblock end");
	} else if (is_number) {
		value = number(p);
		debug("\nnumber: %s,%f",tkn_get_token_cstr(tk,p->scratch_buffer),value);
		parser_step(p);
	} else {
		parser_error(p,"%s","bad factor");
	}
	
	if (USE_EXPR)
		lookup_next(p,&value);

	return value;
}
