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



static int var_count = 1;	//Used for temporary variables, not rust declared variables
static int loop_count = 1;	//Will use for while and loop
static int cmp_count = 1;
static int ifelse_count = 1;
static GHashTable* count_table;


llvm_var* llvm_new( char* id , char* reg , char* label , struct type* type){
	
	llvm_var* new = malloc( sizeof(llvm_var) );	
	

	if(id){
		
		int id_len = strlen( id );
		new->id = malloc( sizeof( char ) * (id_len+1) );
		strcpy( new->id , id );	

	}


	if(reg){
		
		int reg_len = strlen( reg );
		new->reg = malloc( sizeof( char ) * (reg_len+1) );
		strcpy( new->reg , reg );	


	}


	if(label){

		int label_len = strlen( label );
		new->label = malloc( sizeof( char ) * (label_len+1) );
		strcpy( new->label , label );	

	}

	new->type = type;//Shallow copy of type

	return new;
	
}


void llvm_free( llvm_var* var){


	if(var->label)
		free(label);
	

	if(var->reg)
		free(reg);
	

	if(var->id)
		free(id);

	
}



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

char* llvm_block(GNode* block){

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
			default:
				str = llvm_exp(stmt);
				break;
		}
		
		stmt = stmt->next;

	}
		
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


llvm_var* llvm_left_exp(GNode* lexp){

	int kind = get_ast(lexp)->kind;
	llvm_var* var = NULL;

	switch(kind){
		
		case ID:
			var = llvm_id(lexp);
			break;
		case ARRIDX_EXP:
			var = llvm_left_arridx(lexp);
			break;
		case FIELDLUP_EXP:
			var = llvm_left_flup(lexp);
			break
		case DEREF_EXP:
			var = llvm_left_deref(lexp);
			break;
		default:
			#ifdef LLVM_DBG_ON
				printf("llvm_left_exp: Not a recognizable kind\n");
			#endif
		break;
	}

	return var;	

}

//IMPORTANT:
//	When an array is made defined, we keep an array pointer.  
//	e.g. If ID is 'X' then the pointer is 'X_ptr'
//	This allows for getelementptr for l-exp and loading for r-exp.

llvm_var* llvm_left_arridx(GNode* arr_idx){

	GNode* idx = arr_idx->children->next;
	GNode* arr = arr_idx->children;
	
	
	llvm_var* arr_var = llvm_left_exp( arr );	
	llvm_var* idx_var = llvm_exp( idx );
		
	char* arr_reg = arr_var->reg; 
	char* idx_reg = idx_var->reg;
     
	char* new_reg = malloc( 33 * sizeof(char) );
	
	new_reg[0] = '%';
    	itoa(var_count++ , &new_reg[1] , 10);		
	 	
	
	//Print new instruction	
	printf( "%s = getelementptr " , new_reg ); 
	llvm_print_type(  arr_var->type ); 	
	printf( "* , i32 0 , i32 %s\n", idx_reg);
	
		
	
	llvm_free( arr_var );
	llvm_free(idx_var);

	llvm_var* ret = llvm_new(arr_var->id , NULL   , NULL , get_type( arr_idx ) );	
	ret->reg = new_reg;
	

	return ret;

}

llvm_var* llvm_left_deref( GNode* deref ){

	//Printing will occur during assignment
	//	Print will be a "store" into this returned variable	
	//llvm_exp is called over exp since any expression can return a pointer
	return llvm_exp(deref->children);


}

llvm_var* llvm_left_flup( GNode* flup ){

	GNode* left_ch = flup->children;
	GNode* right_ch = left_ch->next;
	
	llvm_var* left_var = llvm_exp(left_ch);	
	llvm_var* righ_var = llvm_exp(right_ch);	
	
	char* left_reg = left_var->reg;	


	//Get Offset into the struct for the given field
	GNode* struct_def = get_type(left_ch)->params->parent->parent;		
	int offset = get_field_offset( struct_def  ,  right_var->id  );	
	

	//Create a new SSA register value
	char* new_reg = malloc( 33 * sizeof(char) );
	new_reg[0] = '%';
    	itoa(var_count++ , &new_reg[1] , 10);	
	
	
	//Print new instruction	
	printf( "%s = getelementptr " , new_reg ); 
	llvm_print_type( left_var->type ); 	
	printf( "* , i32 0 , i32 %d\n", offset );
	
		
	llvm_free( left_var );
	llvm_free( right_var );

	llvm_var* ret = llvm_new(left_var->id , NULL  , NULL , get_type( flup ) );	
	ret->reg = new_reg;
	

	return ret;
	
}


int llvm_field_offset(GNode* struct_def, char* id){
	
	int cnt = 0;	
	
	if(!id){
		#ifdef LLVM_DBG_ON
			printf("llvm_field_offset: id is NULL\n");
		#endif	
	}
	
	GNode* field = struct_def->children->children;
	
	while(field){
		
		char* cmp_id = get_ast( field->children  )->str;	
		
		if( !strcmp(id , cmp_id ) )	//if fields are equal
			break;
		cnt++;
		field = field->next;	
	}

	return cnt;

}


//Unfinished, must complete left expressions first
llvm_var* llvm_assign( GNode* ass){
	
	GNode* left = ass->children;
	GNode* right = left->next;
	
	llvm_var* r_var = NULL;
	llvm_var* l_var = NULL;
	
	int l_type;
	int kind = get_ast(ass)->kind;

	switch(kind){

		case ASSIGNMENT:
			l_var = llvm_left_exp(left);
			r_var = llvm_exp(right);
			l_type = strip_mut( l_var->type )->kind;			
						

			if(	get_ast(left)->kind == ID && 
				( l_type == TYPE_I32 || l_type == TYPE_BOOL ) 
			  )
			{
				
				//print( %var_count++ = or <r-ty> <r-reg> , 0      )			
				//				
	
			}
			else{
			
						
			
			}
			break;

		case PLUS_ASSIGN:
			l_var = llvm_left_exp(left);
			r_var = llvm_exp(right);				
		
			break;

		case SUB_ASSIGN:
			l_var = llvm_left_exp(left);
			r_var = llvm_exp(right);
			
			break;
	
		case MUL_ASSIGN:
			l_var = llvm_left_exp(left);
			r_var = llvm_exp(right);
			
			break;

		case DIV_ASSIGN:
			l_var = llvm_left_exp(left);
			r_var = llvm_exp(right);
			
			break;

		case REM_ASSIGN:
			l_var = llvm_left_exp(left);
			r_var = llvm_exp(right);
			
			break;

		default:
			#ifdef LLVM_DBG_ON
				printf("llvm_assign: Kind not recognized.\n");
			#endif
			break;

	}



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
	buff[31] = '\0';
	buff[32]
	for(; val && i ; --i, val /= base)
	
		buf[i] = "0123456789abcdef"[val % base];
	
	strcpy( usr_buff ,  &buf[i+1] );
		

	return usr_buff;
	
}


