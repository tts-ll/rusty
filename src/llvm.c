#include <stdio.h>
#include <glib.h>
#include <stdlib.h>
#include <string.h>


#include "llvm.h"
#include "ast.h"
#include "env.h"
#include "parser.h"
#include "type_annotate.h"

#define LLVM_DBG_OFF


//Tyler's branch
//Insert binary ops to llvm



static int var_count = 1;

void test_llvm(GNode* crate){

	

	GNode *item = crate->children->children;
	GNode *block;
	GNode *stmts;
	char* str;

	//Find the function definition
	while(item){

	
		
		if(get_ast(item)->kind == FNDEF_ITEM)
			break;
		
		item = item->next;
	}

	#ifdef LLVM_DBG_ON	
	printf("test_llvm: FOUND FUNCTION\n");
	#endif

	block = item->children;
	
	//Get the block
	while(block){

		if(get_ast(block)->kind == BLOCK)
			break;
		block = block->next;
	}
	
	stmts = block->children;
	
	#ifdef LLVM_DBG_ON			
	printf("test_llvm: FOUND_BLOCK\n");
	#endif

	while(stmts){
	
		
		str = llvm_exp( stmts->children );

		if(str)
			free(str);	
		stmts = stmts->next;
	}

	
}

char* llvm_exp(GNode *exp){

	int exp_kind = get_ast( exp )->kind;
	char* str;

	switch(exp_kind){
	
		case ADD_EXP:
		case SUB_EXP:
		case MUL_EXP:
		case DIV_EXP:
		case REM_EXP:
		case AND_EXP:
		case OR_EXP:
			str = llvm_binary_op( exp );
			break;
		
		case EQ_EXP:
		case NE_EXP:	
		case LT_EXP:
		case GT_EXP:
		case LE_EXP:
		case GE_EXP:
			str = llvm_comparison( exp );
			break;

		case ID:
			str = llvm_id( exp );	
			break;
		
		case LITDEC:
			str = llvm_litdec( exp );
			break;
			
		default:
			str = NULL;
	}


	return str;


}



char* llvm_binary_op(GNode *op){
	
	
		
	int op_kind = get_ast(op)->kind;
	char op_string[500];

	GNode *left = op->children;
	GNode *right = op->children->next;
	
	char* var = (char *)malloc(21 * sizeof(char) );
	
	//Get return strings for further down expressions
	char *str_left  = llvm_exp(left);
	char *str_right = llvm_exp(right);
	

	//Append llvm return variable to op_string
	var[0] = '%';
	itoa( var_count , ( var + 1 ) , 10 );	
	strcpy( op_string , var );
	
	var_count++;

		
	//append operation and type
	switch(op_kind){

		case ADD_EXP:
			strcat( op_string , " = add i32 " );
			break;

		case SUB_EXP:
			strcat( op_string , " = sub i32 " );
			break;
		
		case MUL_EXP:
			strcat( op_string , " = mul i32 " );
			break;

		case DIV_EXP:
			strcat( op_string , " = sdiv i32 " );
			break;		
	
		case REM_EXP:
			strcat( op_string , " = srem i32 " );	
			break;
	
		case OR_EXP:
			strcat( op_string , " = or i1 " );
			break;
		
		case AND_EXP:
			strcat( op_string , " = and i1 " );
			break;

		default:
			op_string[0] = '\0'; 						
	
	}

	//Append operands
	strcat(op_string , str_left);
	strcat(op_string , " , ");
	strcat(op_string , str_right);
	strcat(op_string , " ;\n");

	printf("%s", op_string);

	free(str_left);
	free(str_right);

	

	return var;
	
}

char* llvm_comparison(GNode* cmp){

	int cmp_kind = get_ast(cmp)->kind;

	char cmp_string[500];

	GNode *left = cmp->children;
	GNode *right = cmp->children->next;
	
	char* var = (char *)malloc(21 * sizeof(char) );
	
	//Get return strings for further down expressions
	char *str_left  = llvm_exp(left);
	char *str_right = llvm_exp(right);
	

	//Append llvm return variable to op_string
	var[0] = '%';
	itoa( var_count , ( var + 1 ) , 10 );	
	strcpy( cmp_string , var );
	
	var_count++;

	
	switch(cmp_kind){

		case EQ_EXP:
			strcat( cmp_string , " = icmp eq i32 " );
			break;

		case NE_EXP:
			strcat( cmp_string , " = icmp ne i32 " );
			break;
	
		case LT_EXP:
			strcat( cmp_string , " = icmp slt i32 " );
			break;

		case GT_EXP:
			strcat( cmp_string , " = icmp sgt i32 " );
			break;

		case LE_EXP:
			strcat( cmp_string , " = icmp sle i32 " );
			break;

		case GE_EXP:
			strcat( cmp_string , " = icmp sge i32 " );
			break;

		default:
			break;

	}
	
	//Append operands
	strcat(cmp_string , str_left);
	strcat(cmp_string , " , ");
	strcat(cmp_string , str_right);
	strcat(cmp_string , " ;\n");

	printf("%s", cmp_string);

	free(str_left);
	free(str_right);

	return var;

}

char* llvm_id(GNode *id_node){
	
	char* id_t = get_ast(id_node)->str; 
	int len = strlen(id_t);	
	
	char* id = (char*) malloc( (len+2) * sizeof(char) );
	id[0] = '%';	
	
	strcpy(id+1, id_t);	
	
	return id ;
	

}

char* llvm_litdec(GNode *litdec){

	int num = get_ast( litdec )->num;
	
	char* str = (char*)malloc( 100 * sizeof(char)  );
	
	return itoa(num , str , 10);

}


char* itoa(int val, char* usr_buff , int base){
	
	if(!usr_buff)
		return NULL;

	static char buf[32] = {0};
	
	int i = 30;
	
	for(; val && i ; --i, val /= base)
	
		buf[i] = "0123456789abcdef"[val % base];
	
	strcpy( usr_buff ,  &buf[i+1] );
	return usr_buff;
	
}


