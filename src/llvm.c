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
static int loop_count = 1;
static int need_puts = 0;

//Just for Testing
//Gets us to a function's block so that we can test
void test_llvm(GNode* crate){

	

	GNode *item = crate->children->children;
	GNode *block;
	
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
	
	str = llvm_block( block );	
	

	
}

char* llvm_crate(GNode * crate){

	GNode *item = crate->children->children;
	char* str;

	while(item){
		switch(get_ast(item)->kind){
			case FNDEF_ITEM:
				if(!strcmp(get_item_id(item),"main")){
					str = llvm_maindef(item);				
				}
				else{
					str = llvm_fndef(item);
				}
				break;
			case STRUCTDEF_ITEM:
				break;
			default:
				printf("Unsupported item in rust-like source.  Perhaps enum?\n");
				exit(-1);
		}

		item = item->next;
	}

	if(need_puts){
		printf("declare i32 @puts(i8*)\n");
	}
	

	return str;
}

char * llvm_maindef(GNode * maindef){
	
	GNode * block = g_node_last_child(maindef);
	
	char * name = "@main()";
	char * str;
		
	printf("define void %s {\n\n", name);
	
	str = llvm_block(block);

	printf("\nret void\n");
	printf("}\n");
	
	return str;
}

char * llvm_fndef(GNode * fndef){
	char * str;
	str = NULL;
	return str;

}

char * llvm_block(GNode* block){

	GNode* stmt = block->children;
	char* str = NULL;	

	while(stmt){

		switch( get_ast(stmt)->kind ){
	
			case STMT:
				str = llvm_exp(stmt->children);
				break;
			case RETURNEXP_STMT:
			case RETURN_STMT:
			case LET_STMT:
			case LETTYPEASS_STMT:
			case LETASS_STMT:
			case LETTYPE_STMT:
				str = NULL;
				break;
			case PRINTI_STMT:
				str = llvm_printi(stmt->children->children);
				break;
			case PRINTS_STMT:
				str = llvm_prints(stmt->children->children);
				break;
			default:
				str = llvm_exp(stmt);
				break;
		}
		
		stmt = stmt->next;

	}
		
	return str;
}

char * llvm_printi(GNode * exp){
	need_puts = 1;// will cause a declaration of puts for external linking
	char* str = llvm_exp(exp);
	/*
	char* call_string;
	printf("%%c = alloca i8\n");
	printf("store i8 %s, i8* %%c\n", str);
	call_string = " = call i32 @puts(i8* %%c) ";	
	
	str = llvm_exp(exp);
	*/
	printf("\n;printi not yet implemented!!\n");
	str = NULL;//debugging
	return str;
}

char * llvm_prints(GNode * exp){
	need_puts = 1;// will cause a declaration of puts for external linking

	char* str = llvm_exp(exp);
	/*
	char* call_string;
	printf("%%c = alloca i8\n");
	printf("store i8 %s, i8* %%c\n", str);
	call_string = " = call i32 @puts(i8* %%c) ";	
	str = llvm_exp(exp);
	*/
	printf("\n;prints not yet implemented!!\n");
	str = NULL;//debugging
	return str;
	
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

		case LOOP_EXP:
			#ifdef LLVM_DBG_ON
				printf("llvm_exp: AT LOOP\n");	
			#endif
			str = llvm_loop( exp );
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


char* llvm_loop(GNode* loop){
	char* str;	
	int cnt_t = loop_count;

	printf("\nbr label %%loop%d \n", cnt_t);

	printf("\nloop%d:\n\n", loop_count++ );
	
	str = llvm_block( loop->children  );	

	printf("\nbr label %%loop%d \n", cnt_t );	
	
		
	return str;

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


