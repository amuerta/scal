// 	Basic General purpouse Tokenizer (and Parser)
//
// 	example usage: 
/*
// NOTE: this is old example, still should work tho
#define HCH_TOKENIZER_IMPLEMENTATION
#include "tokenizer.h"
#include <stdio.h>

int main(int argc, char** argv) {
	char* string = "var = 'var' + 1.2 + vas \n full";

	Tokenizer t = {
		.source = string,
		.src_len = strlen(string),
		.position = 0
	};

	TokenDefinition dict[] = {
		{"\n",  "EOL"},
		{"-",  "BinOp"},
		{"+",  "BinOp"},
		{"=",  "BinOp"},
		{";",  "BinOp"},
	};

	tokenize(&t,dict,5,'\'');

	for (uint i = 0; i < t.token_count; i++) {
		Token tk = t.tokens[i];
		printf("{");
		for(uint c = 0; c < tk.len; c++) {
			if (!tk.text) {
				printf("newl");
				break;
			}
			printf("%c",tk.text[c]);

		}
		printf("}\t");
	}
	tokenizer_clear(&t);
}
*/

#pragma once

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#ifndef TKN_MAX_TAG_SIZE 
#	define TKN_MAX_TAG_SIZE 128
#endif

#ifndef TOKENIZER_H // header guard
#	define TOKENIZER_H


#ifdef __MINGW32__
	typedef unsigned int uint;

	void* reallocarray(void* ptr, size_t nmel, size_t elsz) {
		return realloc(ptr,nmel*elsz);
	}
#endif

typedef enum {
	Token_None,
	Token_Other,
	Token_EOL,
	Token_Word,
	Token_StringLiterall,
	Token_IntegerLiterall,
	Token_FloatLiterall,
	Token_EndParsing,
} TokenKind;

typedef struct {
	char* 		text;
	char* 		tag;
} TokenDefinition;

typedef struct {
	char* text;
	size_t len;
	char* tag;
	TokenKind kind;
} Token;

typedef struct {
	size_t 	src_len;
	char*  	source;
	size_t 	position;
	size_t 	token_count;
	Token* 	tokens;
	bool 	src_processed;

	// this cstr will live as long as you
	// dont call it again, as soon as 
	// tkn_get_token_str() is called again,
	// previous contents of this cstring is 
	// memset to 0.
	char*  temp_cstr_of_requested_token;
} Tokenizer;


#define TOKEN_INVALID (Token) {0}
#define TOKEN_END	  (Token) {.king = Token_EndParsing}

// API FUNCTIONS
bool	tkn_is_valid(Token t);
bool 	tkn_is_end(Token t) ;
bool 	tkn_eq_text(Token t, const char* text);
bool 	tkn_eq_tag(Token t, const char* tag);
char* 	tkn_alloc_str(char* cstr);
char 	tkn_src_curr(Tokenizer  t);
char 	tkn_src_next(Tokenizer* t);
char 	tkn_src_prev(Tokenizer t);
void  	tkn_append(Token* t, char c);
void 	tkn_pop(Token* t) ;
void 	tkn_clear(Token *t) ;
void 	tkn_tokens_append(Tokenizer *t, Token tok) ;
void 	tokenizer_clear(Tokenizer *t) ;
void 	tkn_cstr(Token* t, char* cstr) ;
bool 	tkn_char_is_newl(char c) ;
bool 	tkn_char_is_space(char c) ;
bool 	tkn_char_is_dec(char c) ;
bool 	tkn_char_is_lowercase_latter(char c) ;
bool 	tkn_char_is_capitalcase_latter(char c) ;
Token 	tkn_word(Tokenizer* t) ;
Token 	tkn_number_literall(Tokenizer* t) ;
Token 	tkn_string_literall(Tokenizer* t, char quote) ;


// USER LAND
Token tkn_tokenize_from_dictionary
			(Tokenizer* t, 
			 TokenDefinition* dictionary,
			 size_t dict_len);

void tokenize(Tokenizer* t, 
			  TokenDefinition* dict, 
			  size_t dict_len,
			  char quote);



//
//	IMPLEMENTATION
//


#ifdef HCH_TOKENIZER_IMPLEMENTATION

bool	tkn_is_valid(Token t) {
	return (t.text && t.len > 0 && t.kind != Token_None);
}

bool 	tkn_is_end(Token t) {
	return t.kind == Token_EndParsing;
}

bool 	tkn_eq_text(Token t, const char* text) {
	return !tkn_is_end(t) && (strncmp(t.text,text,t.len) == 0);
}

bool 	tkn_eq_tag(Token t, const char* tag) {
	return strncmp(t.tag,tag,t.len) == 0;
}

char* tkn_alloc_str(char* cstr) {
	char* str = calloc(strlen(cstr)+1,sizeof(char));
	strncpy(str,cstr,strlen(cstr));
	return str;
}

char tkn_src_curr(Tokenizer  t) {
	if (t.position < t.src_len) {
		return t.source[t.position];
	}
	return 0;
}

char tkn_src_next(Tokenizer* t) {
	if (t->position < t->src_len) {
		return t->source[t->position+1];
	}
	return 0;
}

char tkn_src_prev(Tokenizer t) {
	if (t.position > 0) {
		return t.source[t.position-1];
	}
	return 0;
}

void  tkn_append(Token* t, char c) {
	t->text = reallocarray(
			t->text,
			t->len+1,
			sizeof(char)
	);
	assert(t->text && "t->text should never be NULL");
	t->text[t->len] = c;
	t->len++;
}

void tkn_pop(Token* t) {
	t->text = reallocarray(
			t->text,
			t->len-1,
			sizeof(char)
	);
	assert(t->text && "t->text should never be NULL");
	t->len--;
}

void tkn_clear(Token *t) {
	if (t->text) {
		free(t->text);
	}
	if (t->tag) {
		free(t->tag);
	}
	t->kind = Token_None;
	t->len = 0;
}

void tkn_tokens_append(Tokenizer *t, Token tok) {
	// TODO: linear allocation time, use exponential capacity
	// do decide when to reallocate
	t->tokens = reallocarray(
			t->tokens,
			t->token_count+1,
			sizeof(Token)
		);
	assert(t->tokens && "t->tokens should never be NULL");
	
	// set callback pointer to be
	// able to buffer temp string in 
	// tokenizer struct
	t->tokens[t->token_count] = tok;
	t->token_count++;
}

void tokenizer_clear(Tokenizer *t) {
	if (t->tokens) {
		for (uint i = 0; i < t->token_count; i++) {
			tkn_clear(&t->tokens[i]);
		}
		free(t->tokens);
	}
	if (t->temp_cstr_of_requested_token)
		free(t->temp_cstr_of_requested_token);
}

void tkn_cstr(Token* t, char* cstr) {
	for(uint i = 0; i < strlen(cstr);i++){
		tkn_append(t,cstr[i]);
	}
}


bool tkn_char_is_newl(char c) {
	return (c == '\n');
}

bool tkn_char_is_space(char c) {
	return (c == ' ' || c == '\t' );
}

bool tkn_char_is_dec(char c) {
	return (c >= '0' && c <= '9' );
}

bool tkn_char_is_lowercase_latter(char c) {
	return (c >= 'a' && c <= 'z' );
}

bool tkn_char_is_capitalcase_latter(char c) {
	return (c >= 'A' && c <= 'Z' );
}

Token tkn_word(Tokenizer* t) {
	Token tok = {0};
	size_t relative_i = 0;

	for (uint i = t->position; i < t->src_len; i++) {
		t->position = i;
		char curr = tkn_src_curr(*t);
		bool llat   = tkn_char_is_lowercase_latter(curr);
		bool clat   = tkn_char_is_capitalcase_latter(curr);
		bool is_dec = tkn_char_is_dec(curr);

		if (llat || clat || curr=='_') {
			tkn_append(&tok,curr);
		} else if (is_dec && relative_i != 0) {
			tkn_append(&tok,curr);
		}
		else {

			tok.kind = Token_Word;
			break;
		}

		relative_i++;
	}
	return tok;
}

Token tkn_number_literall(Tokenizer* t) {
	Token tok = {0};
	char curr = tkn_src_curr(*t);
	char prev = 0;
	char next = 0;
	uint relative_i = 0;
	uint dot_count  = 0;

	// suppress missleading warnings
	(void) prev;
	(void) next;

	for (uint i = t->position; i < t->src_len; i++) {
		t->position = i;
		curr = tkn_src_curr(*t);
		prev = tkn_src_prev(*t);
		next = tkn_src_next(t);

		if (tkn_char_is_dec(curr)) {
			tkn_append(&tok,curr);
		} else if (	curr == '.' &&
					tkn_char_is_dec(next) &&
					relative_i != 0 && 
					dot_count < 1) 
		{
			tok.kind = Token_FloatLiterall;
			tkn_append(&tok,curr);
			dot_count++;
		}
		else  {
			break;
		}

		relative_i++;
	}

	if (tok.kind != Token_FloatLiterall &&
		tok.len > 0) 
	{
		tok.kind = Token_IntegerLiterall; 
	}

	return tok;
}


Token tkn_string_literall(Tokenizer* t, char quote) {
	Token tok = {0};
	char curr = tkn_src_curr(*t);
	char prev = 0;//tkn_src_prev(*t);
	char next = 0;//tkn_src_next(t);

	// suppress missleading warnings
	(void) prev;
	(void) next;

	if (curr == quote) {
		t->position++;
		//printf(" HIT ");
	}
	else {
		return tok; // EMPTY
	}

	for(uint i = t->position; i < t->src_len; i++) {
		t->position = i;
		curr = tkn_src_curr(*t);
		prev = tkn_src_prev(*t);
		next = tkn_src_next(t);
		
		if (curr == quote)
		{
			if (prev == '\\') {
				tkn_pop(&tok);
				tkn_append(&tok,quote);
				continue;
			}
			else {
				// we have to skip one char forward
				// to prevent double quote hits
				tok.kind = Token_StringLiterall;
				t->position += 1;
				break;
			}
		}
		else {
			tkn_append(&tok, t->source[i]);
		}
	}
	return tok;
}

Token tkn_tokenize_from_dictionary
	(Tokenizer* t, 
	 TokenDefinition* dictionary,
	 size_t dict_len) 
{
	Token tok = {0};

	for(uint i = 0; i < dict_len; i++) {
		TokenDefinition tok_def = dictionary[i];
		size_t tok_len = strlen(tok_def.text);
		bool fits_src = (t->position < t->src_len);
		size_t src_rem = 0;
		char* current = 0;
		
		if (fits_src) {
			current = t->source + t->position;
			src_rem = strlen(current);
		} else continue;

		if (src_rem >= tok_len && 
			strncmp(tok_def.text,current,tok_len) == 0) 
		{
			tkn_cstr(&tok,tok_def.text);
			t->position += tok_len;
			
			// for tkn_gettemp_tag(Token) static string;
			assert( strlen(tok_def.tag) < TKN_MAX_TAG_SIZE );
			tok.tag = tkn_alloc_str(tok_def.tag);
			tok.kind = Token_Other;
			break;
		} else continue;

	}

	return tok;
}

void tokenize
	(Tokenizer* t, 
	 TokenDefinition* dict, 
	 size_t dict_len,
	 char quote) 
{

	// checks
	t->temp_cstr_of_requested_token = NULL;
	

	Token tok = {0};
	for(uint i = 0; i < t->src_len; i++) {
	
		char ch = tkn_src_curr(*t);
		bool is_uppercase = tkn_char_is_capitalcase_latter(ch);
		bool is_lowercase = tkn_char_is_lowercase_latter(ch);
		bool is_decimal   = tkn_char_is_dec(ch);
		bool is_space     = tkn_char_is_space(ch);
		bool is_newline   = tkn_char_is_newl(ch);

		if (is_space || is_newline) {
			t->position+=1;

			if (is_newline) {
				tok.text = NULL;
				tok.tag  = NULL;
				tok.kind = Token_EOL;
				tkn_tokens_append(t,tok);
			}

			continue;

		} else if (ch == quote) {
			tok = tkn_string_literall(t,quote);

		} else if (is_uppercase || is_lowercase) {
			if (t->source[t->position+1] == 0) {
				return;
			}
			tok = tkn_word(t);

		} else if (is_decimal) {
			// no idea what was THAT about \ (._.) /
			// 	if (t->source[t->position+1] == 0) {
			if (t->source[t->position] == 0) {
				return;
			}
			tok = tkn_number_literall(t);

		} else {
			tok = tkn_tokenize_from_dictionary(t,dict,dict_len);
		}

		if (tok.len > 0) {
			tkn_tokens_append(t,tok);

		} else {
			t->position++;
			tkn_clear(&tok);
		}

		if (t->position >= t->src_len)
			break;
	}

	t->src_processed = true;
}

#endif




// PARSER

// TODO: it seems the grammar can be automated
// so it makes scense to make ParserSystem API
// that uses grammar definitions to do parsing, and define
// user API for building AST or just acessing info from a recusrion
// tree.

/*
PSUEDO CODE:

	// define tree node you want
	// typedef struct {...} MyTreeNode;
	//
	// define functions that operate on the tree
	// ...
	//
	// wrap then into callback functions of ParserSystem
	// ...

	int main(void) {
		ParserGrammar grammar = parsys_grammar_from_cstr({
			"word ::= string | number ;"
		});

		ParserCallbacks callbacks = {
			.add_node = my_tree_add_node,
			.create_node = my_tree_create_node,
			.swap_node	= my_node_swap,
		};

		ParserSystem ps = parsys_init(grammar,callbacks);

		MyTreeNode ast = parsys_parse(&ps, "word = 'jason';");
	}
*/

typedef struct {
	bool		use_error_stack;
	// TODO: use this
	bool 		urgent_error_exit;

	Tokenizer 	lexer; // has tokens and thier count
	size_t 		position;
	
	Token 		current_token; // trough multiple call frames, sometimes we dont want to 
							   // shift token and instead match the current one.

	char* 		scratch_buffer;
	char*		error_stack;
	size_t		errstack_sz;
	size_t		errstack_top; // top index, "length" of error stack
	
	// TODO: use this
	bool 		has_error;
} Parser;


#ifndef PARSER_ERRSTACK_SZ 
#	define PARSER_ERRSTACK_SZ 1024
#endif

// TODO: add all existing parser finctions,
// move token functions to tokenizer.h
void 	parser_allocate_errstack(Parser* p, size_t size);
Token 	parser_lookahead	(Parser p, size_t steps);
void 	parser_step			(Parser* p);
Token	parser_get_token	(Parser* p);



#ifdef HCH_PARSER_IMPLEMENTATION

void parser_allocate_errstack(Parser* p, size_t size) {
	assert( size > 0 && "Errorstack cannot be 0");
	p->error_stack 	= malloc(size);
	memset(p->error_stack,0,size);
	p->errstack_sz 	= size;
	p->errstack_top = 0;
}


void parser_step(Parser* p) {
	const Token  end = {.kind = Token_EndParsing};
	if (p->lexer.token_count > p->position) {
		p->current_token = p->lexer.tokens[p->position++];
		return;
	}
	p->current_token = end;
}

Token 	parser_lookahead(Parser p, size_t step) {
	const size_t index 		= p.position + step;
	if (p.lexer.token_count > index) {
		return p.lexer.tokens[index];
	}
	return TOKEN_INVALID;
}

	
char*	tkn_get_token_cstr(Token t, char* scratch_buffer) {
	assert(scratch_buffer && "Expected scratch buffer to be NOT NULL");

	if (t.kind == Token_EndParsing || t.len == 0 || !t.text)
		return NULL;

	memset(scratch_buffer, 0, t.len+1);
	strncpy(scratch_buffer, t.text, t.len);
	
	return scratch_buffer;
}


#define parser_error(P,FMT,...)  do {							\
	if ((P)->use_error_stack)									\
		assert((P)->error_stack && 								\
				"expected to have error stack initilized");		\
	snprintf((P)->error_stack,(P)->errstack_sz ,				\
			"\n | >  %s() :: '%s' " FMT,						\
			__func__,											\
			tkn_get_token_cstr									\
				((P)->current_token,							\
				 (P)->scratch_buffer),							\
			__VA_ARGS__											\
	);															\
	(P)->has_error = true;										\
} while(0)

// Gets token, expects a token that matches {kind,tag,text}
// if it doesn't get expected, errors out and prints
// error stack trace
//
// if provided test condition is 0, NULL or Token_None
// then testing is skipped for that case


Token parser_get_token(Parser* p) {
	if (p->position == 0) {
		parser_step(p);
	}
	return p->current_token;
}


// TODO:
// impl parser_match_text_option(Parser* p, size_t count , ...) {
// }

bool parser_match_text(Parser* p, const char* text) {
	if (p->position == 0) {
		parser_step(p);
	}
	Token t = p->current_token;
	if (!tkn_is_valid(t)) {
		return false;
	}
	return (t.text && strcmp(t.text,text) == 0);
}

void parser_expect_text(Parser* p, const char* text) {
	
	// -1 the step guard:
	// ref link: 
	// 	https://gist.github.com/amuerta/da611a323411a4001c173ea2258dd786#file-calc-c-L238
	if (p->position == 0) {
		parser_step(p);
	}

	char temp_recieved[256];
	Token t = p->current_token;

	if (!tkn_is_valid(t)) {
		return;
	}

	// NASTY BUG
	bool match = (t.text && strncmp(t.text,text,t.len) == 0);
	
	if (match) {
		parser_step(p);
		return;
	}

	// TODO: implement proper size checking
	// remove *magic numbers*
	strncpy(temp_recieved,
			tkn_get_token_cstr(p->current_token,p->scratch_buffer),
			256);
	parser_error(p,"Expected token with text '%s' got '%s'",
			text, temp_recieved);
}

void parser_step_or_ignore(Parser* p, const char* text) {
	
	if (p->position == 0) {
		parser_step(p);
	}

	Token t = p->current_token;

	if (!tkn_is_valid(t)) {
		return;
	}

	bool match = (t.text && strncmp(t.text,text,t.len) == 0);
	
	if (match) {
		parser_step(p);
		return;
	}
}

void parser_expect_tag(Parser* p, const char* tag) {

	if (p->position == 0) {
		parser_step(p);
	}

	char temp_recieved[256];
	Token t = parser_get_token(p);
	bool match = (t.tag && strncmp(t.tag,tag,t.len) == 0);
	
	if (match) {
		parser_step(p);
		return;
	}

	strncpy(temp_recieved,
			tkn_get_token_cstr(p->current_token, p->scratch_buffer),
			256);
	parser_error(p,"Expected token with text '%s' got '%s'",
			tag, temp_recieved);
}


char* parser_get_errors(Parser* p) {
	return p->error_stack;
}

void parser_begin(
		Parser* p,
		Tokenizer t, 
		bool use_error_stack,
		char* scratch_buffer
	) {
	*p = (Parser) {
		.lexer = t,
		.position = 0,
		.use_error_stack = use_error_stack,
		.current_token = t.tokens[0], 
		.scratch_buffer	= scratch_buffer,
	};

	if (use_error_stack) {
		parser_allocate_errstack(p, PARSER_ERRSTACK_SZ);
	}

}

void parser_reset(Parser* p ) {
	p->position = 0;
	p->current_token = p->lexer.tokens[0];
}

void parser_clear(Parser* p, bool clear_scratch_buffer) {
	if (p->scratch_buffer && clear_scratch_buffer) {
		free(p->scratch_buffer);
		p->scratch_buffer = NULL;
	}

	if (p->use_error_stack && p->error_stack) {
		free(p->error_stack);
	}

	p->position = 0;
}

#endif // IMPLEMENTATION


#endif// HEADER GUARD
