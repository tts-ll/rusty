/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_PARSER_H_INCLUDED
# define YY_YY_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 15 "parser.y" /* yacc.c:1915  */

#include "ast.h"
#include <glib.h>

#line 49 "parser.h" /* yacc.c:1915  */

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    BOOL_OR = 258,
    BOOL_AND = 259,
    ADD = 260,
    NEGATIVE_SIGN = 261,
    ASTERISK = 262,
    DIV = 263,
    EXCLAMATION = 264,
    EQ = 265,
    NE = 266,
    REM = 267,
    LT = 268,
    GT = 269,
    LE = 270,
    GE = 271,
    ADDRESS = 272,
    LPAREN = 273,
    RPAREN = 274,
    MUT = 275,
    AS = 276,
    SHL = 277,
    SHR = 278,
    BITAND = 279,
    BITXOR = 280,
    BITOR = 281,
    ASSIGNMENT = 282,
    RANGE = 283,
    PLUS_ASSIGN = 284,
    SUB_ASSIGN = 285,
    MUL_ASSIGN = 286,
    DIV_ASSIGN = 287,
    REM_ASSIGN = 288,
    BITAND_ASSIGN = 289,
    BITXOR_ASSIGN = 290,
    BITOR_ASSIGN = 291,
    SHL_ASSIGN = 292,
    SHR_ASSIGN = 293,
    PATH = 294,
    FUNCTION_ARROW = 295,
    MATCH_ARROW = 296,
    DIRECTIVE = 297,
    DIRECTIVE_FEATURE = 298,
    APOST = 299,
    DOLLAR = 300,
    LSQUARE = 301,
    RSQUARE = 302,
    LCURLY = 303,
    RCURLY = 304,
    TRIPLE_DOT = 305,
    DOT = 306,
    COMMA = 307,
    COLON = 308,
    SEMICOLON = 309,
    ABSTRACT = 310,
    ALIGNOF = 311,
    BE = 312,
    BOX = 313,
    BREAK = 314,
    CASE = 315,
    CONST = 316,
    CONTINUE = 317,
    CRATE = 318,
    DO = 319,
    ELSE = 320,
    ENUM = 321,
    EXTERN = 322,
    FINAL = 323,
    FN = 324,
    FOR = 325,
    IF = 326,
    IMPL = 327,
    IN = 328,
    LET = 329,
    LOOP = 330,
    MACRO = 331,
    MACRO_RULES = 332,
    MATCH = 333,
    MOD = 334,
    MOVE = 335,
    NEW = 336,
    OFFSETOF = 337,
    OVERRIDE = 338,
    PRIV = 339,
    PUB = 340,
    PURE = 341,
    REF = 342,
    RETURN = 343,
    SIZEOF = 344,
    STATIC = 345,
    SELF = 346,
    STRUCT = 347,
    SUPER = 348,
    TRAIT = 349,
    TYPE = 350,
    TYPEOF = 351,
    UNKNOWN = 352,
    UNSAFE = 353,
    UNSIZED = 354,
    USE = 355,
    VIRTUAL = 356,
    WHERE = 357,
    WHILE = 358,
    YIELD = 359,
    UNIT = 360,
    BOOL = 361,
    U8 = 362,
    U16 = 363,
    U32 = 364,
    U64 = 365,
    I8 = 366,
    I16 = 367,
    I32 = 368,
    I64 = 369,
    F32 = 370,
    F64 = 371,
    USIZE = 372,
    ISIZE = 373,
    CHAR = 374,
    STR = 375,
    UNDERSCORE = 376,
    LITDEC = 377,
    ID = 378,
    LITSTR = 379,
    LITCHAR = 380,
    FALS = 381,
    TRU = 382,
    FLSUFFi = 383,
    EXCOMMA_L = 384,
    EXCOMMA_H = 385,
    ADDRMUT = 386,
    DEREF = 387,
    UMINUS = 388,
    NOT = 389,
    FNCALLEXP = 390,
    FLUPSHIFT = 391,
    NULLLIST = 392,
    END = 393
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 21 "parser.y" /* yacc.c:1915  */

	struct {
		int line_number;
		int num;
		char * str;
		char c;
		int booln;
		GNode * gast;
	};

#line 211 "parser.h" /* yacc.c:1915  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_PARSER_H_INCLUDED  */
