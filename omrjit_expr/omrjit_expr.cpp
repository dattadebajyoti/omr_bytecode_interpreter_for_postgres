/*
 * omrjit_expr.cpp
 *
 *  Created on: Aug. 8, 2020
 *      Author: debajyoti
 */

/*
 * JIT compile expression.
 */

extern "C" {

#include "postgres.h"

#include "miscadmin.h"
#include "nodes/makefuncs.h"
#include "nodes/nodeFuncs.h"
#include "parser/parse_coerce.h"
#include "parser/parsetree.h"
#include "pgstat.h"
#include "utils/acl.h"
#include "utils/builtins.h"
#include "utils/date.h"
#include "utils/fmgrtab.h"
#include "utils/lsyscache.h"
#include "utils/memutils.h"
#include "utils/timestamp.h"
#include "utils/typcache.h"
#include "utils/xml.h"

#include "access/htup_details.h"
#include "access/nbtree.h"
#include "access/tupconvert.h"
#include "catalog/objectaccess.h"
#include "catalog/pg_type.h"
#include "executor/execdebug.h"
#include "executor/nodeAgg.h"
#include "executor/nodeSubplan.h"
#include "executor/execExpr.h"
#include "funcapi.h"

#include "executor/tuptable.h"
#include "nodes/nodes.h"
#include "access/attnum.h"
#include "c.h"
#include "access/tupmacs.h"

#include "postgres.h"
#include "executor/execExpr.h"
#include "jit/jit.h"
#include "executor/tuptable.h"
#include "nodes/nodes.h"
#include "jit/omrjit.h"
#include "jit/omr_operator.h"
#include "utils/expandeddatum.h"

}//extern block ended

#include "/home/debajyoti/vm-project/src/lib/omr_debug/omr/jitbuilder/release/cpp/include/JitBuilder.hpp"
#include <iostream>
#include <vector>
using std::cout;
using std::cerr;


extern "C" {


/*Initialize omr*/
bool omr_init(){
    bool initialized = initializeJit();
    return initialized;
}

/*Shutdown omr*/
void omr_shut(){
    shutdownJit();
}


static Datum
MakeExpandedObjectReadOnlyInternal_func(Datum d)
{
#define MakeExpandedObjectReadOnlyInternal_func_LINE LINETOSTR(__LINE__)

	ExpandedObjectHeader *eohptr;

	/* Nothing to do if not a read-write expanded-object pointer */
	if (!VARATT_IS_EXTERNAL_EXPANDED_RW(DatumGetPointer(d)))
		return d;

	/* Now safe to extract the object pointer */
	eohptr = DatumGetEOHP(d);

	/* Return the built-in read-only pointer instead of given pointer */
	return EOHPGetRODatum(eohptr);
}

//VARSIZE_ANY_func
static size_t VARSIZE_ANY_func(void* attptr) {
    #define VARSIZE_ANY_func_LINE LINETOSTR(__LINE__)

	return ((int32_t)VARSIZE_ANY(attptr));
}

//t_uint32_func
static int32_t t_uint32_func(char *T) {
    #define t_uint32_func_LINE LINETOSTR(__LINE__)

	return *((uint8 *) (T));
}

//t_datum_func
static Datum t_datum_func(char *T) {
    #define t_datum_func_LINE LINETOSTR(__LINE__)

	return *((Datum *)(T));
}

//t_int32_func
static Datum t_int32_func(char *T) {
    #define t_int32_func_LINE LINETOSTR(__LINE__)

	return Int32GetDatum(*((int32 *)(T)));
}

//t_int16_func
static Datum t_int16_func(char *T) {
    #define t_int16_func_LINE LINETOSTR(__LINE__)

	return Int16GetDatum(*((int16 *)(T)));
}

//t_str_func
static Datum t_str_func(char *T) {
    #define t_str_func_LINE LINETOSTR(__LINE__)

	return CharGetDatum(*((char *)(T)));
}


//strlen_func
static int32_t strlen_func(char* attptr) {
    #define strlen_func_LINE LINETOSTR(__LINE__)

	return (strlen((char *) (attptr)));
}

static void store_isnull(TupleTableSlot *slot, int16_t index) {
    #define ISNULL_LINE LINETOSTR(__LINE__)
	bool	   *isnull = slot->tts_isnull;
	isnull[index] = true;
}
static void print_func(int32_t m) {
    #define PRINTFUNC_LINE LINETOSTR(__LINE__)

	elog(INFO, "natts in print: %d\n", m);

}


static char* tp_func(HeapTupleHeader tup) {
    #define TPFUNC_LINE LINETOSTR(__LINE__)

	return ((char *) tup + tup->t_hoff);

}

/* return att_align_nominal */
static int32_t att_addlength_pointer_func(int32_t off, int16_t attlen, char * attptr) {
    #define ATTADDLENFUNC_LINE LINETOSTR(__LINE__)
	return att_addlength_pointer(off, attlen, attptr);

}

/* return fetchatt */
static Datum fetchatt_func(Form_pg_attribute A, char * T, int32_t off) {
    #define FETCHATTFUNC_LINE LINETOSTR(__LINE__)

	return fetchatt(A, T);

}

/* return att_align_nominal */
static int32_t att_align_nominal_func(int32_t cur_offset, char attalign) {
    #define ATTALIGNNOMINALFUNC_LINE LINETOSTR(__LINE__)
	return att_align_nominal(cur_offset, attalign);

}


/* return att_align_pointer */
static int32_t att_align_pointer_func(int32_t cur_offset, char attalign, int32_t attlen, char * attptr) {
    #define ATTALIGNPTRFUNC_LINE LINETOSTR(__LINE__)

	return att_align_pointer(cur_offset, attalign, attlen, attptr + cur_offset);

}


}//extern C block ended



/****************Define class******************/

/*typedef void (OMRJIT_slot_deformFunctionType)(int32_t, TupleTableSlot *, HeapTuple , uint32 *);
OMRJIT_slot_deformFunctionType *slot_deform;*/
class slot_compile_deform : public OMR::JitBuilder::MethodBuilder
   {
   private:

   OMR::JitBuilder::IlType *pStr;
   OMR::JitBuilder::IlType *pChar;
   OMR::JitBuilder::IlType *pDatum;
   OMR::JitBuilder::IlType *pDat;
   OMR::JitBuilder::IlType *pBits;
   OMR::JitBuilder::IlType *pInt32;
   OMR::JitBuilder::IlType *pInt16;
   OMR::JitBuilder::IlType *pBits8;
   OMR::JitBuilder::IlType *bool_type;
   OMR::JitBuilder::IlType *pbool;
   OMR::JitBuilder::IlType *size_t_type;
   OMR::JitBuilder::IlType *puint32;
   OMR::JitBuilder::IlType *datum_rep;
   OMR::JitBuilder::IlType *int32_rep;
   OMR::JitBuilder::IlType *int16_rep;
   OMR::JitBuilder::IlType *str_rep;
   OMR::JitBuilder::IlType *puint8;

   OMR::JitBuilder::IlType *StructTypeContext;
   OMR::JitBuilder::IlType *pStructTypeContext;

   OMR::JitBuilder::IlType *StructTypeTupleDesc;
   OMR::JitBuilder::IlType *pStructTypeTupleDesc;

   OMR::JitBuilder::IlType *StructTypeSlotOps;
   OMR::JitBuilder::IlType *pStructTypeSlotOps;

   OMR::JitBuilder::IlType *FormData_pg_attribute;
   OMR::JitBuilder::IlType *pFormData_pg_attribute;

   //OMR::JitBuilder::IlType *TupleTableSlot;
   //OMR::JitBuilder::IlType *pTupleTableSlot;

   OMR::JitBuilder::IlType *HeapTupleHeaderData;
   OMR::JitBuilder::IlType *pHeapTupleHeaderData;

   OMR::JitBuilder::IlType *HeapTuple;
   OMR::JitBuilder::IlType *pHeapTuple;

   OMR::JitBuilder::IlType *HeapTupleData;
   OMR::JitBuilder::IlType *pHeapTupleData;

   OMR::JitBuilder::IlType *TupleTableSlotType;
   OMR::JitBuilder::IlType *TupleTableSlotDesc;
   OMR::JitBuilder::IlType *pTupleTableSlot;
   OMR::JitBuilder::IlType *psize_t;
   OMR::JitBuilder::IlType *pAddress;
   OMR::JitBuilder::IlType *pInt8;



   public:

   	  TupleDesc	tupleDesc;
      slot_compile_deform(OMR::JitBuilder::TypeDictionary *, int32_t, TupleDesc, TupleTableSlot *);

      int32_t natts;
      TupleTableSlot *slot;

      //OMR::JitBuilder::IlValue* fetch_attributes(IlBuilder *, OMR::JitBuilder::IlValue *, OMR::JitBuilder::IlValue *, OMR::JitBuilder::IlValue *);
      void fetch_attributes(IlBuilder *, char *att, OMR::JitBuilder::IlValue *, Form_pg_attribute );
      void att_align_nominal_cal(IlBuilder *, OMR::JitBuilder::IlValue *, char *, char );
      void att_addlength_pointer_cal(IlBuilder *, OMR::JitBuilder::IlValue *, int16_t , OMR::JitBuilder::IlValue *, char *offset);
      void att_align_pointer_cal(IlBuilder *, OMR::JitBuilder::IlValue *, OMR::JitBuilder::IlValue *, char , char *, int16_t );

      virtual bool buildIL();

   };

/****************************************Define the method builder object********************************************************/

slot_compile_deform::slot_compile_deform(OMR::JitBuilder::TypeDictionary *types, int32_t natts, TupleDesc	tupleDesc, TupleTableSlot *slot)
   : OMR::JitBuilder::MethodBuilder(types)
   {
   DefineLine(LINETOSTR(__LINE__));
   DefineFile(__FILE__);

   DefineName("slot_compile_deform");

   pStr = types->toIlType<char *>();
   pChar = types->toIlType<char>();
   pDatum = types->toIlType<Datum *>();
   pDat = types->toIlType<Datum>();
   pBits = types->toIlType<char>();
   pInt32 = types->PointerTo(Int32);
   pInt16 = types->PointerTo(Int16);
   puint8 = types->toIlType<uint8 *>();
   pInt8 = types->toIlType<int8 *>();

   pBits8 = types->toIlType<bits8 *>();

   bool_type = types->toIlType<bool>();
   pbool = types->toIlType<bool*>();

   size_t_type = types->toIlType<size_t>();

   puint32 = types->toIlType<uint32>();
   psize_t = types->toIlType<size_t>();

   datum_rep = types->PointerTo(pDatum);
   int32_rep = types->PointerTo(pInt32);
   int16_rep = types->PointerTo(pInt16);
   str_rep   = types->PointerTo(pStr);
   pAddress = types->PointerTo(Address);

   /*this->thisatt = new Form_pg_attribute[total_att];
   for(int32_t i = 0; i < total_att; i++)
   {
	   thisatt[i] = att[i];
   }*/
   this->tupleDesc = tupleDesc;
   this->natts = natts;
   this->slot = slot;

   //StructTypeContext      = types->LookupStruct("OMRJitContext");
   //pStructTypeContext     = types->PointerTo(StructTypeContext);

   //StructTypeTupleDesc      = types->LookupStruct("TupleDesc");
   //pStructTypeTupleDesc     = types->PointerTo(StructTypeTupleDesc);

   //StructTypeSlotOps      = types->LookupStruct("TupleTableSlotOps");
   //pStructTypeSlotOps     = types->PointerTo(StructTypeSlotOps);

   FormData_pg_attribute      = types->LookupStruct("FormData_pg_attribute");
   pFormData_pg_attribute     = types->PointerTo((char *)"FormData_pg_attribute");

   //TupleTableSlotType      = types->LookupStruct("TupleTableSlot");
   //pTupleTableSlot     = types->PointerTo(TupleTableSlot);

   StructTypeTupleDesc = types->LookupStruct("TupleDescData");
   pStructTypeTupleDesc = types->PointerTo((char *)"TupleDescData");

   TupleTableSlotDesc = types->LookupStruct("TupleTableSlot");
   pTupleTableSlot = types->PointerTo((char *)"TupleTableSlot");

   HeapTuple      = types->LookupStruct("HeapTupleData");
   pHeapTuple     = types->PointerTo(HeapTuple);

   HeapTupleHeaderData      = types->LookupStruct("HeapTupleHeaderData");
   pHeapTupleHeaderData     = types->PointerTo((char *)"HeapTupleHeaderData");

   /* define Parameters */
   //DefineParameter("context",  pStructTypeContext);
   //DefineParameter("desc",     pStructTypeTupleDesc);

   DefineParameter("natts",    Int32);

   DefineParameter("slot",     pTupleTableSlot);
   DefineParameter("tuple",    HeapTuple);

   DefineParameter("offp",     pInt32);
   DefineParameter("tp",     pStr);

   DefineFunction((char *)"t_uint32_func",
                  (char *)__FILE__,
                  (char *)t_uint32_func_LINE,
                  (void *)&t_uint32_func,
                  Int32,
                  1,
				  pStr);

   DefineFunction((char *)"t_datum_func",
                  (char *)__FILE__,
                  (char *)t_datum_func_LINE,
                  (void *)&t_datum_func,
                  Address,
                  1,
				  pStr);

   DefineFunction((char *)"t_int32_func",
                  (char *)__FILE__,
                  (char *)t_int32_func_LINE,
                  (void *)&t_int32_func,
                  Address,
                  1,
				  pStr);

   DefineFunction((char *)"t_int16_func",
                  (char *)__FILE__,
                  (char *)t_int16_func_LINE,
                  (void *)&t_int16_func,
                  Address,
                  1,
				  pStr);

   DefineFunction((char *)"t_str_func",
                  (char *)__FILE__,
                  (char *)t_str_func_LINE,
                  (void *)&t_str_func,
                  Address,
                  1,
				  pStr);

   //att_align_pointer_func
   DefineFunction((char *)"att_align_pointer_func",
                  (char *)__FILE__,
                  (char *)ATTALIGNPTRFUNC_LINE,
                  (void *)&att_align_pointer_func,
                  Int32,
                  4,
                  Int32, pChar, Int32, pStr/*HeapTupleHeaderData*/);

   //fetchatt_func
   DefineFunction((char *)"fetchatt_func",
                  (char *)__FILE__,
                  (char *)FETCHATTFUNC_LINE,
                  (void *)&fetchatt_func,
                  pDat,
                  3,
				  Address, pStr/*HeapTupleHeaderData*/, Int32);

   //att_align_nominal_func
   DefineFunction((char *)"att_align_nominal_func",
                  (char *)__FILE__,
                  (char *)ATTALIGNNOMINALFUNC_LINE,
                  (void *)&att_align_nominal_func,
                  Int32,
                  2,
				  Int32, pChar);

   DefineFunction((char *)"tp_func",
                  (char *)__FILE__,
                  (char *)TPFUNC_LINE,
                  (void *)&tp_func,
                  pStr,
                  1,
				  HeapTupleHeaderData);

   //VARSIZE_ANY_func
   DefineFunction((char *)"VARSIZE_ANY_func",
                  (char *)__FILE__,
                  (char *)VARSIZE_ANY_func_LINE,
                  (void *)&VARSIZE_ANY_func,
				  psize_t,
                  1,
				  pStr);

   //strlen_func
   DefineFunction((char *)"strlen_func",
                  (char *)__FILE__,
                  (char *)strlen_func_LINE,
                  (void *)&strlen_func,
                  Int32,
                  1,
				  pStr);

   /* Define Return type */
   DefineReturnType(NoType);

   //att_align_nominal_func
   DefineFunction((char *)"att_addlength_pointer_func",
                  (char *)__FILE__,
                  (char *)ATTADDLENFUNC_LINE,
                  (void *)&att_addlength_pointer_func,
                  Int32,
                  3,
				  Int32, Int16, pStr/*HeapTupleHeaderData*/);

   DefineFunction((char *)"print_func",
                  (char *)__FILE__,
                  (char *)PRINTFUNC_LINE,
                  (void *)&print_func,
				  NoType,
                  2,
				  Int32,
				  Int32);

   DefineFunction((char *)"store_isnull",
                  (char *)__FILE__,
                  (char *)ISNULL_LINE,
                  (void *)&store_isnull,
				  NoType,
                  2,
				  Int32, Int16);

   /* Define Return type */
   DefineReturnType(NoType);

   }

class StructTypeDictionary : public OMR::JitBuilder::TypeDictionary
   {
   public:

   StructTypeDictionary() :
      OMR::JitBuilder::TypeDictionary()
      {
           auto d = TypeDictionary{};

           OMR::JitBuilder::IlType *t_field3Type = DefineUnion("t_field3");
      	   UnionField("t_field3", "t_cid", Int32);
     	   UnionField("t_field3", "t_xvac", Int32);
      	   CloseUnion("t_field3");

           OMR::JitBuilder::IlType *HeapTupleFieldsType = DefineStruct("HeapTupleFields");
           DefineField("HeapTupleFields", "t_xmin", Int32);
           DefineField("HeapTupleFields", "t_xmax", Int32);
           DefineField("HeapTupleFields", "t_field3", t_field3Type);

           CloseStruct("HeapTupleFields");

      	   OMR::JitBuilder::IlType *DatumTupleFieldsType = DefineStruct("DatumTupleFields");
      	   DefineField("DatumTupleFields", "datum_len_", Int32);
      	   DefineField("DatumTupleFields", "datum_typmod", Int32);
      	   DefineField("DatumTupleFields", "datum_typeid", Int32);//Oid datum_typeid;

      	   CloseStruct("DatumTupleFields");

           OMR::JitBuilder::IlType *t_choiceType = DefineUnion("t_choice");
      	   UnionField("t_choice", "t_heap", HeapTupleFieldsType);
     	   UnionField("t_choice", "t_datum", DatumTupleFieldsType);
      	   CloseUnion("t_choice");

      	   ///////////BlockIdData and ItemPointerData
      	   OMR::JitBuilder::IlType *BlockIdDataType = DefineStruct("BlockIdData");
      	   DefineField("BlockIdData", "bi_hi", Int16);
      	   DefineField("BlockIdData", "bi_lo", Int16);

      	   CloseStruct("BlockIdData");

      	   OMR::JitBuilder::IlType *ItemPointerDataType = DefineStruct("ItemPointerData");
      	   DefineField("ItemPointerData", "ip_blkid", BlockIdDataType);
      	   DefineField("ItemPointerData", "ip_posid", Int16);

      	   CloseStruct("ItemPointerData");

	   	   /****Declare HeapTupleHeaderData*********/
	   	   OMR::JitBuilder::IlType *HeapTupleHeaderDataType = DefineStruct("HeapTupleHeaderData");

	   	   DefineField("HeapTupleHeaderData", "t_choice", t_choiceType);
	   	   DefineField("HeapTupleHeaderData", "t_ctid", ItemPointerDataType);
	   	   DefineField("HeapTupleHeaderData", "t_infomask2", Int16);
	   	   DefineField("HeapTupleHeaderData", "t_infomask", Int16);
	   	   DefineField("HeapTupleHeaderData", "t_hoff", d.toIlType<uint8>());
	   	   DefineField("HeapTupleHeaderData", "t_bits", d.toIlType<char *>());

	   	   CloseStruct("HeapTupleHeaderData");

	   	   /****Declare HeapTupleData*********/
	   	   DefineStruct("HeapTupleData");

	   	   DefineField("HeapTupleData", "t_len", Int32);
	   	   DefineField("HeapTupleData", "t_self", ItemPointerDataType);
	   	   DefineField("HeapTupleData", "t_tableOid", Int16);//typedef unsigned int Oid;
	   	   DefineField("HeapTupleData", "t_data", HeapTupleHeaderDataType);

	   	   CloseStruct("HeapTupleData");


	   	   /*********Declare FormData_pg_attribute***********/


	   	   DEFINE_STRUCT(FormData_pg_attribute);
	   	   OMR::JitBuilder::IlType *pFormData_pg_attributeType = PointerTo("FormData_pg_attribute");

	   	   DEFINE_FIELD(FormData_pg_attribute, attlen, Int16);

	   	   DEFINE_FIELD(FormData_pg_attribute, attcacheoff, Int32);

	   	   DEFINE_FIELD(FormData_pg_attribute, attbyval, d.toIlType<bool>());//bool
	   	   DEFINE_FIELD(FormData_pg_attribute, attalign,   d.toIlType<char>());

	   	   CLOSE_STRUCT(FormData_pg_attribute);

	   	   /*********Declare AttrDefault***********/
	   	   /*OMR::JitBuilder::IlType *AttrDefaultType =*/ DefineStruct("AttrDefault");
	   	   //OMR::JitBuilder::IlType *pAttrDefaultType = PointerTo("AttrDefault");

	   	   DefineField("AttrDefault", "adnum", Int16);
	   	   DefineField("AttrDefault", "adbin", d.toIlType<char *>());

	   	   CLOSE_STRUCT(AttrDefault);

	   	   /*********Declare ConstrCheck***********/
	   	   /*OMR::JitBuilder::IlType *ConstrCheckType =*/ DefineStruct("ConstrCheck");
	   	   //OMR::JitBuilder::IlType *pConstrCheckType = PointerTo("ConstrCheck");

	   	   DefineField("ConstrCheck", "ccname", d.toIlType<char *>());
	   	   DefineField("ConstrCheck", "ccbin", d.toIlType<char *>());
	   	   DefineField("ConstrCheck", "ccvalid", Int32);
	   	   DefineField("ConstrCheck", "ccnoinherit", Int32);

	   	   CloseStruct("ConstrCheck");

	   	   /*********Declare AttrMissing***********/
	   	   /*OMR::JitBuilder::IlType *AttrMissingType =*/ DefineStruct("AttrMissing");
	   	   //OMR::JitBuilder::IlType *pAttrMissingType = PointerTo("AttrMissing");

	   	   DefineField("AttrMissing", "am_present", Int32);
	   	   DefineField("AttrMissing", "am_value", d.toIlType<Datum>());

	   	   CloseStruct("AttrMissing");

	   	   /*********Declare TupleConstr***********/
	   	   DEFINE_STRUCT(TupleDescData);
	       OMR::JitBuilder::IlType *pTupleDescType = PointerTo("TupleDescData");

	   	   DEFINE_FIELD(TupleDescData, natts, Int32);
	   	   DEFINE_FIELD(TupleDescData, tdtypeid, Int32);
	   	   DEFINE_FIELD(TupleDescData, tdtypmod, Int32);
	   	   DEFINE_FIELD(TupleDescData, tdrefcount, Int32);

	   	   DEFINE_FIELD(TupleDescData, attrs, pFormData_pg_attributeType/*pInt16*/);

	   	   CLOSE_STRUCT(TupleDescData);

	   	   /****Declare TupleTableSlot*********/
	   	   OMR::JitBuilder::IlType *TupleTableSlotType = DEFINE_STRUCT(TupleTableSlot);
	       OMR::JitBuilder::IlType *pTupleTableSlotType = PointerTo("TupleTableSlot");

	   	   DEFINE_FIELD(TupleTableSlot, tts_flags, Int16);
	   	   DEFINE_FIELD(TupleTableSlot, tts_nvalid, Int16);
	   	   DEFINE_FIELD(TupleTableSlot, tts_tupleDescriptor, /*pInt16*/pTupleDescType);
	   	   DEFINE_FIELD(TupleTableSlot, tts_values, toIlType<Datum *>());
	   	   DEFINE_FIELD(TupleTableSlot, tts_isnull, PointerTo(Int32));

	   	   CLOSE_STRUCT(TupleTableSlot);

	   	   /*Declare ExprEvalStep*/
	   	   OMR::JitBuilder::IlType *ExprEvalStepType = DEFINE_STRUCT(ExprEvalStep);
	       OMR::JitBuilder::IlType *pExprEvalStepType = PointerTo("ExprEvalStep");

	   	   DEFINE_FIELD(ExprEvalStep, resvalue, toIlType<Datum *>());
	   	   DEFINE_FIELD(ExprEvalStep, resnull, toIlType<bool *>());

	   	   CLOSE_STRUCT(ExprEvalStep);

	   	   /*Declare PlanState*/
	   	   OMR::JitBuilder::IlType *PlanStateType = DEFINE_STRUCT(PlanState);
	       OMR::JitBuilder::IlType *pPlanStateType = PointerTo("PlanState");

	   	   DEFINE_FIELD(PlanState, resultopsset, toIlType<bool>());

	   	   CLOSE_STRUCT(PlanState);

	   	   /*Declare  Expression*/
	   	   OMR::JitBuilder::IlType *ExprStateType = DEFINE_STRUCT(ExprState);

	   	   DEFINE_FIELD(ExprState, resnull, toIlType<bool>());
	   	   DEFINE_FIELD(ExprState, resvalue, toIlType<Datum>());
	   	   DEFINE_FIELD(ExprState, resultslot, pTupleTableSlotType);
	   	   DEFINE_FIELD(ExprState, steps, pExprEvalStepType);
	   	   DEFINE_FIELD(ExprState, steps_len, Int32);
	   	   DEFINE_FIELD(ExprState, parent, pPlanStateType);

	   	   CLOSE_STRUCT(ExprState);

	   	   /*Declare  ExprContext*/
	   	   OMR::JitBuilder::IlType *ExprContextType = DEFINE_STRUCT(ExprContext);

	   	   DEFINE_FIELD(ExprContext, ecxt_scantuple,  pTupleTableSlotType);
	   	   DEFINE_FIELD(ExprContext, ecxt_innertuple, pTupleTableSlotType);
	   	   DEFINE_FIELD(ExprContext, ecxt_outertuple, pTupleTableSlotType);

	   	   CLOSE_STRUCT(ExprContext);

      }
   };


void
slot_compile_deform::att_addlength_pointer_cal(IlBuilder *b, OMR::JitBuilder::IlValue *cur_offset, int16_t attlen, OMR::JitBuilder::IlValue *attptr, char *offset)
{
	//b->Store("offset", b->ConstInt32(0));

	if(attlen > 0)
	{
		b->Store(offset,
		b->   Add(
		b->      ConstInt32(attlen),
		b->      ConvertTo(Int32, cur_offset)));
	}
	else if(attlen == -1)
	{
		b->Store(offset,
		b->ConvertTo(Int32,
	    b->   Add(
		b->      ConvertTo(Int32, cur_offset),
		b->      ConvertTo(Int32,
	    b->         Call("VARSIZE_ANY_func", 1,
		b->            ConvertTo(pStr, attptr))))));
	}
	else
	{
		b->Store(offset,
		b->   ConvertTo(Int32,
		b->      Add(
		b->         ConvertTo(Int32, cur_offset),
		b->         Add(
		b->            ConstInt32(1),
		b->            ConvertTo(Int32,
		b->               Call("strlen_func",1,
	    b->                  ConvertTo(pStr, attptr))))))     );
	}

	//return b->ConvertTo(Int32, b->Load("offset"));

}

void
slot_compile_deform::att_align_nominal_cal(IlBuilder *b, OMR::JitBuilder::IlValue *cur_offset, char *cal_off, char attalign)
{

	if(attalign == /*TYPALIGN_INT*/0x69)
	{
		b->Store(cal_off,
		b->   ConvertTo(Int32,
	    b->   And(
	    b->      Add(
	    b->	     ConvertTo(Int32, cur_offset),
		b->         ConstInt32(3)),
	    b->      ConstInt32(~3)))    );
	}
	else if(attalign == /*TYPALIGN_CHAR*/ 0x63)
	{
		b->Store(cal_off,
	    b->ConvertTo(Int32, cur_offset));
	}
	else if(attalign == /*TYPALIGN_DOUBLE*/ 0x64)
	{
		b->Store(cal_off,
	    b->   ConvertTo(Int32,
	    b->   And(
		b->      Add(
	    b->         ConvertTo(Int32, cur_offset),
		b->         ConstInt32(7)),
		b->      ConstInt32(~7)))    );
	}
	else
	{
		b->Store(cal_off,
		b->   ConvertTo(Int32,
		b->   And(
		b->      Add(
	    b->         ConvertTo(Int32, cur_offset),
		b->         ConstInt32(1)),
		b->      ConstInt32(~1)))   );
	}

}

void
slot_compile_deform::att_align_pointer_cal(IlBuilder *b, OMR::JitBuilder::IlValue *cur_offset, OMR::JitBuilder::IlValue *attptr, char attalign, char *off_align_ptr,
		int16_t attlen)
{
	if(attlen == -1)
	{
		OMR::JitBuilder::IlBuilder *VARATT_NOT_PAD_BYTE_match = NULL;
		b->IfThen(&VARATT_NOT_PAD_BYTE_match,
	    b->   NotEqualTo(
	    b->      UnsignedConvertTo(Int8,
	    b->	     LoadAt(pInt8,
	    b->	        ConvertTo(pInt8, attptr))),
		b->      ConstInt8(0)));

		//Translation for: (uintptr_t) (cur_offset)
		VARATT_NOT_PAD_BYTE_match->Store(off_align_ptr,
		VARATT_NOT_PAD_BYTE_match->   ConvertTo(Int32, cur_offset));
	}
	else
	{
		att_align_nominal_cal(b, cur_offset, "cal_off2", attalign);

		b->Store(off_align_ptr,
		b->   ConvertTo(Int32,
	    b->	 	Load("cal_off2")   ));
	}

}



void
slot_compile_deform::fetch_attributes(IlBuilder *b, char *att, OMR::JitBuilder::IlValue *tp, Form_pg_attribute attribute)
{

    if(attribute->attbyval)
    {
        if(attribute->attlen == (int) sizeof(Datum))
        {
        	b->Store(att, b->LoadAt(pAddress, b->ConvertTo(pAddress, tp)));
        }
        else if(attribute->attlen == (int) sizeof(int32))
        {
        	b->Store(att, b->LoadAt(pAddress, b->ConvertTo(pInt32, b->ConvertTo(pStr, tp))));
        }
        else if (attribute->attlen == (int) sizeof(int16))
        {
        	b->Store(att, b->LoadAt(pAddress, b->ConvertTo(pInt16, b->ConvertTo(pStr, tp))));
        }
        else
        {
        	b->Store(att, b->LoadAt(pAddress, b->ConvertTo(pStr, tp)));
        }
    }
    else
    {
    	b->Store(att, b->ConvertTo(Address, b->ConvertTo(pStr, tp)));
    }

}


/******************************BUILDIL Tuple deformation**********************************************/

bool
slot_compile_deform::buildIL()
   {

   /*************************Convert the interpreted model**************************************/

	//TupleDesc	tupleDesc = slot_opt->tts_tupleDescriptor;
	Store("tupleDesc",
	   LoadIndirect("TupleTableSlot","tts_tupleDescriptor",
	      Load("slot")));

	//HeapTupleHeader tup = tuple_opt->t_data;
    Store("tup",
	   LoadIndirect("HeapTupleData", "t_data",
		  Load("tuple")));

    OMR::JitBuilder::IlValue *hoff = ConvertTo(Word, LoadIndirect("HeapTupleHeaderData", "t_hoff", Load("tup")));
    Store("tp1", ConvertTo(pInt8, Add(ConvertTo(Address, Load("tup")), hoff)));

    /*Store("tp1",
	   Call("tp_func", 1, Load("tup")));*/

	//uint32		off;			/* offset in tuple data */
	Store("off", ConstInt32(0));

	//bool		slow;			/* can we use/set attcacheoff? */
	Store("slow", ConvertTo(bool_type, ConstInt16(false)));

	/* We can only fetch as many attributes as the tuple has. */

	//bool hasnulls = HeapTupleHasNulls(tuple);  (((tuple)->t_data->t_infomask & 0x0001) != 0)
    Store("hasnulls", ConstInt32(0));

	OMR::JitBuilder::IlBuilder *nulls_check = NULL;
	IfThen(&nulls_check,
	   NotEqualTo(
	      And(
	         ConstInt16(1),
	         ConvertTo(Int16,
	            LoadIndirect("HeapTupleHeaderData", "t_infomask2",
	        	   LoadIndirect("HeapTupleData", "t_data",
	        	      Load("tuple")) )  )),
		  ConstInt16(0)));
	{
		nulls_check->Store("hasnulls",
	    nulls_check->   ConstInt32(1));
	}


	/*
	 * Check whether the first call for this tuple, and initialize or restore
	 * loop state.
	 */

	int32_t attnum = (int32_t)slot->tts_nvalid;
	if(attnum != 0)
	{
		Store("off",
	       ConvertTo(Int32,
	          LoadAt(pInt32,
	             Load("offp"))));

		//slow = TTS_SLOW(slot_opt);
		Store("slow",
		   ConvertTo(bool_type,
		      NotEqualTo(
		         And(
		            LoadIndirect("TupleTableSlot", "tts_flags",
		               Load("slot")),
		            ConstInt16(8)),
				   ConstInt16(0))));
	}



	for(; attnum < natts; attnum++)

	{
		Form_pg_attribute thisatt = TupleDescAttr(tupleDesc, attnum);

		Store("thisatt",
		   IndexAt(pFormData_pg_attribute,
		      StructFieldInstanceAddress("TupleDescData", "attrs",
		         Load("tupleDesc")),
		       ConvertTo(Int32,
		          ConstInt32(attnum)))            );

		//if hasnull
		IlBuilder *hasnull_builder = OrphanBuilder();
		OMR::JitBuilder::IlValue *hasnull_L = hasnull_builder->EqualTo(
											  hasnull_builder->   Load("hasnulls"),
											  hasnull_builder->   ConstInt32(1));

        //if att_isnull :- att_isnull(ATT, BITS) (!((BITS)[(ATT) >> 3] & (1 << ((ATT) & 0x07))))
		IlBuilder *att_isnull_builder = OrphanBuilder();
		OMR::JitBuilder::IlValue *att_isnull_R = att_isnull_builder->NotEqualTo(
												 att_isnull_builder->      And(
												 att_isnull_builder->         ConvertTo(Int32,
												 att_isnull_builder->            LoadAt(pBits8,
												 att_isnull_builder->               IndexAt(pBits8,
												 att_isnull_builder->                  LoadIndirect("HeapTupleHeaderData", "t_bits",
												 att_isnull_builder->                     Load("tup")),
												 att_isnull_builder->                  ShiftR(
												 att_isnull_builder->                     ConvertTo(Int32,
												 att_isnull_builder->                        ConstInt32(attnum)),
												 att_isnull_builder->                     ConstInt32(3))))),
												 att_isnull_builder->         ShiftL(
												 att_isnull_builder->            ConstInt32(1),
												 att_isnull_builder->            And(
												 att_isnull_builder->               ConvertTo(Int32,
												 att_isnull_builder->				   ConstInt32(attnum)),
												 att_isnull_builder->               ConstInt32(7)))),
												 att_isnull_builder->      ConstInt32(0));


		IlBuilder *null_true = NULL, *null_false = NULL;
		IfAnd(&null_true, &null_false, 2,
		   MakeCondition(hasnull_builder, hasnull_L),
		   MakeCondition(att_isnull_builder, att_isnull_R));

		//values[attnum] = (Datum) 0;
		null_true->StoreAt(
		null_true->   IndexAt(pDatum,
		null_true->      LoadIndirect("TupleTableSlot", "tts_values",
		null_true->         Load("slot")),
		null_true->	  ConvertTo(Int32,
		null_true->	     ConstInt32(attnum))),
		null_true->   /*ConvertTo(Address,*/ConstAddress((void*)0)
	    /*null_true->	     ConstInt32(0))*/);

		//isnull[attnum] = true;
		null_true->StoreAt(
		null_true->   IndexAt(pbool,
		null_true->      LoadIndirect("TupleTableSlot", "tts_isnull",
		null_true->         Load("slot")),
		null_true->      ConvertTo(Int32,
		null_true->         ConstInt32(attnum))),
		null_true->   ConvertTo(bool_type,
		null_true->      ConstInt16(true)));

		//slow = true;
		null_true->Store("slow",
		null_true->   ConvertTo(bool_type,
		null_true->      ConstInt16(true)));


        //continue

		//isnull[attnum] = false;
		null_false->StoreAt(
		null_false->   IndexAt(pbool,
		null_false->      LoadIndirect("TupleTableSlot", "tts_isnull",
		null_false->         Load("slot")),
		null_false->		 ConvertTo(Int32,
		null_false->          ConstInt32(attnum))),
		null_false->   ConvertTo(bool_type,
		null_false->      ConstInt16(false)));

		IlBuilder *x_ge_L_builder = null_false->OrphanBuilder();
		OMR::JitBuilder::IlValue *x_ge_L = x_ge_L_builder->NotEqualTo(
		                                      x_ge_L_builder->   ConvertTo(bool_type,
		                                      x_ge_L_builder->      Load("slow")),
		                                      x_ge_L_builder->   ConvertTo(bool_type,
		                                      x_ge_L_builder->      ConstInt16(true)));

		IlBuilder *x_lt_U_builder = null_false->OrphanBuilder();
		OMR::JitBuilder::IlValue *x_lt_U = x_lt_U_builder->GreaterOrEqualTo(
		                                      x_lt_U_builder->   ConvertTo(Int32,
		                                      x_lt_U_builder->      LoadIndirect("FormData_pg_attribute", "attcacheoff",
		                                      x_lt_U_builder->         Load("thisatt"))),
		                                      x_lt_U_builder->   ConstInt32(0));


		IlBuilder *rc1True = NULL, *rc1false = NULL;
		null_false->IfAnd(&rc1True, &rc1false, 2,
		null_false->   MakeCondition(x_ge_L_builder, x_ge_L),
		null_false->   MakeCondition(x_lt_U_builder, x_lt_U));

	    //in-if ... off = thisatt->attcacheoff;
		rc1True->Store("off",
		rc1True->   ConvertTo(Int32,
		rc1True->      LoadIndirect("FormData_pg_attribute", "attcacheoff",
		rc1True->         Load("thisatt"))));

		//
		/*if(thisatt->attcacheoff >= 0)
		{
			OMR::JitBuilder::IlBuilder *thisatt_attlen_negative_match = NULL;
		}*/
		//


		OMR::JitBuilder::IlBuilder *thisatt_attlen_negative_match = NULL;
		OMR::JitBuilder::IlBuilder *thisatt_attlen_negative_match_else = NULL;

		//in-else if ... if(thisatt->attlen == -1)
		rc1false->IfThenElse(&thisatt_attlen_negative_match, &thisatt_attlen_negative_match_else,
		rc1false->   EqualTo(
		rc1false->      ConvertTo(Int16,
		rc1false->         LoadIndirect("FormData_pg_attribute", "attlen",//suspicious
		rc1false->            Load("thisatt"))),
		rc1false->      ConstInt16(-1)));

		//in-else if ...  if(!slow)
		IlBuilder *x_ge_L_builder1 = thisatt_attlen_negative_match->OrphanBuilder();
		OMR::JitBuilder::IlValue *x_ge_L1 = x_ge_L_builder1->NotEqualTo(
		                                      x_ge_L_builder1->   ConvertTo(bool_type,
		                                      x_ge_L_builder1->      Load("slow")),
		                                      x_ge_L_builder1->   ConvertTo(bool_type,
		                                      x_ge_L_builder1->      ConstInt16(true)));

		//in-else if ... off == att_align_nominal(off, thisatt->attalign)
		IlBuilder *x_lt_U_builder1 = thisatt_attlen_negative_match->OrphanBuilder();
		att_align_nominal_cal(x_lt_U_builder1, x_lt_U_builder1->ConvertTo(Int32, x_lt_U_builder1->Load("off")),
											  "cal_off", thisatt->attalign);

		OMR::JitBuilder::IlValue *x_lt_U1 = x_lt_U_builder1->EqualTo(
		                                      x_lt_U_builder1->   Load("off"),
		                                      x_lt_U_builder1->   Load("cal_off")      );

		IlBuilder *rc1True1 = NULL, *slow_off_att_align_nominal_match_else = NULL;
		thisatt_attlen_negative_match->IfAnd(&rc1True1, &slow_off_att_align_nominal_match_else, 2,
		thisatt_attlen_negative_match->   MakeCondition(x_ge_L_builder1, x_ge_L1),
		thisatt_attlen_negative_match->   MakeCondition(x_lt_U_builder1, x_lt_U1));

		rc1True1->StoreIndirect("FormData_pg_attribute", "attcacheoff",
		rc1True1->   Load("thisatt"),
		rc1True1->   ConvertTo(Int32,
		rc1True1->      Load("off")));

		//in-else if ... off = att_align_pointer_cal
		att_align_pointer_cal(                        slow_off_att_align_nominal_match_else,
	    slow_off_att_align_nominal_match_else->          Load("off"),
		slow_off_att_align_nominal_match_else->          Add(
		slow_off_att_align_nominal_match_else->             Load("tp1"),
		slow_off_att_align_nominal_match_else->             Load("off")),
														 thisatt->attalign, "off_align_ptr", -1);


		slow_off_att_align_nominal_match_else->Store("off",
	    slow_off_att_align_nominal_match_else->   ConvertTo(Int32,
	    slow_off_att_align_nominal_match_else->      Load("off_align_ptr")));

		//in-else if ... slow = true;
		slow_off_att_align_nominal_match_else->Store("slow",
	    slow_off_att_align_nominal_match_else->   ConvertTo(bool_type,
		slow_off_att_align_nominal_match_else->      ConstInt16(true)));

        //in-else
		att_align_nominal_cal(thisatt_attlen_negative_match_else,
		thisatt_attlen_negative_match_else->      ConvertTo(Int32,
		thisatt_attlen_negative_match_else->         Load("off")),
												  "cal_off1", thisatt->attalign);

		thisatt_attlen_negative_match_else->Store("off",
		thisatt_attlen_negative_match_else->   ConvertTo(Int32,
		thisatt_attlen_negative_match_else->      Load("cal_off1")));

		//in-else ... if (!slow)
		OMR::JitBuilder::IlBuilder *slow_thisatt_attlen_negative_match_else = NULL;

		thisatt_attlen_negative_match_else->IfThen(&slow_thisatt_attlen_negative_match_else,
		thisatt_attlen_negative_match_else->   NotEqualTo(
		thisatt_attlen_negative_match_else->      ConvertTo(bool_type,
		thisatt_attlen_negative_match_else->         Load("slow")),
		thisatt_attlen_negative_match_else->      ConvertTo(bool_type,
		thisatt_attlen_negative_match_else->         ConstInt16(true))));

		//in-else ... thisatt->attcacheoff = off;
		slow_thisatt_attlen_negative_match_else->StoreIndirect("FormData_pg_attribute", "attcacheoff",
		slow_thisatt_attlen_negative_match_else->   Load("thisatt"),
		slow_thisatt_attlen_negative_match_else->   ConvertTo(Int32,
		slow_thisatt_attlen_negative_match_else->      Load("off")));

		//values[attnum] = fetchatt(thisatt, tp + off);

        fetch_attributes(null_false, "att",
        null_false->         Add(
        null_false->            Load("tp1"),
        null_false->			  ConvertTo(Int32,
        null_false->			     Load("off"))), thisatt);

		null_false->StoreAt(
		null_false->   IndexAt(pDatum,
		null_false->      LoadIndirect("TupleTableSlot", "tts_values",
		null_false->         Load("slot")),
		null_false->		 ConvertTo(Int32,
		null_false->          ConstInt32(attnum))),
		null_false->   ConvertTo(Address, null_false->Load("att")));
		//

		//off = att_addlength_pointer(off, thisatt->attlen, tp + off);
		att_addlength_pointer_cal(null_false,
		null_false->         ConvertTo(Int32,
		null_false->		      Load("off")), thisatt->attlen,
		null_false->         Add(
		null_false->            Load("tp1"),
		null_false->			  ConvertTo(Int32,
		null_false->               Load("off"))),
									 "offset");

		null_false->Store("off",
		null_false->   ConvertTo(Int32,
		null_false->Load("offset")     ));

		/* can't use attcacheoff anymore */

		if (thisatt->attlen <= 0)
		{
			Store("slow",
			   ConvertTo(bool_type,
			      ConstInt16(true)));
		}

	}

	/*
	 * Save state for next execution
	 */
	//slot_opt->tts_nvalid = attnum;
	StoreIndirect("TupleTableSlot", "tts_nvalid",
	   Load("slot"),
	   ConvertTo(Int16, ConstInt32(attnum)));

	//*off_opt = off;
	StoreAt(
	   Load("offp"),
	   ConvertTo(Int32, Load("off")));


	OMR::JitBuilder::IlBuilder *slow_final_match = NULL;
	OMR::JitBuilder::IlBuilder *slow_final_match_else = NULL;

	IfThenElse(&slow_final_match, &slow_final_match_else,
	   EqualTo(
	      ConvertTo(bool_type, Load("slow")),
	      ConvertTo(bool_type, ConstInt16(true))));
	{
    //if (slow)
	//slot->tts_flags |= TTS_FLAG_SLOW; TTS_FLAG_SLOW= (1 <<3) = 8
	slow_final_match->StoreIndirect("TupleTableSlot", "tts_flags",
	slow_final_match->   Load("slot"),
	slow_final_match->   Or(
	slow_final_match->      ConvertTo(Int16, slow_final_match->LoadIndirect("TupleTableSlot", "tts_flags",
	slow_final_match->         Load("slot"))),
	slow_final_match->      ConstInt16(TTS_FLAG_SLOW)));

	}

	{
	//else
	//slot->tts_flags &= ~TTS_FLAG_SLOW; TTS_FLAG_SLOW= (1 <<3) = 8
	slow_final_match_else->StoreIndirect("TupleTableSlot", "tts_flags",
	slow_final_match_else->   Load("slot"),
	slow_final_match_else->   And(
	slow_final_match_else->      ConvertTo(Int16, slow_final_match_else->LoadIndirect("TupleTableSlot", "tts_flags",
	slow_final_match_else->         Load("slot"))),
	slow_final_match_else->      ConstInt16(~TTS_FLAG_SLOW)));

	}

   Return();
   return true;


   }

class omr_expr_compile : public OMR::JitBuilder::MethodBuilder
   {
   private:

   OMR::JitBuilder::IlType *pStr;
   OMR::JitBuilder::IlType *pChar;
   OMR::JitBuilder::IlType *pDatum;
   OMR::JitBuilder::IlType *pDat;
   OMR::JitBuilder::IlType *pBits;
   OMR::JitBuilder::IlType *pInt32;
   OMR::JitBuilder::IlType *pInt16;
   OMR::JitBuilder::IlType *pBits8;
   OMR::JitBuilder::IlType *bool_type;
   OMR::JitBuilder::IlType *pbool;
   OMR::JitBuilder::IlType *size_t_type;
   OMR::JitBuilder::IlType *puint32;
   OMR::JitBuilder::IlType *datum_rep;
   OMR::JitBuilder::IlType *int32_rep;
   OMR::JitBuilder::IlType *int16_rep;
   OMR::JitBuilder::IlType *str_rep;
   OMR::JitBuilder::IlType *puint8;

   OMR::JitBuilder::IlType *StructTypeContext;
   OMR::JitBuilder::IlType *pStructTypeContext;

   OMR::JitBuilder::IlType *StructTypeTupleDesc;
   OMR::JitBuilder::IlType *pStructTypeTupleDesc;

   OMR::JitBuilder::IlType *StructTypeSlotOps;
   OMR::JitBuilder::IlType *pStructTypeSlotOps;

   OMR::JitBuilder::IlType *FormData_pg_attribute;
   OMR::JitBuilder::IlType *pFormData_pg_attribute;

   //OMR::JitBuilder::IlType *TupleTableSlot;
   //OMR::JitBuilder::IlType *pTupleTableSlot;

   OMR::JitBuilder::IlType *HeapTupleHeaderData;
   OMR::JitBuilder::IlType *pHeapTupleHeaderData;

   OMR::JitBuilder::IlType *HeapTuple;
   OMR::JitBuilder::IlType *pHeapTuple;

   OMR::JitBuilder::IlType *HeapTupleData;
   OMR::JitBuilder::IlType *pHeapTupleData;

   OMR::JitBuilder::IlType *TupleTableSlotType;
   OMR::JitBuilder::IlType *TupleTableSlotDesc;
   OMR::JitBuilder::IlType *pTupleTableSlot;
   OMR::JitBuilder::IlType *psize_t;
   OMR::JitBuilder::IlType *pAddress;
   OMR::JitBuilder::IlType *pInt8;


   OMR::JitBuilder::IlType *pExprState;



   public:

   	  TupleDesc	tupleDesc;
   	  omr_expr_compile(OMR::JitBuilder::TypeDictionary *);

      int32_t natts;
      TupleTableSlot *slot;
      ExprState *state;
      ExprContext *econtext;
      bool *isnull;

      void fetch_attributes(IlBuilder *, char *att, OMR::JitBuilder::IlValue *, Form_pg_attribute );
      void att_align_nominal_cal(IlBuilder *, OMR::JitBuilder::IlValue *, char *, char );
      void att_addlength_pointer_cal(IlBuilder *, OMR::JitBuilder::IlValue *, int16_t , OMR::JitBuilder::IlValue *, char *offset);
      void att_align_pointer_cal(IlBuilder *, OMR::JitBuilder::IlValue *, OMR::JitBuilder::IlValue *, char , char *, int16_t );

      virtual bool buildIL();

   };

//EEOP_DONE_func
static void
EEOP_DONE_func(bool *isnull, ExprState *state)
{
#define EEOP_DONE_func_LINE LINETOSTR(__LINE__)

	*isnull = state->resnull;
}

//EEOP_FUNCEXPR_STRICT_func
static void
EEOP_FUNCEXPR_STRICT_func(ExprEvalStep *op)
{
#define EEOP_FUNCEXPR_STRICT_func_LINE LINETOSTR(__LINE__)
	FunctionCallInfo fcinfo = op->d.func.fcinfo_data;
	NullableDatum *args = fcinfo->args;
	int			nargs = op->d.func.nargs;
	Datum		d;

	int flag = 0;
	/* strict function, so check for NULL args */
	for (int argno = 0; argno < nargs; argno++)
	{
		if (args[argno].isnull)
		{
			*op->resnull = true;
			flag++;
			break;
		}
	}

	if(flag == 0)
	{
		fcinfo->isnull = false;
		d = op->d.func.fn_addr(fcinfo);
		*op->resvalue = d;
		*op->resnull = fcinfo->isnull;
	}

}

//EEOP_AGG_ORDERED_TRANS_TUPLE_func
static void
EEOP_AGG_ORDERED_TRANS_TUPLE_func(ExprState *state, ExprEvalStep *op, ExprContext *econtext)
{
#define EEOP_AGG_ORDERED_TRANS_TUPLE_func_LINE LINETOSTR(__LINE__)

	ExecEvalAggOrderedTransTuple(state, op, econtext);

}

//EEOP_NOT_DISTINCT_func
static void
EEOP_NOT_DISTINCT_func(ExprEvalStep *op)
{
#define EEOP_NOT_DISTINCT_func_LINE LINETOSTR(__LINE__)

	FunctionCallInfo fcinfo = op->d.func.fcinfo_data;

	if (fcinfo->args[0].isnull && fcinfo->args[1].isnull)
	{
		*op->resvalue = BoolGetDatum(true);
		*op->resnull = false;
	}
	else if (fcinfo->args[0].isnull || fcinfo->args[1].isnull)
	{
		*op->resvalue = BoolGetDatum(false);
		*op->resnull = false;
	}
	else
	{
		Datum		eqresult;

		fcinfo->isnull = false;
		eqresult = op->d.func.fn_addr(fcinfo);
		*op->resvalue = eqresult;
		*op->resnull = fcinfo->isnull;
	}

}

//EEOP_CONST_func
static void
EEOP_CONST_func(ExprEvalStep *op)
{
#define EEOP_CONST_func_LINE LINETOSTR(__LINE__)

	*op->resnull = op->d.constval.isnull;
	*op->resvalue = op->d.constval.value;

}


//EEOP___VAR_func
static void
EEOP___VAR_func(ExprEvalStep *op, TupleTableSlot *slot)
{
#define EEOP___VAR_func_LINE LINETOSTR(__LINE__)

	int	attnum = op->d.var.attnum;
	*op->resvalue = slot->tts_values[attnum];
	*op->resnull = slot->tts_isnull[attnum];

}

static void
slot_getsomeattrs_int_func(TupleTableSlot *slot, ExprEvalStep *op)
{
#define slot_getsomeattrs_int_func_LINE LINETOSTR(__LINE__)

	int attnum = op->d.fetch.last_var;
	if (slot->tts_nvalid < attnum){
		/* Check for caller errors */
		Assert(slot->tts_nvalid < attnum);	/* checked in slot_getsomeattrs */
		Assert(attnum > 0);

		if (unlikely(attnum > slot->tts_tupleDescriptor->natts))
			elog(ERROR, "invalid attribute number %d", attnum);

		/* Fetch as many attributes as possible from the underlying tuple. */
		slot->tts_ops->getsomeattrs(slot, attnum);


		/*
		 * If the underlying tuple doesn't have enough attributes, tuple
		 * descriptor must have the missing attributes.
		 */
		if (unlikely(slot->tts_nvalid < attnum))
		{
			slot_getmissingattrs(slot, slot->tts_nvalid, attnum);
			slot->tts_nvalid = attnum;
		}
	}
}

/****************************************Define the method builder object********************************************************/

/*Expression Compiler
 OMRJIT*/
class expr_omr_vm : public OMR::JitBuilder::MethodBuilder
   {
   private:

   OMR::JitBuilder::IlType *pStr;
   OMR::JitBuilder::IlType *pChar;
   OMR::JitBuilder::IlType *pDatum;
   OMR::JitBuilder::IlType *pDat;
   OMR::JitBuilder::IlType *pBits;
   OMR::JitBuilder::IlType *pInt32;
   OMR::JitBuilder::IlType *pInt16;
   OMR::JitBuilder::IlType *pBits8;
   OMR::JitBuilder::IlType *bool_type;
   OMR::JitBuilder::IlType *pbool;
   OMR::JitBuilder::IlType *size_t_type;
   OMR::JitBuilder::IlType *puint32;
   OMR::JitBuilder::IlType *datum_rep;
   OMR::JitBuilder::IlType *int32_rep;
   OMR::JitBuilder::IlType *int16_rep;
   OMR::JitBuilder::IlType *str_rep;

   OMR::JitBuilder::IlType *StructTypeContext;
   OMR::JitBuilder::IlType *pStructTypeContext;

   OMR::JitBuilder::IlType *StructTypeTupleDesc;
   OMR::JitBuilder::IlType *pStructTypeTupleDesc;

   OMR::JitBuilder::IlType *StructTypeSlotOps;
   OMR::JitBuilder::IlType *pStructTypeSlotOps;

   OMR::JitBuilder::IlType *FormData_pg_attribute;
   OMR::JitBuilder::IlType *pFormData_pg_attribute;

   //OMR::JitBuilder::IlType *TupleTableSlot;
   //OMR::JitBuilder::IlType *pTupleTableSlot;

   OMR::JitBuilder::IlType *HeapTupleHeaderData;
   OMR::JitBuilder::IlType *pHeapTupleHeaderData;

   OMR::JitBuilder::IlType *HeapTuple;
   OMR::JitBuilder::IlType *pHeapTuple;

   OMR::JitBuilder::IlType *HeapTupleData;
   OMR::JitBuilder::IlType *pHeapTupleData;

   OMR::JitBuilder::IlType *TupleTableSlotType;
   OMR::JitBuilder::IlType *TupleTableSlotDesc;
   OMR::JitBuilder::IlType *pTupleTableSlot;
   OMR::JitBuilder::IlType *psize_t;
   OMR::JitBuilder::IlType *pExprState;
   OMR::JitBuilder::IlType *pExprContext;
   OMR::JitBuilder::IlType *pExprEvalStep;



   public:

   ExprState *state;
   ExprContext *econtext;
   bool *isNull;
   expr_omr_vm(OMR::JitBuilder::TypeDictionary *, ExprState *state, ExprContext *, bool *);
   virtual bool buildIL();

   };

/****************************************Define the method builder object********************************************************/

expr_omr_vm::expr_omr_vm(OMR::JitBuilder::TypeDictionary *types, ExprState *state, ExprContext *econtext, bool *isNull)
   : OMR::JitBuilder::MethodBuilder(types)
   {
   DefineLine(LINETOSTR(__LINE__));
   DefineFile(__FILE__);

   DefineName("expr_omr_vm");

   this->state = state;
   this->econtext = econtext;
   this->isNull = isNull;

   pStr = types->toIlType<char *>();
   pChar = types->toIlType<char>();
   pDatum = types->toIlType<Datum *>();
   pDat = types->toIlType<Datum>();
   pBits = types->toIlType<char>();
   pInt32 = types->PointerTo(Int32);
   pInt16 = types->PointerTo(Int16);

   pBits8 = types->toIlType<bits8 *>();

   bool_type = types->toIlType<bool>();
   pbool = types->toIlType<bool *>();

   size_t_type = types->toIlType<size_t>();

   puint32 = types->toIlType<uint32>();
   psize_t = types->toIlType<size_t>();

   datum_rep = types->PointerTo(pDatum);
   int32_rep = types->PointerTo(pInt32);
   int16_rep = types->PointerTo(pInt16);
   str_rep   = types->PointerTo(pStr);

   FormData_pg_attribute      = types->LookupStruct("FormData_pg_attribute");
   pFormData_pg_attribute     = types->PointerTo((char *)"FormData_pg_attribute");

   StructTypeTupleDesc = types->LookupStruct("TupleDescData");
   pStructTypeTupleDesc = types->PointerTo((char *)"TupleDescData");

   TupleTableSlotDesc = types->LookupStruct("TupleTableSlot");
   pTupleTableSlot = types->PointerTo((char *)"TupleTableSlot");

   HeapTuple      = types->LookupStruct("HeapTupleData");
   pHeapTuple     = types->PointerTo(HeapTuple);

   HeapTupleHeaderData      = types->LookupStruct("HeapTupleHeaderData");
   pHeapTupleHeaderData     = types->PointerTo((char *)"HeapTupleHeaderData");

   pExprState    = types->PointerTo((char *)"ExprState");
   pExprContext  = types->PointerTo((char *)"ExprContext");
   pExprEvalStep = types->PointerTo((char *)"ExprEvalStep");

   DefineParameter("state",     pExprState);
   DefineParameter("econtext",  pExprContext);
   DefineParameter("isnull",    pbool);

   DefineFunction((char *)"print_func",
                  (char *)__FILE__,
                  (char *)PRINTFUNC_LINE,
                  (void *)&print_func,
				  NoType,
                  2,
				  Int32,
				  Int32);

   DefineFunction((char *)"slot_getsomeattrs_int_func",
                  (char *)__FILE__,
                  (char *)slot_getsomeattrs_int_func_LINE,
                  (void *)&slot_getsomeattrs_int_func,
				  NoType,
                  2,
				  pTupleTableSlot,
				  pExprEvalStep);

   //EEOP___VAR
   DefineFunction((char *)"EEOP___VAR_func",
                  (char *)__FILE__,
                  (char *)EEOP___VAR_func_LINE,
                  (void *)&EEOP___VAR_func,
				  NoType,
                  2,
				  pExprEvalStep,
				  pTupleTableSlot);

   //EEOP_CONST
   DefineFunction((char *)"EEOP_CONST_func",
                  (char *)__FILE__,
                  (char *)EEOP_CONST_func_LINE,
                  (void *)&EEOP_CONST_func,
				  NoType,
                  1,
				  pExprEvalStep);

   //EEOP_FUNCEXPR_STRICT_func
   DefineFunction((char *)"EEOP_FUNCEXPR_STRICT_func",
                  (char *)__FILE__,
                  (char *)EEOP_FUNCEXPR_STRICT_func_LINE,
                  (void *)&EEOP_FUNCEXPR_STRICT_func,
				  NoType,
                  1,
				  pExprEvalStep);

   //EEOP_DONE_func
   DefineFunction((char *)"EEOP_DONE_func",
                  (char *)__FILE__,
                  (char *)EEOP_DONE_func_LINE,
                  (void *)&EEOP_DONE_func,
				  NoType,
                  2,
				  pbool,
				  pExprState);

   //EEOP_NOT_DISTINCT
   DefineFunction((char *)"EEOP_NOT_DISTINCT_func",
                  (char *)__FILE__,
                  (char *)EEOP_NOT_DISTINCT_func_LINE,
                  (void *)&EEOP_NOT_DISTINCT_func,
				  NoType,
                  1,
				  pExprEvalStep);

   //EEOP_AGG_ORDERED_TRANS_TUPLE
   DefineFunction((char *)"EEOP_AGG_ORDERED_TRANS_TUPLE_func",
                  (char *)__FILE__,
                  (char *)EEOP_AGG_ORDERED_TRANS_TUPLE_func_LINE,
                  (void *)&EEOP_AGG_ORDERED_TRANS_TUPLE_func,
				  NoType,
                  3,
				  pExprState,
				  pExprEvalStep,
				  pExprContext);





   /* Define Return type */
   DefineReturnType(Address);

   }
static const char *const opcode_names[] = {
		"CASE_EEOP_DONE",
		"CASE_EEOP_INNER_FETCHSOME",
		"CASE_EEOP_OUTER_FETCHSOME",
		"CASE_EEOP_SCAN_FETCHSOME",
		"CASE_EEOP_INNER_VAR",
		"CASE_EEOP_OUTER_VAR",
		"CASE_EEOP_SCAN_VAR",
		"CASE_EEOP_INNER_SYSVAR",
		"CASE_EEOP_OUTER_SYSVAR",
		"CASE_EEOP_SCAN_SYSVAR",
		"CASE_EEOP_WHOLEROW",
		"CASE_EEOP_ASSIGN_INNER_VAR",
		"CASE_EEOP_ASSIGN_OUTER_VAR",
		"CASE_EEOP_ASSIGN_SCAN_VAR",
		"CASE_EEOP_ASSIGN_TMP",
		"CASE_EEOP_ASSIGN_TMP_MAKE_RO",
		"CASE_EEOP_CONST",
		"CASE_EEOP_FUNCEXPR",
		"CASE_EEOP_FUNCEXPR_STRICT",
		"CASE_EEOP_FUNCEXPR_FUSAGE",
		"CASE_EEOP_FUNCEXPR_STRICT_FUSAGE",
		"CASE_EEOP_BOOL_AND_STEP_FIRST",
		"CASE_EEOP_BOOL_AND_STEP",
		"CASE_EEOP_BOOL_AND_STEP_LAST",
		"CASE_EEOP_BOOL_OR_STEP_FIRST",
		"CASE_EEOP_BOOL_OR_STEP",
		"CASE_EEOP_BOOL_OR_STEP_LAST",
		"CASE_EEOP_BOOL_NOT_STEP",
		"CASE_EEOP_QUAL",
		"CASE_EEOP_JUMP",
		"CASE_EEOP_JUMP_IF_NULL",
		"CASE_EEOP_JUMP_IF_NOT_NULL",
		"CASE_EEOP_JUMP_IF_NOT_TRUE",
		"CASE_EEOP_NULLTEST_ISNULL",
		"CASE_EEOP_NULLTEST_ISNOTNULL",
		"CASE_EEOP_NULLTEST_ROWISNULL",
		"CASE_EEOP_NULLTEST_ROWISNOTNULL",
		"CASE_EEOP_BOOLTEST_IS_TRUE",
		"CASE_EEOP_BOOLTEST_IS_NOT_TRUE",
		"CASE_EEOP_BOOLTEST_IS_FALSE",
		"CASE_EEOP_BOOLTEST_IS_NOT_FALSE",
		"CASE_EEOP_PARAM_EXEC",
		"CASE_EEOP_PARAM_EXTERN",
		"CASE_EEOP_PARAM_CALLBACK",
		"CASE_EEOP_CASE_TESTVAL",
		"CASE_EEOP_MAKE_READONLY",
		"CASE_EEOP_IOCOERCE",
		"CASE_EEOP_DISTINCT",
		"CASE_EEOP_NOT_DISTINCT",
		"CASE_EEOP_NULLIF",
		"CASE_EEOP_SQLVALUEFUNCTION",
		"CASE_EEOP_CURRENTOFEXPR",
		"CASE_EEOP_NEXTVALUEEXPR",
		"CASE_EEOP_ARRAYEXPR",
		"CASE_EEOP_ARRAYCOERCE",
		"CASE_EEOP_ROW",
		"CASE_EEOP_ROWCOMPARE_STEP",
		"CASE_EEOP_ROWCOMPARE_FINAL",
		"CASE_EEOP_MINMAX",
		"CASE_EEOP_FIELDSELECT",
		"CASE_EEOP_FIELDSTORE_DEFORM",
		"CASE_EEOP_FIELDSTORE_FORM",
		"CASE_EEOP_SBSREF_SUBSCRIPT",
		"CASE_EEOP_SBSREF_OLD",
		"CASE_EEOP_SBSREF_ASSIGN",
		"CASE_EEOP_SBSREF_FETCH",
		"CASE_EEOP_DOMAIN_TESTVAL",
		"CASE_EEOP_DOMAIN_NOTNULL",
		"CASE_EEOP_DOMAIN_CHECK",
		"CASE_EEOP_CONVERT_ROWTYPE",
		"CASE_EEOP_SCALARARRAYOP",
		"CASE_EEOP_XMLEXPR",
		"CASE_EEOP_AGGREF",
		"CASE_EEOP_GROUPING_FUNC",
		"CASE_EEOP_WINDOW_FUNC",
		"CASE_EEOP_SUBPLAN",
		"CASE_EEOP_ALTERNATIVE_SUBPLAN",
		"CASE_EEOP_AGG_STRICT_DESERIALIZE",
		"CASE_EEOP_AGG_DESERIALIZE",
		"CASE_EEOP_AGG_STRICT_INPUT_CHECK_ARGS",
		"CASE_EEOP_AGG_STRICT_INPUT_CHECK_NULLS",
		"CASE_EEOP_AGG_PLAIN_PERGROUP_NULLCHECK",
		"CASE_EEOP_AGG_PLAIN_TRANS_INIT_STRICT_BYVAL",
		"CASE_EEOP_AGG_PLAIN_TRANS_STRICT_BYVAL",
		"CASE_EEOP_AGG_PLAIN_TRANS_BYVAL",
		"CASE_EEOP_AGG_PLAIN_TRANS_INIT_STRICT_BYREF",
		"CASE_EEOP_AGG_PLAIN_TRANS_STRICT_BYREF",
		"CASE_EEOP_AGG_PLAIN_TRANS_BYREF",
		"CASE_EEOP_AGG_ORDERED_TRANS_DATUM",
		"CASE_EEOP_AGG_ORDERED_TRANS_TUPLE",
		"CASE_EEOP_LAST"
};
/******************************BUILDIL Tuple deformation**********************************************/

bool
expr_omr_vm::buildIL()
{
	TupleTableSlot *resultslot = state->resultslot;
	Store("resultslot",
	   LoadIndirect("ExprState", "resultslot",
	      Load("state")));

	TupleTableSlot *innerslot = econtext->ecxt_innertuple;
	Store("innerslot",
	   LoadIndirect("ExprContext", "ecxt_innertuple",
	      Load("econtext")));

	TupleTableSlot *outerslot = econtext->ecxt_outertuple;
	Store("outerslot",
	   LoadIndirect("ExprContext", "ecxt_outertuple",
	      Load("econtext")));

	TupleTableSlot *scanslot = econtext->ecxt_scantuple;
	Store("scanslot",
	   LoadIndirect("ExprContext", "ecxt_scantuple",
	      Load("econtext")));

	//op = state->steps;
	Store("op", LoadIndirect("ExprState", "steps", Load("state")));


	OMR::JitBuilder::BytecodeBuilder **builders = (OMR::JitBuilder::BytecodeBuilder **) malloc(state->steps_len * sizeof(OMR::JitBuilder::BytecodeBuilder *));

	int32_t opno;
	//initialize the builder objects
	for (opno = 0; opno < state->steps_len; opno++)
	{
		builders[opno] = OrphanBytecodeBuilder(opno, (char *)opcode_names[opno]);
	}

	OMR::JitBuilder::VirtualMachineState *vmState = new OMR::JitBuilder::VirtualMachineState();
	setVMState(vmState);

	AllLocalsHaveBeenDefined();

	AppendBytecodeBuilder(builders[0]);
	opno = GetNextBytecodeFromWorklist();

	while (opno != -1) {
		//elog(INFO, "opno in print: %d\n", opno);
		OMR::JitBuilder::BytecodeBuilder *b = builders[opno];

		ExprEvalStep *op;
		ExprEvalOp	opcode;

		op = &state->steps[opno];
		opcode = ExecEvalStepOp(state, op);

		switch (opcode)
		{
			case EEOP_DONE://use
			{
				/**isNull = state->resnull;*/
				/*b->StoreAt(
				b->   Load("isnull"),
				b->   ConvertTo(bool_type,
				b->      LoadIndirect("ExprState", "resnull",
				b->         Load("state"))));*/

				b->Call("EEOP_DONE_func", 2, b->Load("isnull"), b->Load("state"));

				break;
			}
			case EEOP_INNER_FETCHSOME://use
			{
				//slot_getsomeattrs(innerslot, op->d.fetch.last_var);
				b->Call("slot_getsomeattrs_int_func", 2, b->Load("innerslot"), b->Load("op"));
				b->AddFallThroughBuilder(builders[opno+1]);

				break;
			}
			case EEOP_OUTER_FETCHSOME://use
			{
				//slot_getsomeattrs(outerslot, op->d.fetch.last_var);
				b->Call("slot_getsomeattrs_int_func", 2, b->Load("outerslot"), b->Load("op"));
				b->AddFallThroughBuilder(builders[opno+1]);

				break;
			}
			case EEOP_SCAN_FETCHSOME://use
			{
				//slot_getsomeattrs(scanslot, op->d.fetch.last_var);
				b->Call("slot_getsomeattrs_int_func", 2, b->Load("scanslot"), b->Load("op"));
				b->AddFallThroughBuilder(builders[opno+1]);

				break;
			}
			case EEOP_INNER_VAR://use
			{
				b->Call("EEOP___VAR_func", 2, b->Load("op"), b->Load("innerslot"));
				b->AddFallThroughBuilder(builders[opno+1]);

				break;
			}
			case EEOP_OUTER_VAR://use
			{
				b->Call("EEOP___VAR_func", 2, b->Load("op"), b->Load("outerslot"));
				b->AddFallThroughBuilder(builders[opno+1]);


				break;
			}
			case EEOP_SCAN_VAR://use
			{
				b->Call("EEOP___VAR_func", 2, b->Load("op"), b->Load("scanslot"));
				b->AddFallThroughBuilder(builders[opno+1]);

				break;
			}
			case EEOP_INNER_SYSVAR:
			{
				break;
			}
			case EEOP_OUTER_SYSVAR:
			{
				break;
			}
			case EEOP_SCAN_SYSVAR:
			{
				break;
			}
			case EEOP_WHOLEROW:
			{
				break;
			}
			case EEOP_ASSIGN_INNER_VAR:
			{

				break;
			}
			case EEOP_ASSIGN_OUTER_VAR:
			{
				break;
			}
			case EEOP_ASSIGN_SCAN_VAR:
			{
				break;
			}
			case EEOP_ASSIGN_TMP:
			{
				break;
			}
			case EEOP_ASSIGN_TMP_MAKE_RO:
			{
				break;
			}
			case EEOP_CONST://use
			{

				b->Call("EEOP_CONST_func", 1, b->Load("op"));
				b->AddFallThroughBuilder(builders[opno+1]);

				break;
			}
			case EEOP_FUNCEXPR:
			{
				break;
			}
			case EEOP_FUNCEXPR_STRICT://use
			{

				b->Call("EEOP_FUNCEXPR_STRICT_func", 1, b->Load("op"));
				b->AddFallThroughBuilder(builders[opno+1]);

				break;
			}
			case EEOP_FUNCEXPR_FUSAGE:
			{
				break;
			}
			case EEOP_FUNCEXPR_STRICT_FUSAGE:
			{
				break;
			}
			case EEOP_BOOL_AND_STEP_FIRST:
			{

				break;
			}
			case EEOP_BOOL_AND_STEP:
			{

				break;
			}
			case EEOP_BOOL_AND_STEP_LAST:
			{

				break;
			}
			case EEOP_BOOL_OR_STEP_FIRST:
			{

				break;
			}
			case EEOP_BOOL_OR_STEP:
			{

				break;
			}
			case EEOP_BOOL_OR_STEP_LAST:
			{

				break;
			}
			case EEOP_BOOL_NOT_STEP:
			{

				break;
			}
			case EEOP_QUAL://use
			{
				/*if (*op->resnull ||
					!DatumGetBool(*op->resvalue))
				{
					*op->resnull = false;
					*op->resvalue = BoolGetDatum(false);
					//EEO_JUMP(op->d.qualexpr.jumpdone);
					b->Store("j", b->ConstInt32(0));
					b->AddFallThroughBuilder(builders[op->d.qualexpr.jumpdone]);
				}*/
				//b->Call("EEOP_QUAL_func", 2, b->Load("op"));
				b->AddFallThroughBuilder(builders[opno+1]);

				break;
			}
			case EEOP_JUMP:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_JUMP_IF_NULL:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_JUMP_IF_NOT_NULL:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_JUMP_IF_NOT_TRUE:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_NULLTEST_ISNULL:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_NULLTEST_ISNOTNULL:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_NULLTEST_ROWISNULL:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_NULLTEST_ROWISNOTNULL:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_BOOLTEST_IS_TRUE:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_BOOLTEST_IS_NOT_FALSE:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_BOOLTEST_IS_FALSE:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_BOOLTEST_IS_NOT_TRUE:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_PARAM_EXEC:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_PARAM_EXTERN:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_PARAM_CALLBACK:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_SBSREF_SUBSCRIPTS:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_SBSREF_OLD:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_SBSREF_ASSIGN:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_SBSREF_FETCH:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_CASE_TESTVAL:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_MAKE_READONLY:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_IOCOERCE:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_DISTINCT:
			{
				b->Call("EEOP_NOT_DISTINCT_func", 1, b->Load("op"));
				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_NOT_DISTINCT:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_NULLIF:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_SQLVALUEFUNCTION:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_CURRENTOFEXPR:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_NEXTVALUEEXPR:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_ARRAYEXPR:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_ARRAYCOERCE:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_ROW:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_ROWCOMPARE_STEP:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_ROWCOMPARE_FINAL:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_MINMAX:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_FIELDSELECT:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_FIELDSTORE_DEFORM:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_FIELDSTORE_FORM:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_DOMAIN_TESTVAL:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_DOMAIN_NOTNULL:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_DOMAIN_CHECK:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_CONVERT_ROWTYPE:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_SCALARARRAYOP:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_XMLEXPR:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_AGGREF:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_GROUPING_FUNC:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_WINDOW_FUNC:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_SUBPLAN:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_AGG_STRICT_DESERIALIZE:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_AGG_DESERIALIZE:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_AGG_STRICT_INPUT_CHECK_ARGS:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_AGG_STRICT_INPUT_CHECK_NULLS:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_AGG_PLAIN_PERGROUP_NULLCHECK:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_AGG_PLAIN_TRANS_INIT_STRICT_BYVAL:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_AGG_PLAIN_TRANS_STRICT_BYVAL:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_AGG_PLAIN_TRANS_BYVAL:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_AGG_PLAIN_TRANS_INIT_STRICT_BYREF:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_AGG_PLAIN_TRANS_STRICT_BYREF:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_AGG_PLAIN_TRANS_BYREF:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_AGG_ORDERED_TRANS_DATUM:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_AGG_ORDERED_TRANS_TUPLE:
			{
				b->Call("EEOP_AGG_ORDERED_TRANS_TUPLE_func", 3, b->Load("state"), b->Load("op"), b->Load("econtext"));

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_LAST:
			{

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
		}
		opno = GetNextBytecodeFromWorklist();
	}

	free(builders);

	Return(ConstInt8(1));

	return true;
}

/*
 *
 * end
 *
 * */

extern "C" {
PG_MODULE_MAGIC;

/*function to compile tuple deformation Jit code*/
OMRJIT_slot_deformFunctionType * omr_compile(int32_t natts, TupleDesc tupleDesc, TupleTableSlot *slot){

	StructTypeDictionary joinmethodTypes;

	slot_compile_deform method(&joinmethodTypes, natts, tupleDesc, slot);
	void *entry=0;
	int32_t rc = compileMethodBuilder(&method, &entry);

	if (rc != 0)
	{

		elog(INFO, "FAIL: compilation error %d\n", rc);
	   exit(-2);
	}
	/*slot_deform = (OMRJIT_slot_deformFunctionType *)entry;*/
	return (OMRJIT_slot_deformFunctionType *)entry;

}

/*function to compile expression evaluation code*/
omr_expression_bytecode_computation_FunctionType *omr_expr_compiler(ExprState *state, ExprContext *econtext, bool *isNull){

	StructTypeDictionary expr_types;
	expr_omr_vm method(&expr_types, state, econtext, isNull);
	void *entry=0;
	elog(INFO, "START: compilation \n");
	int32_t rc = compileMethodBuilder(&method, &entry);
	elog(INFO, "PASS: compilation \n");
	if (rc != 0)
	{
	   elog(INFO, "FAIL: compilation error %d\n", rc);
	   exit(-2);
	}
	return (omr_expression_bytecode_computation_FunctionType *)entry;

}



}















