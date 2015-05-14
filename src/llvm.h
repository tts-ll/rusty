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
llvm_var* 		llvm_block(GNode*);
llvm_var*		llvm_structdef(GNode*);

//llvm_var*		llvm_let();
llvm_var*		llvm_let_type();
llvm_var*		llvm_let_ass();
llvm_var*		llvm_let_type_ass();

//Left expressions: UNDER CONSTRUCTION
llvm_var*		llvm_left_exp(GNode*);
llvm_var*		llvm_left_arridx(GNode*);
llvm_var*		llvm_left_deref(GNode*);
llvm_var*		llvm_left_flup(GNode*);


llvm_var* 		llvm_exp(GNode*);
llvm_var*		llvm_exp_no_load(GNode*);
llvm_var* 		llvm_loop(GNode*);
llvm_var*		llvm_while(GNode*);
llvm_var*		llvm_ifelse(GNode*);
llvm_var* 		llvm_binary_op(GNode*);
llvm_var* 		llvm_comparison(GNode*);
llvm_var*		llvm_assign(GNode*);
llvm_var*		llvm_arridx(GNode*);
llvm_var*		llvm_deref(GNode*);
llvm_var*		llvm_flup(GNode*);
llvm_var*		llvm_fncall(GNode*);


llvm_var* 		llvm_id(GNode*);
llvm_var* 		llvm_litdec(GNode*);
llvm_var*		llvm_litbool(GNode*);
llvm_var*		llvm_litarr(GNode*);
llvm_var*		llvm_litstruct(GNode*);


//Get the offset of a field within a structure
int 			llvm_field_offset( GNode* struct_def , char* field_id);


//Print the type of a given llvm variable
void			llvm_print_type(struct type*);

//Fill and return a string filled with a string representation of the type
char*			llvm_type_str(struct type* , char* );

//Convert an integer into a character
char* 			itoa(int num, char * buf, int radix);


//Constructor/Destructor
llvm_var* 		llvm_new( char* , char* , char* , struct type*);
void 			llvm_free( llvm_var* );



#endif /*LLVM_H*/
