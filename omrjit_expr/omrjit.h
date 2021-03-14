/*
 * omrjit.h
 *
 *  Created on: Sep. 22, 2020
 *      Author: debajyoti
 */
#include "executor/execExpr.h"

#ifndef SRC_INCLUDE_JIT_OMRJIT_H_
#define SRC_INCLUDE_JIT_OMRJIT_H_

TupleTableSlot *slot_opt;
HeapTuple tuple_opt;
uint32 *off_opt;
TupleDesc desc_opt;
int natts_opt;
bool is_compiled;
bool b_is_compiled;
bool m_is_compiled;
bool h_is_compiled;
int attnum_opt;
bool is_virtual;
bool agg_flag;

bool omr_provider_successfully_loaded;
//extern bool provider_init(void);



/*typedef int32_t (*omr_eval_exec)(int32_t a, int32_t b);
omr_eval_exec omreval_exec;*/

typedef bool (*omr_eval_initialize)();
omr_eval_initialize omreval_init;

typedef void (*omr_eval_shutdown)();
omr_eval_shutdown omreval_shut;

extern bool provider_init(void);
char	omrjit_path[MAXPGPATH];

/*typedef int32_t (*omr_eval_compile)();
omr_eval_compile omreval_compile;*/

/*declare info about compilation*/
#define MAX_no_of_compilations 1000
int32_t compiled_code_iterator;

typedef Datum (OMRJIT_slot_deformFunctionType)(/*TupleTableSlot **/ExprContext *, int32_t, int32_t, ExprEvalStep *,ExprState *, bool *);
typedef OMRJIT_slot_deformFunctionType *(omr_eval_compile)(ExprState *, TupleTableSlot *, TupleTableSlot *, TupleTableSlot *,
															TupleTableSlot *);
omr_eval_compile *omreval_compile;
OMRJIT_slot_deformFunctionType *slot_deform;
OMRJIT_slot_deformFunctionType *slot_agg;
int32_t irc;

typedef struct omr_compiledcode_info
{
	OMRJIT_slot_deformFunctionType *compiledSequence;
	bool is_sequence_compiled;
}omr_compiledcode_info;

struct omr_compiledcode_info o_cc_info[MAX_no_of_compilations];
/* END */

ExprEvalStep *op_omr;
bool is_compiled_expr;
bool is_compiled_expr_outerslot;

//Deform
typedef void (OMRJIT_slot_deformFunctionType_att)(int32_t, TupleTableSlot *, HeapTuple , uint32 *, char *);
typedef OMRJIT_slot_deformFunctionType_att *(omr_eval_compile_att)(int32_t , TupleDesc , TupleTableSlot *);
omr_eval_compile_att *omreval_compile_att;
OMRJIT_slot_deformFunctionType_att *slot_deform_att;
//

//can be deleted
typedef void (OMRJIT_slot_deformFunctionType_hard_jit)(int32_t, TupleTableSlot *, HeapTuple , uint32 *, char *);
typedef OMRJIT_slot_deformFunctionType_hard_jit *(omr_eval_compile_hard_jit)();
omr_eval_compile_hard_jit *omreval_compile_hard_jit;
OMRJIT_slot_deformFunctionType_hard_jit *slot_deform_hard_jit;
//


int32_t guaranteed_column_number;
int32_t alignto_opt;

//jit eval expression
/*typedef void (*omr_expr_qual_compile)();
omr_expr_qual_compile expr_qual_compile;*/
typedef int32_t (eval_expr_qual_FunctionType)(bool **, Datum **);
typedef eval_expr_qual_FunctionType *(omr_expr_qual_compile)();
omr_expr_qual_compile *expr_qual_compile;
eval_expr_qual_FunctionType *qual_FunctionType;

typedef int32_t (EEOP_FUNCEXPR_STRICT_FunctionType)(bool **, Datum **, Datum, FunctionCallInfo);
typedef EEOP_FUNCEXPR_STRICT_FunctionType *(omr_expr_FUNCEXPR_STRICT_compile)(int32_t, NullableDatum *);
omr_expr_FUNCEXPR_STRICT_compile *expr_funcexprstrict_compile;
EEOP_FUNCEXPR_STRICT_FunctionType *FUNCEXPR_STRICT;

typedef int32_t (EEOP_NOT_DISTINCT_FunctionType)(bool **, Datum **, Datum, FunctionCallInfo);
typedef EEOP_NOT_DISTINCT_FunctionType *(omr_EEOP_NOT_DISTINCT_compile)();
omr_EEOP_NOT_DISTINCT_compile *EEOP_NOT_DISTINCT_compile;
EEOP_NOT_DISTINCT_FunctionType *EEOP_NOT_DISTINCT_FUNC;



typedef void (EEOP_VAR_FunctionType)(int32_t, TupleTableSlot * , bool **, Datum **);
typedef EEOP_VAR_FunctionType *(omr_EEOP_var_compile)();
omr_EEOP_var_compile *expr_var_compile;
EEOP_VAR_FunctionType *VAR_FunctionType;


typedef void (EEOP_ASSIGN_VAR_FunctionType)(/*Datum *, bool *, Datum, bool*/int32_t, TupleTableSlot *, Datum, bool);
typedef EEOP_ASSIGN_VAR_FunctionType *(omr_EEOP_ASSIGN_var_compile)();
omr_EEOP_ASSIGN_var_compile *expr_ASSIGN_var_compile;
EEOP_ASSIGN_VAR_FunctionType *ASSIGN_VAR_FunctionType;


typedef int32_t (EEOP_AGGREF_FunctionType)(bool **, Datum **, Datum, bool);
typedef EEOP_AGGREF_FunctionType *(EEOP_AGGREF_compile)();
EEOP_AGGREF_compile *expr_EEOP_AGGREF_compile;
EEOP_AGGREF_FunctionType *EEOP_AGGREF_Func;

typedef int32_t (EEOP_ASSIGN_TMP_MAKE_RO_FunctionType)(int32_t, TupleTableSlot *, ExprState *);
typedef EEOP_ASSIGN_TMP_MAKE_RO_FunctionType *(EEOP_ASSIGN_TMP_MAKE_RO_compile)();
EEOP_ASSIGN_TMP_MAKE_RO_compile *expr_EEOP_ASSIGN_TMP_MAKE_RO_compile;
EEOP_ASSIGN_TMP_MAKE_RO_FunctionType *EEOP_ASSIGN_TMP_MAKE_RO_Func;


//


/*Expression Compilation*/
bool omr_jit_compile_expr;
bool is_omr_expr_compiled;

//compile time eval
bool omr_byval[30];
int16 omr_attlen[30];
int indexer;

//bytecode interpreter
typedef Datum (omr_expression_bytecode_computation_FunctionType)(ExprState *, ExprContext *, bool *);
typedef omr_expression_bytecode_computation_FunctionType *(omr_vm_compile)(ExprState */*, ExprContext *, bool **/);
omr_vm_compile *omr_bytecode_Interpreter;
omr_expression_bytecode_computation_FunctionType *omr_vm;
bool is_BI_compiler;
//

//new
bool is_compile_FUNCEXPR_STRICT;
//


#endif /* SRC_INCLUDE_JIT_OMRJIT_H_ */
