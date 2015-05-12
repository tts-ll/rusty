#ifndef LLVM_H
#define LLVM_H

#include "type_annotate.h"
//Tyler's branch version



typedef struct llvm_var{

	struct type* type;		//The type of the variable
	char* id;				//The corresponding rust variable
	char* reg;				//The virtual  SSA (SingleSaticAssignment) register
	char* label;			//The label/basic-block the var belongs to
	
} llvm_var;

void 			test_llvm(GNode*);
llvm_var** 		llvm_block(GNode*);

//Left expressions
llvm_var*		llvm_left_exp(GNode*);
llvm_var*		llvm_left_arridx(GNode*);
llvm_var*		llvm_left_deref(GNode*);
llvm_var*		llvm_left_flup(GNode*);


llvm_var* 		llvm_exp(GNode*);
llvm_var* 		llvm_loop(GNode*);
llvm_var* 		llvm_binary_op(GNode*);
llvm_var* 		llvm_comparison(GNode*);
llvm_var*		llvm_assign(GNode*);
llvm_var* 		llvm_id(GNode*);
llvm_var* 		llvm_litdec(GNode*);


void			llvm_print_type(struct type*);

//Convert an integer into a character
char* 			itoa(int num, char * buf, int radix);


//Constructor/Destructor
llvm_var* 		llvm_new( char* , char* , char* , struct type*);
void 			llvm_free( llvm_var* );



#endif /*LLVM_H*/
