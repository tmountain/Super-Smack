%{
#include "../config.h"

#include <stdio.h>
#include <iostream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <getopt.h>
  
#include "die.h"
#include "query.h"
#include "client.h"
#include "parse.h"

extern char* yytext;
extern int yylineno;
extern FILE* yyin;
int yyparse();
int yylex();

void yyerror(char *);
void do_parse(FILE *);
%}

%token TABLE_SYM
%token QUERY_SYM
%token STRING_SYM
%token OPEN_SECT_SYM
%token CLOSE_SECT_SYM
%token SEMI_SYM
%token TYPE_SYM
%token HAS_RESULT_SET_SYM
%token CLIENT_SYM
%token PARSED_SYM
%token QUERY_BARREL_SYM
%token DICT_SYM
%token SOURCE_SYM
%token SOURCE_TYPE_SYM
%token USER_SYM
%token PASS_SYM
%token DB_SYM
%token HOST_SYM
%token IDENT_SYM
%token NUMBER_SYM
%token DOT_SYM
%token OPEN_PAREN_SYM
%token CLOSE_PAREN_SYM
%token MAIN_SYM
%token COMMA_SYM
%token DOLLAR_SYM
%token EQ_SYM
%token COL_COL_SYM
%token DELIM_SYM
%token FILE_SIZE_EQUIV_SYM
%token CREATE_SYM
%token MIN_ROWS_SYM
%token DATA_FILE_SYM
%token GEN_DATA_FILE_SYM
%token PORT_SYM
%token SOCKET_SYM

%left EQ_SYM
%left DOT_SYM

%union
{
  string* str;
  Query* q;
  Query_def_line* q_def_line;
  Dictionary* d;
  Client* c;
  Table_def_line* table_def_line;
  Table_def* table_def;
  Dict_def* dict_def;
  Dict_def_line* dict_def_line;
  Client_def* client_def;
  Client_def_line* client_def_line;
  Main_line* main_line;
  Param_list* param_list;
  int n;
  Val* val;
}

%type <str> STRING_SYM
%type <str> TABLE_SYM
%type <str> IDENT_SYM
%type <str> NUMBER_SYM
%type <str> method_name
%type <val> simple_expr
%type <val> expr
%type <q> query
%type <q> query_def
%type <q_def_line> query_def_line
%type <c> client
%type <d> dict
%type <dict_def> dict_def
%type <dict_def_line> dict_def_line
%type <client_def> client_def
%type <client_def_line> client_def_line
%type <table_def> table_def
%type <table_def_line> table_def_line
%type <main_line> main_line
%type <param_list> param_list


%start program

%%

program : section_list main
{
}
;

main : MAIN_SYM OPEN_SECT_SYM main_lines CLOSE_SECT_SYM
{
}
;

section_list: section_list section
{
}
|
section
{
}
;

section: query
{
  query_pool[$1->name] = $1;
}
| dict
{
  dict_pool[$1->name] = $1;
}
| client
{
  client_pool[$1->name] = $1;
}
| table
;

table: TABLE_SYM STRING_SYM OPEN_SECT_SYM table_def CLOSE_SECT_SYM
{
  $4->validate_table(*($2));
}
;

dict: DICT_SYM STRING_SYM OPEN_SECT_SYM dict_def CLOSE_SECT_SYM
{
  $4->name = *($2);
  $$ = $4->make_dict();
}
;

client: CLIENT_SYM STRING_SYM OPEN_SECT_SYM client_def CLOSE_SECT_SYM
{
  $$ = $4->make_client();
  $$->name = *($2);
}
;

table_def: table_def table_def_line
{
  $$ = $1;
  $2->update_table_def($1);
}
| table_def_line
{
  $$ = new Table_def;
  $1->update_table_def($$);
}
;

dict_def: dict_def dict_def_line
{
  $$ = $1;
  $2->update_dict_def(*($1));
}
| dict_def_line
{
  $$ = new Dict_def;
  $1->update_dict_def(*($$));
}
;

client_def: client_def client_def_line
{
  $$ = $1;
  $2->update_client_def(*($$));
}
| client_def_line
{
  $$ = new Client_def;
  $1->update_client_def(*($$));
}
;

table_def_line: CLIENT_SYM STRING_SYM SEMI_SYM
{
  $$ = new Table_client_def_line(*($2));
}
| CREATE_SYM STRING_SYM SEMI_SYM
{
  $$ = new Table_create_def_line(*($2));
}
| MIN_ROWS_SYM STRING_SYM SEMI_SYM
{
  $$ = new Table_min_rows_def_line(*($2));
}
| DATA_FILE_SYM STRING_SYM SEMI_SYM
{
  $$ = new Table_data_file_def_line(*($2));
}
| GEN_DATA_FILE_SYM STRING_SYM SEMI_SYM
{
  $$ = new Table_gen_data_file_def_line(*($2));
}
;

dict_def_line: TYPE_SYM STRING_SYM SEMI_SYM
{
  $$ = new Dict_dict_type_def_line(*($2));
}
| SOURCE_TYPE_SYM STRING_SYM SEMI_SYM
{
  $$ = new Dict_source_type_def_line(*($2));
}
| SOURCE_SYM STRING_SYM SEMI_SYM
{
  $$ = new Dict_source_def_line(*($2));
}
| DELIM_SYM STRING_SYM SEMI_SYM
{
  $$ = new Dict_delim_def_line(*($2));
}
| FILE_SIZE_EQUIV_SYM STRING_SYM SEMI_SYM
{
  $$ = new Dict_file_size_equiv_def_line(*($2));
}
;

client_def_line: QUERY_BARREL_SYM STRING_SYM SEMI_SYM
{
  $$ = new Client_query_barrel_def_line(*($2));
}
| USER_SYM STRING_SYM SEMI_SYM
{
  $$ = new Client_user_def_line(*($2));
}
| PASS_SYM STRING_SYM SEMI_SYM
{
  $$ = new Client_pass_def_line(*($2));
}
| DB_SYM STRING_SYM SEMI_SYM
{
  $$ = new Client_db_def_line(*($2));
}
| HOST_SYM STRING_SYM SEMI_SYM
{
  $$ = new Client_host_def_line(*($2));
}
| PORT_SYM STRING_SYM SEMI_SYM
{
  $$ = new Client_port_def_line(*($2));
}
| SOCKET_SYM STRING_SYM SEMI_SYM
{
  $$ = new Client_socket_def_line(*($2));
}

;

query: QUERY_SYM STRING_SYM OPEN_SECT_SYM query_def CLOSE_SECT_SYM
{
  $4->name = *($2);
  $$ = $4;
}
;

query_def: query_def query_def_line
{
  $2->set_q_param($1);
  $$ = $1;
}
| query_def_line
{
  $$ = new Query;
  $1->set_q_param($$);
};

query_def_line: QUERY_SYM STRING_SYM SEMI_SYM
{
  $$ = new Query_query_def_line(*($2));
}
| TYPE_SYM STRING_SYM SEMI_SYM
{
  $$ = new Query_type_def_line(*($2));
}
| HAS_RESULT_SET_SYM STRING_SYM SEMI_SYM
{
  $$ = new Query_has_result_set_def_line(*($2));
}
| PARSED_SYM STRING_SYM SEMI_SYM
{
  $$ = new Query_parsed_def_line(*($2));
}

;

main_lines: main_lines main_line
{
  $2->execute();
}
| main_line
{
  $1->execute();
}
| main_lines assign_line
{
}
|
assign_line
{
}
;

main_line: IDENT_SYM DOT_SYM method_name OPEN_PAREN_SYM param_list
 CLOSE_PAREN_SYM SEMI_SYM
{
  $$ = new Main_line;
  $$->client = *($1);
  $$->method = *($3);
  $$->param_list = $5;
}
;

method_name: IDENT_SYM
{
  $$ = $1;
}
|
QUERY_SYM
{
  $$ = new string("query");
}
;

assign_line: DOLLAR_SYM IDENT_SYM EQ_SYM expr SEMI_SYM
{
  Val* v = var_hash[*$2];
  if(!v)
    {
      v = new Val;
      v->delete_me_not = 1;
      var_hash[*$2] = v;
    }
  v->set(*$4);
}
;

param_list: param_list COMMA_SYM expr
{
  $1->add($3->strval);
  $$ = $1;
}
|
expr
{
  $$ = new Param_list;
  $$->add($1->strval);
}
|
{
  $$ = new Param_list;
}
;

expr: expr DOT_SYM expr
{
  $$ = new Val(*$1);
  $$->concat(*$3);
  if(!$1->delete_me_not)
    delete $1;
  if(!$3->delete_me_not)
    delete $3;
  
}
|
simple_expr
{
}
;

simple_expr: NUMBER_SYM
{
  $$ = new Val(*$1);
}
| STRING_SYM
{
  $$ = new Val(*$1);
  
}
| DOLLAR_SYM NUMBER_SYM
{
  int n = atoi($2->c_str());
  if(n >= glob_argc)
    die(0, "Line %d references argument %d while the command line supplied\
 only %d arguments\n", yylineno, n, glob_argc - 1);
  $$ = new Val(glob_argv[n]);
}
| DOLLAR_SYM IDENT_SYM
{
  Val* v = var_hash[*$2];
  if(!v)
    die(0, "Line %d: reference to uninitialized variable '%s' ",
	yylineno, $2->c_str());
  $$ = v;
}
|
IDENT_SYM COL_COL_SYM IDENT_SYM COL_COL_SYM IDENT_SYM
{
  Client *c = client_pool[$1->c_str()];
  if(!c)
    die(0, "Line %d: Client '%s' is not defined ", yylineno, $1->c_str());
  $$ = c->get_param_val(*$3, *$5);
}
|
IDENT_SYM COL_COL_SYM IDENT_SYM
{
  Client *c = client_pool[$1->c_str()];
  if(!c)
    die(0, "Line %d: Client '%s' is not defined ", yylineno, $1->c_str());
  $$ = c->get_param_val(*$3);
}
;


%%

void yyerror(char* msg)
{
  die(0, "Parse error on line %d near %s", yylineno, yytext);
}

void do_parse(FILE *fd)
{
  yyin = fd;
  yyparse();
}
