%{
  using namespace std;
  #include <stdio.h>
  #include <iostream>
  #include <string>
  #include <ctype.h>
  #include "query.h"
  #include "parse.h"
  #include "super-smack-yacc.h"

  static string q_str = "";
%}

%option yylineno

%x Q_STR
%x Q_STR_ESC
%x COMMENT

WHITE	[ \n\t]
LETTER	[a-zA-Z]
IDENT_CHAR [a-zA-Z0-9_]
DIGIT [0-9]

%%

<INITIAL>"query" { return QUERY_SYM; }
<INITIAL>"type"  { return TYPE_SYM; }
<INITIAL>"has_result_set" { return HAS_RESULT_SET_SYM; }
<INITIAL>"client" { return CLIENT_SYM; }
<INITIAL>"dictionary" { return DICT_SYM; }
<INITIAL>"table" { return TABLE_SYM; }
<INITIAL>"delim" { return DELIM_SYM; }
<INITIAL>"file_size_equiv" { return FILE_SIZE_EQUIV_SYM; }
<INITIAL>"create" { return CREATE_SYM; }
<INITIAL>"min_rows" { return MIN_ROWS_SYM; }
<INITIAL>"data_file" { return DATA_FILE_SYM; }
<INITIAL>"gen_data_file" { return GEN_DATA_FILE_SYM; }
<INITIAL>"parsed" { return PARSED_SYM; }
<INITIAL>"query_barrel" { return QUERY_BARREL_SYM; }
<INITIAL>"source" { return SOURCE_SYM; }
<INITIAL>"source_type" { return SOURCE_TYPE_SYM; }
<INITIAL>"main" { return MAIN_SYM;}
<INITIAL>"{" { return OPEN_SECT_SYM;}
<INITIAL>"}" { return CLOSE_SECT_SYM;}
<INITIAL>"(" { return OPEN_PAREN_SYM;}
<INITIAL>")" { return CLOSE_PAREN_SYM;}
<INITIAL>"\"" { BEGIN Q_STR; q_str = "";}
<INITIAL>";" { return SEMI_SYM;}
<INITIAL>"." { return DOT_SYM;}
<INITIAL>"," { return COMMA_SYM;}
<INITIAL>"$" { return DOLLAR_SYM;}
<INITIAL>"::" { return COL_COL_SYM;}
<INITIAL>"=" { return EQ_SYM;}
<INITIAL>"db" { return DB_SYM;}
<INITIAL>"host" { return HOST_SYM;}
<INITIAL>"user" { return USER_SYM;}
<INITIAL>"pass" { return PASS_SYM;}
<INITIAL>"port" { return PORT_SYM;}
<INITIAL>"socket" { return SOCKET_SYM;}
<INITIAL>{LETTER}{IDENT_CHAR}* { yylval.str = new string(yytext);
 return IDENT_SYM;
}

<INITIAL>{DIGIT}+ { yylval.str = new string(yytext); return NUMBER_SYM; }
<INITIAL>"#"|"//" { BEGIN COMMENT;}
<COMMENT>\n { BEGIN 0;}
<COMMENT>.|\n {}

<Q_STR>"\\" { BEGIN Q_STR_ESC; }
<Q_STR_ESC>"\\"|"\""|\n { q_str += yytext; BEGIN Q_STR; }
<Q_STR_ESC>. {}
<Q_STR>"\"" { BEGIN 0; yylval.str = new string(q_str); return STRING_SYM;}
<Q_STR>.|\n { q_str += yytext;}

.|\n {}

%%

int yywrap(void)
{
  return 1;
}













