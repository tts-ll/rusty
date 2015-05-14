#include <stdio.h>
#include <glib.h>
#include <stdlib.h>
#include <string.h>


#include "llvm.h"
#include "ast.h"
#include "env.h"
#include "parser.h"
#include "type_annotate.h"

#define LLVM_DBG_ON


//Tyler's branch
//Insert binary ops to llvm



static int var_count = 1;	//Used for temporary variables, not rust declared variables
static int loop_count = 1;	//Will use for while and loop
//static int cmp_count = 1;
static int if_count = 1;
//static GHashTable* count_table;


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

/*
	if(var->label)
		free(var->label);
	

	if(var->reg)
		free(var->reg);
	

	if(var->id)
		free(var->id);
	
	free(var);
*/
	
}



//Just for Testing
//Gets us to a function's block so that we can test
void test_llvm(GNode* crate){

	

	GNode *item = crate->children->children;
	GNode *block;
	
	llvm_var* var = NULL;

	//Find the function definition
	while(item){

	
			
		if( get_ast(item)->kind == STRUCTDEF_ITEM)
			llvm_structdef(item);	
		else if(get_ast(item)->kind == FNDEF_ITEM && !strcmp( get_ast(item->children)->str , "main")  ){
				#ifdef LLVM_DBG_ON	
					printf("test_llvm: FOUND FUNCTION\n");
				#endif
			break;
		}
		//else 
		item = item->next;
	}



	block = item->children;
	
	//Get the block
	while(block){

		if(get_ast(block)->kind == BLOCK)
			break;
		block = block->next;
	}
	
	var = llvm_block( block );	
	
	#ifdef LLVM_DBG_ON
	fputs("test_llvm: Leaving function\n",stderr);
	#endif	
}

llvm_var* llvm_structdef(GNode* struct_def){

					//struct_def->id->fields->field
	GNode* field = struct_def->children->next->children;	
	char* id = get_ast( struct_def->children )->str;

	printf("%%struct.%s = type {" , id);	
	
	
	while(field){

		llvm_print_type( strip_mut( get_type(field) )  );						
		
		field = field->next;
		if(field) printf(",");
			
	}
	printf("}\n");
	return NULL;

}


llvm_var* llvm_block(GNode* block){

	GNode* stmt = block->children;
	llvm_var* var = NULL;	

	while(stmt){

		switch( get_ast(stmt)->kind ){
	
			case STMT:
				var = NULL; 
				llvm_exp(stmt->children);
				break;
			case RETURNEXP_STMT:
				var = llvm_ret_exp( stmt );
				break;
			case RETURN_STMT:
				var = llvm_ret(stmt);
				break;
			case LET_STMT:
				break;
			case LETTYPEASS_STMT:
				llvm_let_type_ass(stmt);	
				break;
			case LETASS_STMT:
				llvm_let_ass( stmt );
				break;
			case LETTYPE_STMT:
				var = llvm_let_type( stmt );
				break;
			default:
				var = llvm_exp(stmt);
				break;
		}
		
		stmt = stmt->next;

	}
		
	return var;
}

llvm_var* llvm_ret(GNode* ret){

	printf("ret void \n");

	return NULL;
}

llvm_var* llvm_ret_exp(GNode* ret){
	
	llvm_var* var = llvm_exp(ret->children);
	
	printf("ret ");
	llvm_print_type( strip_mut( var->type ) );
	printf(" %s \n", var->reg );

	return NULL;
}

llvm_var*	llvm_let_type(GNode* let){

	GNode* id_node = let->children->children;		
	struct type* var_type = strip_mut( get_type(id_node) );
	char* id = get_ast(id_node)->str;


	printf("%%%s_ptr = alloca ", id  );
	llvm_print_type( var_type );
	printf("\n");	

	return NULL;
}

llvm_var* llvm_let_ass(GNode* let){

	GNode* id_node = let->children->children;
	GNode* exp = let->children->next;
		
	struct type* var_type = strip_mut( get_type(id_node) );
	char* id = get_ast(id_node)->str;
	
	llvm_var* ass_var = llvm_exp( exp );

	printf("%%%s_ptr = alloca ", id  );
	llvm_print_type( var_type );
	printf("\n");	

	printf("store ");
	llvm_print_type( var_type );
	printf(" %s , ", ass_var->reg );
	llvm_print_type( var_type );
	printf("* %%%s_ptr \n" , id );
	
	return NULL;

}

llvm_var* llvm_let_type_ass(GNode* let){


	GNode* id_node = let->children->children;
	GNode* exp = let->children->next->next;
		
	struct type* var_type = strip_mut( get_type(id_node) );
	char* id = get_ast(id_node)->str;
	
	llvm_var* ass_var = llvm_exp( exp );


	printf("%%%s_ptr = alloca ", id  );
	llvm_print_type( var_type );
	printf("\n");	

	printf("store ");
	llvm_print_type( var_type );
	printf(" %s , ", ass_var->reg );
	llvm_print_type( var_type );
	printf("* %%%s_ptr \n" , id );
	
	return NULL;


}


llvm_var* llvm_exp(GNode *exp){

	int exp_kind = get_ast( exp )->kind;
	llvm_var* var;
	llvm_var* new;
	char reg[100];

	switch(exp_kind){
	
		case ADD_EXP:
		case SUB_EXP:
		case MUL_EXP:
		case DIV_EXP:
		case REM_EXP:
		case AND_EXP:
		case OR_EXP:
			var = llvm_binary_op( exp );
			break;
		
		case EQ_EXP:
		case NE_EXP:	
		case LT_EXP:
		case GT_EXP:
		case LE_EXP:
		case GE_EXP:
			var = llvm_comparison( exp );
			break;

		case ASSIGNMENT:
		case PLUS_ASSIGN:
		case SUB_ASSIGN:
		case MUL_ASSIGN:
		case DIV_ASSIGN:
		case REM_ASSIGN:
			var = llvm_assign( exp );
			break;
	
		case FNCALL_EXP:
			var = llvm_fncall( exp );
			break;
	
		case ARRIDX_EXP:
			#ifdef LLVM_DBG_ON
			fputs("llvm_exp: In arr_idx\n", stderr);		
			#endif
			var = llvm_arridx( exp );
			break;
		
		case FIELDLUP_EXP:
			var = llvm_flup( exp );
			break;
		
		case DEREF_EXP:
			var = llvm_deref( exp );
		
			//Initialize new SSA register
			reg[0] = '%';
			itoa(var_count++ , &reg[1] , 10);
			
			//New variable
			new = llvm_new( get_ast(exp)->str , &reg[0] , NULL , strip_mut(var->type) );
			
			//Load variable
			printf("%s = load ", new->reg);
			llvm_print_type( strip_mut( var->type ) );
			printf("* %s \n", var->reg);
			
			//llvm_free(var);
			return new;

			break;	
		
		case LOOP_EXP:
			#ifdef LLVM_DBG_ON
				printf("llvm_exp: AT LOOP\n");	
			#endif
			var = llvm_loop( exp );
			break;

		case WHILE_EXP:	
		 	var = llvm_while( exp );
			break;
		
		case IF_EXP:
			var = llvm_ifelse( exp );
			break; 

		case ID:
			var = llvm_id( exp );	
			//print load instruction
			reg[0] = '%';
			itoa(var_count++ , &reg[1] , 10);
			
			new = llvm_new( get_ast(exp)->str , &reg[0] , NULL , strip_mut(var->type) );
			
			printf("%s = load ", new->reg);
			llvm_print_type( strip_mut( var->type ) );
			printf("* %s \n", var->reg);
			
			//llvm_free(var);
			return new;

			break;
		
		case TRU:
		case FALS:
			var = llvm_litbool( exp );
			break;

		case LITDEC:
			var = llvm_litdec( exp );
			break;
		case ARRAY_LIT:
			var = llvm_litarr( exp );		
			break;
		
		case STRUCT_LIT:
			var = llvm_litstruct( exp );
			break;

		default:
			var = NULL;
	}


	return var;


}


llvm_var* llvm_exp_no_load(GNode *exp){

	int exp_kind = get_ast( exp )->kind;
	llvm_var* var;
	
//	char reg[100];

	switch(exp_kind){
	
		case ADD_EXP:
		case SUB_EXP:
		case MUL_EXP:
		case DIV_EXP:
		case REM_EXP:
		case AND_EXP:
		case OR_EXP:
			var = llvm_binary_op( exp );
			break;
		
		case EQ_EXP:
		case NE_EXP:	
		case LT_EXP:
		case GT_EXP:
		case LE_EXP:
		case GE_EXP:
			var = llvm_comparison( exp );
			break;

		case ASSIGNMENT:
		case PLUS_ASSIGN:
		case SUB_ASSIGN:
		case MUL_ASSIGN:
		case DIV_ASSIGN:
		case REM_ASSIGN:
			var = llvm_assign( exp );
			break;
		
		case FNCALL_EXP:
			var = llvm_fncall( exp );
			break;

		case ARRIDX_EXP:
			#ifdef LLVM_DBG_ON
			fputs("llvm_exp: In arr_idx\n", stderr);		
			#endif

			var = llvm_arridx( exp );
			break;
		
		case FIELDLUP_EXP:
			var = llvm_flup( exp );
			
			break;
		
		case DEREF_EXP:
			var = llvm_deref( exp );
			break;	
		
		case LOOP_EXP:
			#ifdef LLVM_DBG_ON
				printf("llvm_exp: AT LOOP\n");	
			#endif
			var = llvm_loop( exp );
			break;

		case WHILE_EXP:
			var = llvm_while( exp );
			break;
	
		case IF_EXP:
			var = llvm_ifelse( exp );
			break; 
	
		case ID:
			var = llvm_id( exp );	
	
			break;
		
		case LITDEC:
			var = llvm_litdec( exp );
			break;
	
		case TRU:
		case FALS:
			var = llvm_litbool( exp );
			break;
			
		case ARRAY_LIT:
			var = llvm_litarr( exp );
			break;
		
		case STRUCT_LIT:
			var = llvm_litstruct( exp );
			break;
		
		default:
			var = NULL;
	}


	return var;


}



//Similar to expressions except that it returns a pointer
llvm_var* llvm_left_exp(GNode* lexp){

	int kind = get_ast(lexp)->kind;
	llvm_var* var = NULL;

	switch(kind){
		
		case ID:
			var = llvm_id( lexp );
			break;
		case ARRIDX_EXP:
			var = llvm_left_arridx( lexp );
			break;
		case FIELDLUP_EXP:
			var = llvm_left_flup( lexp );
			break;
		case DEREF_EXP:
			var = llvm_left_deref( lexp );
			break;
		default:
			#ifdef LLVM_DBG_ON
				printf("llvm_left_exp: Not a recognizable left-exp\n");
			#endif
		break;
	}

	return var;	

}

llvm_var*	llvm_fncall(GNode* fncall){

	GNode* param = fncall->children->next->children;
	GNode* save = param;
	struct type* ret_type = get_type(fncall);
	char* id = get_ast(fncall->children)->str;
	char reg[50];
	//char param_str[1000];
	llvm_var* param_vars[100];		
	int i = 0;	
	
	//Initialize Return register value
	reg[0] = '%';	
	itoa( var_count++ , &reg[1] , 10 );
	llvm_var* ret_var = llvm_new( NULL , &reg[0] , NULL , strip_mut( ret_type ) );


	//Retrieve parameters
	while(param){
				
		param_vars[i] = llvm_exp(param);		
		
		i++;	
		param = param->next;
	}
	
	#ifdef LLVM_DBG_ON
	fprintf(stderr , "llvm_fncall: Retrieved paramters\n");
	#endif 
	
	//Print call instruction
	if( ret_type->kind == TYPE_UNIT )
		printf("call void @%s( " , id );
	else{
		printf("%s = call " , &reg[0]);
		llvm_print_type( ret_var->type );
		printf(" @%s( ", id );
	}
	
	#ifdef LLVM_DBG_ON
	fprintf(stderr , "llvm_fncall: Passed print call\n");
	#endif 
	
	//Print parameters
	param = save;
	i = 0;
	while(param){
		
		llvm_print_type( strip_mut( param_vars[i]->type )  );
	
		printf(" %s" , param_vars[i]->reg );
	
			i++;					
		param = param->next;
		if(param)
			printf(", ");
	}	

					
	printf(")\n");

	
	return ret_var;
}


//IMPORTANT:
//	When an array is made defined, we keep an array pointer.  
//	e.g. If ID is 'X' then the pointer is 'X_ptr'
//	This allows for getelementptr for l-exp and loading for r-exp.

llvm_var* llvm_arridx(GNode* arr_idx){

	GNode* idx = arr_idx->children->next;
	GNode* arr = arr_idx->children;
	
	
	llvm_var* arr_var = llvm_exp( arr );	
	llvm_var* idx_var = llvm_exp( idx );
		
	char* arr_reg = arr_var->reg; 
	char* idx_reg = idx_var->reg;
     
	char* new_reg = malloc( 33 * sizeof(char) );
	
	new_reg[0] = '%';
    itoa(var_count++ , &new_reg[1] , 10);		
	 	
	
	//Print new instruction	
	printf( "%s = extractvalue " , new_reg ); 
	llvm_print_type(  strip_mut(arr_var->type)  ); 	
	printf( " %s ,  %s\n", arr_reg , idx_reg  );
	
		
	
	llvm_var* ret = llvm_new(arr_var->id , NULL   , NULL , strip_mut( get_type( arr_idx ))  );	
	ret->reg = new_reg;

	llvm_free( arr_var );
	llvm_free(idx_var);
	

	return ret;

}


llvm_var* llvm_deref( GNode* deref ){

	//Printing will occur during assignment
	//	Print will be a "store" into this returned variable	
	//llvm_exp is called over exp since any expression can return a pointer
	 llvm_var* var = llvm_exp( deref->children );
	 var->type =  strip_mut( get_type( deref ) );

	 return var;

}




llvm_var* llvm_flup( GNode* flup ){

	GNode* left_ch = flup->children;
	GNode* right_ch = left_ch->next;
	
	llvm_var* left_var = llvm_exp(left_ch);	
	llvm_var* right_var = llvm_exp_no_load(right_ch);	
	
	char* left_reg = left_var->reg;	


	//Get Offset into the struct for the given field
	GNode* struct_def = get_type(left_ch)->params->parent->parent;		
	int offset = llvm_field_offset( struct_def  ,  right_var->id  );	
	

	//Create a new SSA register value
	char* new_reg = malloc( 33 * sizeof(char) );
	new_reg[0] = '%';
    itoa(var_count++ , &new_reg[1] , 10);	
	
	
	//Print new instruction	
	printf( "%s = extractvalue " , new_reg ); 
	llvm_print_type( strip_mut( left_var->type ) ); 	
	printf( " %s , %d\n", left_reg , offset );
	
	llvm_var* ret = llvm_new(left_var->id , NULL  , NULL , strip_mut( get_type( flup ) ) );	
	ret->reg = new_reg;
		
	llvm_free( left_var );
	llvm_free( right_var );



	return ret;
	
}


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
	llvm_print_type(  strip_mut(arr_var->type)  ); 	
	printf( "* %s , i32 0 , i32 %s\n", arr_reg , idx_reg  );
	
		
	llvm_var* ret = llvm_new(arr_var->id , NULL   , NULL , strip_mut( get_type( arr_idx ))  );	
	ret->reg = new_reg;
	
	llvm_free( arr_var );
	llvm_free(idx_var);



	return ret;

}

//Needs work
llvm_var* llvm_left_deref( GNode* deref ){

	 llvm_var* var = llvm_left_exp( deref->children );
	 var->type =  strip_mut( get_type( deref ) );

	 return var;

}




llvm_var* llvm_left_flup( GNode* flup ){

	GNode* left_ch = flup->children;
	GNode* right_ch = left_ch->next;
	
	llvm_var* left_var = llvm_left_exp(left_ch);	
	llvm_var* right_var = llvm_left_exp(right_ch);	
	
	char* left_reg = left_var->reg;	


	//Get Offset into the struct for the given field
	GNode* struct_def = get_type(left_ch)->params->parent->parent;		
	int offset = llvm_field_offset( struct_def  ,  right_var->id  );	
	

	//Create a new SSA register value
	char* new_reg = malloc( 33 * sizeof(char) );
	new_reg[0] = '%';
    itoa(var_count++ , &new_reg[1] , 10);	
	
	
	//Print new instruction	
	printf( "%s = getelementptr " , new_reg ); 
	llvm_print_type( strip_mut( left_var->type ) ); 	
	printf( "* %s , i32 0 , i32 %d\n", left_reg , offset );
	

	llvm_var* ret = llvm_new(left_var->id , NULL  , NULL , strip_mut( get_type( flup ) ) );	
	ret->reg = new_reg;

			
	llvm_free( left_var );
	llvm_free( right_var );



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
	
	
	int kind = get_ast(ass)->kind;
	
	char* r_reg = NULL;
	char* l_reg = NULL;
	char reg_t[50];
	char reg_tt[50];
	switch(kind){

		case ASSIGNMENT:
			l_var = llvm_left_exp(left);	//Pointer value
			r_var = llvm_exp(right);		//Not necessarily a pointer value	
			l_reg = l_var->reg;
			r_reg = r_var->reg;
			

			break;

		case PLUS_ASSIGN:
			l_var = llvm_left_exp(left);
			r_var = llvm_exp(right);				
			r_reg = r_var->reg;
			l_reg = l_var->reg;
			
			//Print load pointer from left-exp		
			reg_t[0] = '%';	
			itoa( var_count++ , &reg_t[1] , 10  );		
			printf("%s = load ", reg_t);	
			llvm_print_type( strip_mut( l_var->type )  );
			printf("* %s\n" , l_reg );
			
			//Print add for addition portion
			reg_tt[0] = '%';	
			itoa( var_count++ , &reg_tt[1] , 10  );	
			printf("%s = add i32 %s , %s \n", reg_tt , reg_t , r_reg );
			
			//reg_tt's (now r_reg's) value will be stored below
			strcpy(r_reg , reg_tt);		
	
			break;

		case SUB_ASSIGN:
			l_var = llvm_left_exp(left);
			r_var = llvm_exp(right);
			r_reg = r_var->reg;
			l_reg = l_var->reg;
			
			//Print load pointer from left-exp		
			reg_t[0] = '%';	
			itoa( var_count++ , &reg_t[1] , 10  );		
			printf("%s = load ", reg_t);	
			llvm_print_type( strip_mut( l_var->type )  );
			printf("* %s\n" , l_reg );
			
			//Print add for subtraction portion
			reg_tt[0] = '%';	
			itoa( var_count++ , &reg_tt[1] , 10  );	
			printf("%s = sub i32 %s , %s \n", reg_tt , reg_t , r_reg );
			
			//reg_tt's (now r_reg's) value will be stored below
			strcpy(r_reg , reg_tt);		

			break;
	
		case MUL_ASSIGN:
			l_var = llvm_left_exp(left);
			r_var = llvm_exp(right);
			r_reg = r_var->reg;
			l_reg = l_var->reg;
			
			//Print load pointer from left-exp		
			reg_t[0] = '%';	
			itoa( var_count++ , &reg_t[1] , 10  );		
			printf("%s = load ", reg_t);	
			llvm_print_type(  strip_mut( l_var->type )  );
			printf("* %s \n" , l_reg );
			
			//Print add for multiplication portion
			reg_tt[0] = '%';	
			itoa( var_count++ , &reg_tt[1] , 10  );	
			printf("%s = mul i32 %s , %s \n", reg_tt , reg_t , r_reg );
			
			//reg_tt's (now r_reg's) value will be stored below
			strcpy(r_reg , reg_tt);	
			break;

		case DIV_ASSIGN:
			l_var = llvm_left_exp(left);
			r_var = llvm_exp(right);
			r_reg = r_var->reg;
			l_reg = l_var->reg;
			
			//Print load pointer from left-exp		
			reg_t[0] = '%';	
			itoa( var_count++ , &reg_t[1] , 10  );		
			printf("%s = load ", reg_t);	
			llvm_print_type( strip_mut( l_var->type )  );
			printf("* %s\n" , l_reg );
			
			//Print add for division portion
			reg_tt[0] = '%';	
			itoa( var_count++ , &reg_tt[1] , 10  );	
			printf("%s = sdiv i32 %s , %s \n", reg_tt , reg_t , r_reg );
			
			//reg_tt's (now r_reg's) value will be stored below
			strcpy(r_reg , reg_tt);	
			break;

		case REM_ASSIGN:
			l_var = llvm_left_exp(left);
			r_var = llvm_exp(right);
			r_reg = r_var->reg;
			l_reg = l_var->reg;
			
			//Print load pointer from left-exp		
			reg_t[0] = '%';	
			itoa( var_count++ , &reg_t[1] , 10  );		
			printf("%s = load ", reg_t);	
			llvm_print_type( strip_mut( l_var->type )  );
			printf("* %s\n" , l_reg );
			
			//Print add for remainder portion
			reg_tt[0] = '%';	
			itoa( var_count++ , &reg_tt[1] , 10  );	
			printf("%s = srem i32 %s , %s \n", reg_tt , reg_t , r_reg );
			
			//reg_tt's (now r_reg's) value will be stored below
			strcpy(r_reg , reg_tt);
	
			break;

		default:
			#ifdef LLVM_DBG_ON
				printf("llvm_assign: Operation not recognized.\n");
			#endif
			break;

	}
	
	//Print store function for the assignment
	//	i.e. storing in the address pointed to by the l-var pointer
	//Store <r-type> <r-reg> , <l-type>* <l-reg>
	printf("store ");
	llvm_print_type( strip_mut( r_var->type ) );
	printf(" %s , ", r_reg);
	llvm_print_type( strip_mut( l_var->type ) );
	printf("* %s \n" , l_reg);		



	llvm_free(l_var);
	llvm_free(r_var);

	return NULL;
} 



llvm_var* llvm_binary_op(GNode *op){
	
	
		
	int op_kind = get_ast(op)->kind;
	char op_string[500];

	GNode *left = op->children;
	GNode *right = op->children->next;
	
	
	char* var = (char *)malloc(21 * sizeof(char) );
	
	llvm_var* ret = llvm_new( NULL , NULL , NULL ,strip_mut( get_type(op) ) );
	ret->reg = var;

	//Get return strings for further down expressions
	llvm_var *var_left  = llvm_exp(left);
	llvm_var *var_right = llvm_exp(right);
	char* str_left = var_left->reg;	
	char* str_right = var_right->reg;

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

	llvm_free(var_left);
	llvm_free(var_right);

	

	return ret;
	
}




llvm_var* llvm_loop(GNode* loop){



	llvm_var* var;		
	int cnt_t = loop_count;
	
	printf("\nLOOP%d:\n\n", loop_count++ );
	
	var = llvm_block( loop->children  );	

	printf("\nbr label %%LOOP%d \n", cnt_t );	
	
		
	return var;

}


llvm_var* llvm_while(GNode* loop){
	
	GNode* loop_eval = loop->children;
	GNode* loop_block = loop_eval->next;

	llvm_var* eval;
	llvm_var* block;
	
	int cntr = loop_count; 
	loop_count++;

	printf("WHILE%d:\n", cntr);	
	eval = llvm_exp(loop_eval);
	printf("br i1 %s , label %%LOOP%d , label %%JOIN%d \n", eval->reg , cntr , cntr );		
	printf("LOOP%d:\n" , cntr );
	block = llvm_block(loop_block);
	printf("br label %%WHILE%d \n" , cntr );			
	printf("JOIN%d:\n" , cntr );

	return block;
}


llvm_var* llvm_ifelse(GNode* ifelse){
	
	GNode* eval_exp = ifelse->children;
	GNode* if_block = eval_exp->next;
	GNode* else_block = if_block->next;	
	
	llvm_var* eval;
	llvm_var* if_var = NULL ;
	llvm_var* else_var = NULL;
	
	
	char* jmp_label;

	if(else_block)
		jmp_label = "ELSE";		
	else
		jmp_label = "END";
	int cnt = if_count;
	if_count++;
	
	
	printf("IF%d:\n" , cnt );
		
	eval = llvm_exp( eval_exp );	
	printf("br i1 %s , label %%THEN%d , label %%%s%d \n" , eval->reg , cnt , jmp_label , cnt );
	
	//Print IF BLOCK
	printf("THEN%d:\n", cnt );	
	if_var = llvm_block( if_block );
	printf("br label %%END%d \n" , cnt );
	

	//Print ELSE BLOCK
	if(else_block){
		printf("ELSE%d:\n" , cnt);
		else_var = llvm_block( else_block );
		printf("br label %%END%d \n" , cnt );	

	}	
	
	//Print end label
	printf("END%d:\n" , cnt );
	
	//Print phi statement for blocks ??
	

	return NULL;

}


llvm_var* llvm_comparison(GNode* cmp){

	int cmp_kind = get_ast(cmp)->kind;
		
	
	char cmp_string[500];

	GNode *left = cmp->children;
	GNode *right = cmp->children->next;
	
	char* var = (char *)malloc(21 * sizeof(char) );
	llvm_var* ret = llvm_new(NULL , NULL , NULL , strip_mut( get_type(cmp) ) );	
	ret->reg = var;

	llvm_var* var_left = llvm_exp(left);
	llvm_var* var_right = llvm_exp(right);
	
	//Get return strings for further down expressions
	char *str_left  = var_left->reg;
	char *str_right = var_right->reg;
	
	//Append llvm return variable to op_string
	var[0] = '%';
	itoa( var_count++ , ( var + 1 ) , 10 );	
	strcpy( cmp_string , var );
	

	
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

	return ret;

}

llvm_var* llvm_id(GNode *id_node){
	
	llvm_var* var;
		
	char* id_t = get_ast(id_node)->str; 
		
	char reg[100]; 
	reg[0] = '%';	
		
	strcpy(&reg[1], id_t);	
	strcat(&reg[0] , "_ptr");
	
	var = llvm_new(id_t , &reg[0] , NULL , strip_mut(get_type(id_node)) );
	return var ;
	

}

llvm_var* llvm_litarr(GNode* arr){

	GNode* elmt = arr->children;
	GNode* save = elmt;
	struct type* arr_type = strip_mut( get_type(arr)  );	

	char reg[50];
	llvm_var* var[1000];
	int i = 0;

	
	
	
	while(elmt){
		
		var[i] = llvm_exp( elmt );			
	
		i++;
		elmt = elmt->next;
				
	}

	//Build initial insert variable instruction
	//%s = insertvalue %struct.id undef , type reg , index
	printf("%%%d = insertvalue  ", var_count++ );
	llvm_print_type( arr_type );
	printf(" undef , ");
	llvm_print_type( strip_mut( var[0]->type )  );	
	printf(" %s , 0 \n" , var[0]->reg );
	
	elmt = save->next;
	i = 1;
	
	//Build following insert variable instructions for each field
	while(elmt){

		printf("%%%d = insertvalue  ", var_count );
		llvm_print_type( arr_type );
		printf(" %%%d , ", var_count-1);
		llvm_print_type( strip_mut( var[1]->type )  );	
		printf(" %s , %d \n" , var[1]->reg , i );
		var_count++;

		i++;
		elmt = elmt->next;
	
	}
	
	reg[0] = '%';
	itoa(var_count-1 , &reg[1] , 10);
		
	return llvm_new(NULL , &reg[0]  , NULL , arr_type );
}

llvm_var* llvm_litstruct(GNode* litstruct){
	
	GNode* field = litstruct->children->next->children;	
	GNode* exp = NULL;
	char* id = get_ast(litstruct->children)->str;
	char reg[50];
	GNode* save = field;
	
	
	
	llvm_var* vars[1000];
	int i = 0;
	
	//Obtain SSA register/types for each element
	while(field){
		exp = field->children->next;
		
		vars[i] = llvm_exp( exp );
			
		
		i++;
		field = field->next;
	}


	//Build initial insert variable instruction
	//%s = insertvalue %struct.id undef , type reg , index
	printf("%%%d = insertvalue %%struct.%s undef , ", var_count++ , id );
	llvm_print_type( strip_mut( vars[0]->type )  );	
	printf(" %s , 0 \n" , vars[0]->reg );
	
	field = save->next;
	i = 1;
	
	//Build following insert variable instructions for each field
	while(field){

		printf("%%%d = insertvalue %%struct.%s %%%d , ", var_count , id , var_count-1);
		var_count++;
		llvm_print_type( strip_mut( vars[1]->type )  );	
		printf(" %s , %d \n" , vars[1]->reg , i );

		i++;
		field = field->next;
	
	}
	
	reg[0] = '%';
	itoa(var_count-1 , &reg[1] , 10);
		
	return llvm_new(NULL , &reg[0]  , NULL , strip_mut(get_type( litstruct ) ) );

}


llvm_var* llvm_litdec(GNode *litdec){

	int num = get_ast( litdec )->num;
	char num_str[100]; 
	llvm_var* ret;	
	itoa(num , &num_str[0] , 10);
		
	
	ret = llvm_new(NULL , &num_str[0] , NULL , strip_mut( get_type(litdec) ) );
		
	//printf("\n%s\n" , ret->reg);	

	return ret;

}

llvm_var* llvm_litbool(GNode* litbool){

	int kind = get_ast(litbool)->kind;
	char* b;

	if(kind == TRU)	
		b = "1";
	else
		b = "0";	

	return llvm_new( NULL , b , NULL , strip_mut( get_type(litbool) ) );

}



char* itoa(int val, char* usr_buff , int base){
	
	if(!usr_buff)
		return NULL;

	static char buf[32] = {0};
	
	int i = 30;
	buf[31] = '\0';
	if(val == 0){
		buf[30] = '0';
		i = 29;  
	}
	else
		for(; val && i ; --i, val /= base)
			buf[i] = "0123456789abcdef"[val % base];
	
	strcpy( usr_buff ,  &buf[i+1] );
		

	return usr_buff;
	
}


//I don't think this is used anymore
char* llvm_type_str(struct type* type , char* buf){
	int strt;
	if(!type)return buf;

	switch(type->kind){
		case TYPE_INVALID:
			sprintf(buf , "invalid" ); 
			break;
		case TYPE_OK:
			sprintf(buf , "ok!");
			break;
		case TYPE_ERROR:
			sprintf(buf , "ERROR!");		
			break;		
		case TYPE_UNIT:
			sprintf(buf , "()");
			break;		
		case TYPE_I32:
			sprintf(buf , "i32");
			break;
		case TYPE_U8:
			sprintf(buf , "i8");
			break;
		case TYPE_BOOL:
			sprintf(buf , "i1");
			break;
		case TYPE_ARRAY:
 			sprintf(buf , "[%d x " , type->length);
			strt = strlen(buf);	
			llvm_type_str( type->type, &buf[strt] );
			strt = strlen(buf);
			sprintf(&buf[strt] ,  "]");
			break;
		case TYPE_REF:
			llvm_type_str(type->type , buf);
			strt = strlen(buf);
			sprintf(&buf[strt] ,  "]");
			break;
		case TYPE_SLICE:
			sprintf(buf , "[");
			llvm_type_str(type->type , &buf[1]);
			strt = strlen(buf);
			sprintf(&buf[strt] ,  "]");
			break;
 		case TYPE_BOX:
			llvm_type_str(type->type , buf);
			strt = strlen(buf);
			sprintf(&buf[strt] , "*");
			break;
		case TYPE_FN:
			// TODO but i aint gonna
			sprintf(buf , "fn (TODO) -> TODO");
			break;
		case TYPE_ID:
			sprintf(buf , "%%struct.%s", type->id);
			break;
		case TYPE_MUT:
			sprintf(buf , " MUT SHOULD BE STRIPPED!!!");
			break;
		default:
			sprintf(buf , "HALT:  error in typing engine!\n");
		
	}

	return buf;

}

void llvm_print_type( struct type* type ){

	if(!type)return;

	switch(type->kind){
		case TYPE_INVALID:
			printf("invalid"); 
			break;
		case TYPE_OK:
			printf("ok!");
			break;
		case TYPE_ERROR:
			printf("ERROR!");		
			break;		
		case TYPE_UNIT:
			printf("()");
			break;		
		case TYPE_I32:
			printf("i32");
			break;
		case TYPE_U8:
			printf("i8");
			break;
		case TYPE_BOOL:
			printf("i1");
			break;
		case TYPE_ARRAY:
 			printf("[%d x ", type->length);	
			llvm_print_type(type->type);
			printf("]");
			break;
		case TYPE_REF:
			llvm_print_type(type->type);
			printf("*");
			break;
		case TYPE_SLICE:
			printf("[");
			llvm_print_type(type->type);
			printf("]");
			break;
 		case TYPE_BOX:
			llvm_print_type(type->type);
			printf("*");
			break;
		case TYPE_FN:
			// TODO but i aint gonna
			printf("fn (TODO) -> TODO");
			break;
		case TYPE_ID:
			printf("%%struct.%s", type->id);
			break;
		case TYPE_MUT:
			printf(" MUT SHOULD BE STRIPPED!!!");
			llvm_print_type(type->type);
			break;
		default:
			printf("HALT:  error in typing engine!\n");
		
	}
}
