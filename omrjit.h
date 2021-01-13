/*
 * omrjit.h
 *
 *  Created on: Sep. 22, 2020
 *      Author: debajyoti
 */

#ifndef SRC_INCLUDE_JIT_OMRJIT_H_
#define SRC_INCLUDE_JIT_OMRJIT_H_

TupleTableSlot *slot_opt;
HeapTuple tuple_opt;
uint32 *off_opt;
TupleDesc desc_opt;
int natts_opt;
bool is_compiled;
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

typedef void (OMRJIT_slot_deformFunctionType)(int32_t, TupleTableSlot *, HeapTuple , uint32 *);
typedef OMRJIT_slot_deformFunctionType *(omr_eval_compile)();
omr_eval_compile *omreval_compile;
OMRJIT_slot_deformFunctionType *slot_deform;


int32_t guaranteed_column_number;
int32_t alignto_opt;

//jit eval expression
/*typedef void (*omr_expr_qual_compile)();
omr_expr_qual_compile expr_qual_compile;*/
typedef int32_t (eval_expr_qual_FunctionType)(bool **, Datum **);
typedef eval_expr_qual_FunctionType *(omr_expr_qual_compile)();
omr_expr_qual_compile *expr_qual_compile;
eval_expr_qual_FunctionType *qual_FunctionType;

typedef int32_t (EEOP_FUNCEXPR_STRICT_FunctionType)(int32_t, NullableDatum *, bool **, Datum **, Datum, FunctionCallInfo);
typedef EEOP_FUNCEXPR_STRICT_FunctionType *(omr_expr_FUNCEXPR_STRICT_compile)();
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







#endif /* SRC_INCLUDE_JIT_OMRJIT_H_ */
