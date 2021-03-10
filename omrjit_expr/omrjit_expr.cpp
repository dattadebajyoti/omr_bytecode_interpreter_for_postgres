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

#define MAX 1000

}//extern block ended

#include "/home/debajyoti/vm-project/src/lib/omr_debug/omr/jitbuilder/release/cpp/include/JitBuilder.hpp"
#include <iostream>
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
static void print_func(int32_t a, int32_t b) {
    #define PRINTFUNC_LINE LINETOSTR(__LINE__)

	elog(INFO, "a,b in print: %d %d\n", a, b);

}

static void print_state_func(ExprState *state) {
    #define print_state_func_LINE LINETOSTR(__LINE__)

	elog(INFO,"----------------------------------int state print---------------------------------");
	ExprEvalStep *op = state->steps;
	FunctionCallInfo fcinfo = op->d.func.fcinfo_data;
	int			nargs = op->d.func.nargs;
	/*for (int argno = 0; argno < 2; argno++)
	{
		elog(INFO,"arg in loop------------------------------------------------------- %d\n",argno);
		elog(INFO,"arg in loop------------------------------------------------------- %d\n", ((int32) ((fcinfo->args[argno].value))));
	}*/

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

static void
slot_getsomeattrs_int_func(TupleTableSlot *slot, int32_t  attnum)
{
#define slot_getsomeattrs_int_func_LINE LINETOSTR(__LINE__)

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

//EEOP_SCAN_VAR_func
static void
EEOP_SCAN_VAR_func(ExprEvalStep *op, TupleTableSlot *scanslot)
{
#define EEOP_SCAN_VAR_func_LINE LINETOSTR(__LINE__)

	int			attnum = op->d.var.attnum;
	*op->resvalue = scanslot->tts_values[attnum];
	*op->resnull = scanslot->tts_isnull[attnum];
}



//ExecEvalStepOp_func
static void
ExecEvalStepOp_func(ExprState *state)
{
#define ExecEvalStepOp_func_LINE LINETOSTR(__LINE__)

	elog(INFO,"call state from buildil------------------------------------------------------- %d\n", state->steps_len);
}


}//extern C block ended


/****************Define class******************/

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

   this->tupleDesc = tupleDesc;
   this->natts = natts;
   this->slot = slot;


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


   DefineParameter("natts",    Int32);

   DefineParameter("slot",     pTupleTableSlot);
   DefineParameter("tuple",    HeapTuple);

   DefineParameter("offp",     pInt32);
   DefineParameter("tp1",     pStr);

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
                  Int32, pChar, Int32, pStr);

   //fetchatt_func
   DefineFunction((char *)"fetchatt_func",
                  (char *)__FILE__,
                  (char *)FETCHATTFUNC_LINE,
                  (void *)&fetchatt_func,
                  pDat,
                  3,
				  Address, pStr, Int32);

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
				  Int32, Int16, pStr);

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
	OMR::JitBuilder::IlValue *ld_tupleDesc = LoadIndirect("TupleTableSlot","tts_tupleDescriptor", Load("slot"));

	//HeapTupleHeader tup = tuple_opt->t_data;
    OMR::JitBuilder::IlValue *ld_tup = LoadIndirect("HeapTupleData", "t_data", Load("tuple"));

    OMR::JitBuilder::IlValue *ld_t_bits = LoadIndirect("HeapTupleHeaderData", "t_bits", ld_tup);

    OMR::JitBuilder::IlValue *ld_tts_values = LoadIndirect("TupleTableSlot", "tts_values", Load("slot"));
    OMR::JitBuilder::IlValue *ld_tts_isnull = LoadIndirect("TupleTableSlot", "tts_isnull", Load("slot"));

	OMR::JitBuilder::IlValue *off_value = ConstInt32(0);


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
		off_value = ConvertTo(Int32,
		               LoadAt(pInt32,
		                  Load("offp")));

	}



	for(; attnum < natts; attnum++)

	{
		Form_pg_attribute thisatt = TupleDescAttr(tupleDesc, attnum);

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
												 att_isnull_builder->               IndexAt(pBits8, ld_t_bits,
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
		null_true->   IndexAt(pDatum, ld_tts_values,
		null_true->	  ConvertTo(Int32,
		null_true->	     ConstInt32(attnum))),
		null_true->   ConvertTo(Address,
	    null_true->	     ConstInt32(0)));

		//isnull[attnum] = true;
		null_true->StoreAt(
		null_true->   IndexAt(pbool, ld_tts_isnull,
		null_true->      ConvertTo(Int32,
		null_true->         ConstInt32(attnum))),
		null_true->   ConvertTo(bool_type,
		null_true->      ConstInt16(true)));


        //continue

		//isnull[attnum] = false;
		null_false->StoreAt(
		null_false->   IndexAt(pbool, ld_tts_isnull,
		null_false->		 ConvertTo(Int32,
		null_false->          ConstInt32(attnum))),
		null_false->   ConvertTo(bool_type,
		null_false->      ConstInt16(false)));

		//

		if(thisatt->attlen == -1)
		{
			//in-else if ... off = att_align_pointer_cal
			att_align_pointer_cal(null_false, off_value,
			null_false->          Add(
			null_false->             Load("tp1"), off_value),
								  thisatt->attalign, "off_align_ptr", -1);


			off_value = null_false->   ConvertTo(Int32,
					    null_false->      Load("off_align_ptr"));

		}
		else
		{
			att_align_nominal_cal(null_false,
			null_false->      ConvertTo(Int32, off_value),
							  "cal_off1", thisatt->attalign);


			off_value = null_false->   ConvertTo(Int32,
					    null_false->      Load("cal_off1"));

		}
		//

		//values[attnum] = fetchatt(thisatt, tp + off);

        fetch_attributes(null_false, "att",
        null_false->         Add(
        null_false->            Load("tp1"),
        null_false->			  ConvertTo(Int32, off_value)), thisatt);

		null_false->StoreAt(
		null_false->   IndexAt(pDatum, ld_tts_values,
		null_false->		 ConvertTo(Int32,
		null_false->          ConstInt32(attnum))),
		null_false->   ConvertTo(Address,
		null_false->      Load("att")));
		//

		//off = att_addlength_pointer(off, thisatt->attlen, tp + off);
		att_addlength_pointer_cal(null_false,
		null_false->         ConvertTo(Int32, off_value), thisatt->attlen,
		null_false->         Add(
		null_false->            Load("tp1"),
		null_false->			  ConvertTo(Int32, off_value)),
									 "offset");


        off_value = null_false->   ConvertTo(Int32,
        		null_false->Load("offset")     );

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
	   ConvertTo(Int32, off_value));

	//slot->tts_flags |= TTS_FLAG_SLOW; TTS_FLAG_SLOW= (1 <<3) = 8
	StoreIndirect("TupleTableSlot", "tts_flags",
	   Load("slot"),
	   Or(
	      ConvertTo(Int16, LoadIndirect("TupleTableSlot", "tts_flags",
	         Load("slot"))),
	      ConstInt16(TTS_FLAG_SLOW)));



   Return();
   return true;


   }


/*End*/

/****************Define class******************/

class omrjit_compile_expr : public OMR::JitBuilder::MethodBuilder
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
   OMR::JitBuilder::IlType *pExprContext;
   OMR::JitBuilder::IlType *ExprEvalStepStruct;
   OMR::JitBuilder::IlType *pExprEvalStep;
   OMR::JitBuilder::IlType *exprState;
   OMR::JitBuilder::IlType *pExprEvalOp;

   OMR::JitBuilder::IlType *pNullableDatum;
   OMR::JitBuilder::IlType *FunctionCallInfoBaseDataStruct;
   OMR::JitBuilder::IlType *pFunctionCallInfoBaseDataStruct;
   OMR::JitBuilder::IlType *dUnion;

   OMR::JitBuilder::IlType *pstep_jit;



   public:

      omrjit_compile_expr(OMR::JitBuilder::TypeDictionary *, ExprState *,
    		  TupleTableSlot *, TupleTableSlot *, TupleTableSlot *, TupleTableSlot *);

      TupleTableSlot *resultslot;
      TupleTableSlot *innerslot;
      TupleTableSlot *outerslot;
      TupleTableSlot *scanslot;
      //ExprEvalStep *op;
      ExprState *state;

      virtual bool buildIL();

   };

/****************************************Define the method builder object********************************************************/

omrjit_compile_expr::omrjit_compile_expr(OMR::JitBuilder::TypeDictionary *types, ExprState *state,
		TupleTableSlot *resultslot, TupleTableSlot *innerslot, TupleTableSlot *outerslot, TupleTableSlot *scanslot)
   : OMR::JitBuilder::MethodBuilder(types)
   {
   DefineLine(LINETOSTR(__LINE__));
   DefineFile(__FILE__);

   DefineName("omrjit_compile_expr");

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
   pbool = types->toIlType<bool *>();

   size_t_type = types->toIlType<size_t>();

   puint32 = types->toIlType<uint32>();
   psize_t = types->toIlType<size_t>();

   pExprEvalOp = types->toIlType<intptr_t>();

   datum_rep = types->PointerTo(pDatum);
   int32_rep = types->PointerTo(pInt32);
   int16_rep = types->PointerTo(pInt16);
   str_rep   = types->PointerTo(pStr);
   pAddress = types->PointerTo(Address);

   this->state = state;
   this->resultslot = resultslot;
   this->innerslot = innerslot;
   this->outerslot = outerslot;
   this->scanslot = scanslot;


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
   //pExprContext  = types->PointerTo((char *)"ExprContext");
   ExprEvalStepStruct      = types->LookupStruct("ExprEvalStep");
   pExprEvalStep = types->PointerTo((char *)"ExprEvalStep");

   pNullableDatum = types->PointerTo((char *)"NullableDatum");
   FunctionCallInfoBaseDataStruct = types->LookupStruct("FunctionCallInfoBaseData");
   pFunctionCallInfoBaseDataStruct = types->PointerTo((char *)"FunctionCallInfoBaseData");

   dUnion = types->LookupUnion("d");

   //ExprContext
   pExprContext = types->PointerTo((char *)"ExprContext");


   DefineParameter("econtext",     pExprContext);
   //DefineParameter("scanslot",     pTupleTableSlot);
   DefineParameter("natts",     Int32);
   DefineParameter("var_attnum",     Int32);
   DefineParameter("op",     pExprEvalStep);
   DefineParameter("state",  pExprState);
   DefineParameter("isnull",  pbool);



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
                  Int32, pChar, Int32, pStr);

   //fetchatt_func
   DefineFunction((char *)"fetchatt_func",
                  (char *)__FILE__,
                  (char *)FETCHATTFUNC_LINE,
                  (void *)&fetchatt_func,
                  pDat,
                  3,
				  Address, pStr, Int32);

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
				  Int32, Int16, pStr);

   DefineFunction((char *)"print_func",
                  (char *)__FILE__,
                  (char *)PRINTFUNC_LINE,
                  (void *)&print_func,
				  NoType,
                  2,
				  Int32,
				  Int32);

   DefineFunction((char *)"print_state_func",
                  (char *)__FILE__,
                  (char *)print_state_func_LINE,
                  (void *)&print_state_func,
				  NoType,
                  1,
				  pExprState);

   DefineFunction((char *)"store_isnull",
                  (char *)__FILE__,
                  (char *)ISNULL_LINE,
                  (void *)&store_isnull,
				  NoType,
                  2,
				  Int32, Int16);

   DefineFunction((char *)"slot_getsomeattrs_int_func",
                  (char *)__FILE__,
                  (char *)slot_getsomeattrs_int_func_LINE,
                  (void *)&slot_getsomeattrs_int_func,
				  NoType,
                  2,
				  pTupleTableSlot,
				  Int32);

   //EEOP_SCAN_VAR_func
   DefineFunction((char *)"EEOP_SCAN_VAR_func",
                  (char *)__FILE__,
                  (char *)EEOP_SCAN_VAR_func_LINE,
                  (void *)&EEOP_SCAN_VAR_func,
				  NoType,
                  2,
				  pInt32,
				  pTupleTableSlot);

   //ExecEvalStepOp_func
   DefineFunction((char *)"ExecEvalStepOp_func",
                  (char *)__FILE__,
                  (char *)ExecEvalStepOp_func_LINE,
                  (void *)&ExecEvalStepOp_func,
				  NoType,
                  1,
				  pExprState);

   /* Define Return type */
   DefineReturnType(Address);

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

	   	   //
	   	   DefineStruct("NullableDatum");
           OMR::JitBuilder::IlType *pNullableDatum = PointerTo("NullableDatum");

	   	   DefineField("NullableDatum", "value", toIlType<Datum >());
	   	   DefineField("NullableDatum", "isnull", toIlType<bool>());

	       CloseStruct("NullableDatum");

	       //FunctionCallInfoBaseData
		   DEFINE_STRUCT(FunctionCallInfoBaseData);
           OMR::JitBuilder::IlType *pFunctionCallInfoBaseData = PointerTo("FunctionCallInfoBaseData");

		   DEFINE_FIELD(FunctionCallInfoBaseData, nargs, toIlType<short>());
		   DEFINE_FIELD(FunctionCallInfoBaseData, args, pNullableDatum);

		   CLOSE_STRUCT(FunctionCallInfoBaseData);

		   OMR::JitBuilder::IlType *func_Type = DefineStruct("func");

		   DefineField("func", "fcinfo_data", pFunctionCallInfoBaseData);
		   DefineField("func", "nargs", Int32);

		   CloseStruct("func");

		   //union
           OMR::JitBuilder::IlType *d_Type = DefineUnion("d");

     	   UnionField("d", "func", func_Type);
      	   //UnionField("d", "nargs", Int32);

      	   CloseUnion("d");
	   	   //

	   	   /*Declare ExprEvalStep*/
	   	   DEFINE_STRUCT(ExprEvalStep);
	       OMR::JitBuilder::IlType *pExprEvalStepType = PointerTo("ExprEvalStep");

	       DEFINE_FIELD(ExprEvalStep, opcode, toIlType<intptr_t>());
	   	   DEFINE_FIELD(ExprEvalStep, resvalue, toIlType<Datum *>());
	   	   DEFINE_FIELD(ExprEvalStep, resnull, 	toIlType<bool *>());
	   	   DEFINE_FIELD(ExprEvalStep, operatorId, 	toIlType<Oid>());
	   	   DEFINE_FIELD(ExprEvalStep, d, 	d_Type);

	   	   CLOSE_STRUCT(ExprEvalStep);

	   	   /*DEFINE_STRUCT(step_jit);
	       OMR::JitBuilder::IlType *pstep_jitType = PointerTo("step_jit");

	   	   DEFINE_FIELD(step_jit, resvalue, toIlType<Datum *>());
	   	   DEFINE_FIELD(step_jit, resnull, 	toIlType<bool *>());

	   	   CLOSE_STRUCT(step_jit);*/


	   	   /*Declare  Expression*/
	   	   OMR::JitBuilder::IlType *ExprStateType = DEFINE_STRUCT(ExprState);

	   	   DEFINE_FIELD(ExprState, resnull, d.toIlType<bool>());
	   	   DEFINE_FIELD(ExprState, resvalue, d.toIlType<Datum>());
	   	   DEFINE_FIELD(ExprState, resultslot, pTupleTableSlotType);
	   	   DEFINE_FIELD(ExprState, steps, pExprEvalStepType);
	   	   DEFINE_FIELD(ExprState, op_resvalue, d.toIlType<Datum *>());
	   	   DEFINE_FIELD(ExprState, op_resnull, d.toIlType<bool *>());

	   	   CLOSE_STRUCT(ExprState);

	   	   /*Declare  econtext*/
	   	   OMR::JitBuilder::IlType *ExprContextType = DEFINE_STRUCT(ExprContext);

	   	   DEFINE_FIELD(ExprContext, ecxt_scantuple, pTupleTableSlotType);
	   	   DEFINE_FIELD(ExprContext, ecxt_innertuple, pTupleTableSlotType);
	   	   DEFINE_FIELD(ExprContext, ecxt_outertuple, pTupleTableSlotType);
	   	   DEFINE_FIELD(ExprContext, ecxt_aggvalues, d.toIlType<Datum *>());
	   	   DEFINE_FIELD(ExprContext, ecxt_aggnulls, d.toIlType<bool *>());
	   	   DEFINE_FIELD(ExprContext, caseValue_datum, d.toIlType<Datum>());
	   	   DEFINE_FIELD(ExprContext, caseValue_isNull, d.toIlType<bool>());
	   	   DEFINE_FIELD(ExprContext, domainValue_datum, d.toIlType<Datum>());
	   	   DEFINE_FIELD(ExprContext, domainValue_isNull, d.toIlType<bool>());

	   	   CLOSE_STRUCT(ExprState);

      }
   };


/******************************BUILDIL Expression compilation**********************************************/
static const char *const opcode_names[6] = {
		"EEOP_DONE",
		"EEOP_OMRJITCOMPILE_EXPR",
		"EEOP_SCAN_FETCHSOME",
		"EEOP_SCAN_VAR",
		"EEOP_FUNCEXPR_STRICT"
		"EEOP_QUAL"
};

bool
omrjit_compile_expr::buildIL()
   {
	OMR::JitBuilder::BytecodeBuilder *DONE_builder = OrphanBytecodeBuilder(0, "COMPLETE");

	OMR::JitBuilder::IlValue **operandStack = (OMR::JitBuilder::IlValue **) malloc(MAX * sizeof(OMR::JitBuilder::IlValue *));
	int32_t operandIterator = -1;

	OMR::JitBuilder::IlValue *r_econtext  		= Load("econtext");
	OMR::JitBuilder::IlValue *r_scanslot  		= LoadIndirect("ExprContext", "ecxt_scantuple", r_econtext);
	OMR::JitBuilder::IlValue *r_innerslot  		= LoadIndirect("ExprContext", "ecxt_innertuple", r_econtext);
	OMR::JitBuilder::IlValue *r_outerslot  		= LoadIndirect("ExprContext", "ecxt_outertuple", r_econtext);
	OMR::JitBuilder::IlValue *r_tts_scanvalues  = LoadIndirect("TupleTableSlot", "tts_values", r_scanslot);
	OMR::JitBuilder::IlValue *r_tts_scanisnull  = LoadIndirect("TupleTableSlot", "tts_isnull", r_scanslot);

	ExprEvalStep *op;
	OMR::JitBuilder::IlValue *r_op = NULL;

	OMR::JitBuilder::IlValue *r_resvalue = NULL;
	OMR::JitBuilder::IlValue *r_resnull = NULL;

	OMR::JitBuilder::IlValue *op_resvalue = NULL;
	OMR::JitBuilder::IlValue *op_resnull = NULL;

	Store("r_val", ConstInt32(0));
	Store("r_null", ConstInt32(0));

	OMR::JitBuilder::BytecodeBuilder **builders = (OMR::JitBuilder::BytecodeBuilder **) malloc((state->steps_len-1) * sizeof(OMR::JitBuilder::BytecodeBuilder *));
	int32_t opno;

	//initialize the builder objects
	for (opno = 0; opno < state->steps_len; opno++)
	{
		builders[opno] = OrphanBytecodeBuilder(opno, (char *)opcode_names[opno]);
		elog(INFO, "opno in print: %d done\n", opno);
	}

	OMR::JitBuilder::VirtualMachineState *vmState = new OMR::JitBuilder::VirtualMachineState();
	setVMState(vmState);
	AllLocalsHaveBeenDefined();
	AppendBytecodeBuilder(builders[0]);
	opno = GetNextBytecodeFromWorklist();
	Call("print_func", 2, ConstInt32(opno), ConstInt32(6666));

	while (opno != -1) {
		elog(INFO, "opno in while: %d done\n", opno);
		OMR::JitBuilder::BytecodeBuilder *b = builders[opno];

		op = &state->steps[opno];
		b->Store("r_op", b->Add( b->IndexAt(pExprEvalStep, b->StructFieldInstanceAddress("ExprState", "steps", b->Load("state")), b->ConstInt32(opno)), b->ConstInt32(1)));
		ExprEvalOp opcode = ExecEvalStepOp(state, op);

		switch(opcode)
		{
			case EEOP_DONE:
			{
				b->StoreAt(b->Load("isnull"), b->LoadIndirect("ExprState", "resnull", b->Load("state")));
				b->AddFallThroughBuilder(DONE_builder);
				break;
			}
			case EEOP_OMRJITCOMPILE_EXPR:
			{
				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_SCAN_FETCHSOME:
			{
				//elog(INFO, "in EEOP_SCAN_FETCHSOME-       after-------");
				b->Call("slot_getsomeattrs_int_func", 2, r_scanslot, b->ConstInt32(op->d.fetch.last_var));

				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_SCAN_VAR:
			{
				//elog(INFO, "in EEOP_SCAN_VAR-       after-------");
				OMR::JitBuilder::IlValue *r_values = b->IndexAt(pDatum, r_tts_scanvalues, b->ConstInt32(op->d.var.attnum));
				OMR::JitBuilder::IlValue *r_nulls  = b->IndexAt(pbool,  r_tts_scanisnull, b->ConstInt32(op->d.var.attnum));

				r_resvalue = b->LoadAt(pDatum, r_values);
				r_resnull  = b->LoadAt(pbool,  r_nulls);

				b->StoreIndirect("ExprEvalStep", "resvalue", b->Load("r_op"), r_values);
				b->StoreIndirect("ExprEvalStep", "resnull",  b->Load("r_op"),  r_nulls);
				op_resvalue = b->LoadAt(pDatum, b->LoadIndirect("ExprEvalStep", "resvalue", b->Load("r_op")));
				op_resnull  = b->LoadAt(pDatum, b->LoadIndirect("ExprEvalStep", "resnull",  b->Load("r_op")));

				operandIterator++;
				operandStack[operandIterator] = b->LoadAt(pDatum, b->LoadIndirect("ExprEvalStep", "resvalue", b->Load("r_op")));

				//b->Call("print_func", 2, b->ConvertTo(Int32, operandStack[operandIterator]), b->ConstInt32(6666));
				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_FUNCEXPR_STRICT:
			{
				FunctionCallInfo fcinfo = op->d.func.fcinfo_data;

				if(fcinfo->args[1].value != 0)
				{
					operandIterator++;
					operandStack[operandIterator] = b->ConstAddress((void *)fcinfo->args[1].value);
				}

				OMR::JitBuilder::IlValue *op_top = operandStack[operandIterator];
				operandIterator--;
				OMR::JitBuilder::IlValue *op_2nd_top = operandStack[operandIterator];
				operandIterator--;
				b->Call("print_func", 2, b->ConstInt32(6666), b->ConstInt32(6666));
				b->Call("print_func", 2, b->ConvertTo(Int32, op_top), b->ConvertTo(Int32, op_2nd_top));
				b->Call("print_func", 2, b->ConstInt32(6666), b->ConstInt32(6666));

				switch(op->operatorId)
				{
					//date_gt
					case 1089:
					{
						operandIterator++;

						IlBuilder *date_ge = NULL;
						IlBuilder *date_ge_else = NULL;

						b->Store("operand_temp", op_2nd_top);

						b->IfThenElse(&date_ge, &date_ge_else,
						b->	GreaterThan(op_2nd_top, op_top));

						date_ge->Store("operand_temp", date_ge->ConstInt64(1));
						date_ge_else->Store("operand_temp", date_ge_else->ConstInt64(0));

						operandStack[operandIterator] = b->Load("operand_temp");

						break;
					}

					//date_ge
					case 1090:
					{
						operandIterator++;

						IlBuilder *date_ge = NULL;
						IlBuilder *date_ge_else = NULL;

						b->Store("operand_temp", op_2nd_top);

						b->IfThenElse(&date_ge, &date_ge_else,
						b->	GreaterOrEqualTo(op_2nd_top, op_top));

						date_ge->Store("operand_temp", date_ge->ConstInt64(1));
						date_ge_else->Store("operand_temp", date_ge_else->ConstInt64(0));

						operandStack[operandIterator] = b->Load("operand_temp");

						break;
					}

					//date_lt
					case 1087:
					{
						operandIterator++;

						IlBuilder *date_le = NULL;
						IlBuilder *date_le_else = NULL;

						b->Store("operand_temp", op_2nd_top);

						b->IfThenElse(&date_le, &date_le_else,
						b->	LessThan(op_2nd_top, op_top));

						date_le->Store("operand_temp", date_le->ConstInt64(1));
						date_le_else->Store("operand_temp", date_le_else->ConstInt64(0));

						operandStack[operandIterator] = b->Load("operand_temp");

						break;
					}

					//date_le
					case 1088:
					{
						operandIterator++;

						IlBuilder *date_le = NULL;
						IlBuilder *date_le_else = NULL;

						b->Store("operand_temp", op_2nd_top);

						b->IfThenElse(&date_le, &date_le_else,
						b->	LessOrEqualTo(op_2nd_top, op_top));

						date_le->Store("operand_temp", date_le->ConstInt64(1));
						date_le_else->Store("operand_temp", date_le_else->ConstInt64(0));

						operandStack[operandIterator] = b->Load("operand_temp");

						break;
					}

					//float8gt
					case 297:
					{
						operandIterator++;

						IlBuilder *float8_ge = NULL;
						IlBuilder *float8_ge_else = NULL;

						b->Store("operand_temp", op_2nd_top);

						b->IfThenElse(&float8_ge, &float8_ge_else,
						b->	GreaterThan(op_2nd_top, op_top) );

						float8_ge->Store("operand_temp", float8_ge->ConstInt64(1));
						float8_ge_else->Store("operand_temp", float8_ge_else->ConstInt64(0));

						operandStack[operandIterator] = b->Load("operand_temp");

						break;
					}

					//float8ge
					case 298:
					{
						operandIterator++;

						IlBuilder *float8_ge = NULL;
						IlBuilder *float8_ge_else = NULL;

						b->Store("operand_temp", op_2nd_top);

						b->IfThenElse(&float8_ge, &float8_ge_else,
						b->	GreaterOrEqualTo(op_2nd_top, op_top) );

						float8_ge->Store("operand_temp", float8_ge->ConstInt64(1));
						float8_ge_else->Store("operand_temp", float8_ge_else->ConstInt64(0));

						operandStack[operandIterator] = b->Load("operand_temp");

						break;
					}

					//float8lt
					case 295:
					{
						operandIterator++;

						IlBuilder *float8_le = NULL;
						IlBuilder *float8_le_else = NULL;

						b->Store("operand_temp", op_2nd_top);

						b->IfThenElse(&float8_le, &float8_le_else,
						b->	LessThan(op_2nd_top, op_top) );

						float8_le->Store("operand_temp", float8_le->ConstInt64(1));
						float8_le_else->Store("operand_temp", float8_le_else->ConstInt64(0));

						operandStack[operandIterator] = b->Load("operand_temp");

						break;
					}

					//float8le
					case 296:
					{
						operandIterator++;

						IlBuilder *float8_le = NULL;
						IlBuilder *float8_le_else = NULL;

						b->Store("operand_temp", op_2nd_top);

						b->IfThenElse(&float8_le, &float8_le_else,
						b->	LessOrEqualTo(op_2nd_top, op_top) );

						float8_le->Store("operand_temp", float8_le->ConstInt64(1));
						float8_le_else->Store("operand_temp", float8_le_else->ConstInt64(0));

						operandStack[operandIterator] = b->Load("operand_temp");

						break;
					}
					//int4lt
					case 66:
					{

						IlBuilder *int4lt_true = NULL;
						IlBuilder *int4lt_false = NULL;

						operandIterator++;
						//b->Call("print_func", 2, b->ConstInt32(222222), b->ConstInt32(222222));
						b->Store("operand_temp_lt", op_2nd_top);
						b->IfThenElse(&int4lt_true, &int4lt_false,
						b->	LessThan(op_2nd_top, op_top));

						int4lt_true->Store("operand_temp_lt", int4lt_true->ConstInt64(1));
						int4lt_false->Store("operand_temp_lt", int4lt_false->ConstInt64(0));

						operandStack[operandIterator] = b->Load("operand_temp_lt");

						//Call("print_func", 2, ConvertTo(Int32, operandStack[operandIterator]), ConstInt32(operandIterator));

						break;
					}
					//int4le
					case 149:
					{

						IlBuilder *int4lt_true = NULL;
						IlBuilder *int4lt_false = NULL;

						operandIterator++;
						//Call("print_func", 2, ConstInt32(100001), ConstInt32(100001));
						b->Store("operand_temp", op_2nd_top);
						b->IfThenElse(&int4lt_true, &int4lt_false,
						b->	LessOrEqualTo(op_2nd_top, op_top));

						int4lt_true->Store("operand_temp", int4lt_true->ConstInt64(1));
						int4lt_false->Store("operand_temp", int4lt_false->ConstInt64(0));

						operandStack[operandIterator] = b->Load("operand_temp");

						//Store("r_val", ConvertTo(Int32, operandStack[operandIterator]));

						//Call("print_func", 2, ConvertTo(Int32, operandStack[operandIterator]), ConstInt32(operandIterator));

						break;
					}

					//int4eq
					case 65:
					{

						IlBuilder *int4eq_true = NULL;
						IlBuilder *int4eq_false = NULL;

						operandIterator++;

						b->Store("operand_temp_ge", op_2nd_top);

						b->IfThenElse(&int4eq_true, &int4eq_false,
						b->	EqualTo(
						b->		ConvertTo(Int64, op_2nd_top),
						b->		ConvertTo(Int64, op_top)));

						int4eq_true->Store("operand_temp_ge", int4eq_true->ConstInt64(1));
						int4eq_false->Store("operand_temp_ge", int4eq_false->ConstInt64(0));

						operandStack[operandIterator] = b->Load("operand_temp_ge");

						break;
					}

					//int4ge
					case 150:
					{

						IlBuilder *int4gt_true = NULL;
						IlBuilder *int4gt_false = NULL;

						operandIterator++;

						b->Store("operand_temp_ge", op_2nd_top);

						b->IfThenElse(&int4gt_true, &int4gt_false,
						b->	GreaterOrEqualTo(op_2nd_top, op_top));

						int4gt_true->Store("operand_temp_ge", int4gt_true->ConstInt64(1));
						int4gt_false->Store("operand_temp_ge", int4gt_false->ConstInt64(0));

						operandStack[operandIterator] = b->Load("operand_temp_ge");

						break;
					}

					//int4gt
					case 147:
					{

						IlBuilder *int4gt_true = NULL;
						IlBuilder *int4gt_false = NULL;

						operandIterator++;

						b->Store("operand_temp_gt", op_2nd_top);

						b->IfThenElse(&int4gt_true, &int4gt_false,
						b->	GreaterThan(op_2nd_top, op_top));

						int4gt_true->Store("operand_temp_gt", int4gt_true->ConstInt64(1));
						int4gt_false->Store("operand_temp_gt", int4gt_false->ConstInt64(0));

						operandStack[operandIterator] = b->Load("operand_temp_gt");

						break;
					}

					//int4mul 141
					case 141:
					{
						operandIterator++;
						b->Store("operand_temp_gt", b->Mul(b->ConvertTo(Int64, op_2nd_top), b->ConvertTo(Int64, op_top)));
						operandStack[operandIterator] = b->Load("operand_temp_gt");

						break;
					}

					//int4pl 177
					case 177:
					{

						operandIterator++;
						b->Store("operand_temp_gt", b->Add(b->ConvertTo(Int64, op_2nd_top), b->ConvertTo(Int64, op_top)));

						operandStack[operandIterator] = b->Load("operand_temp_gt");

						break;
					}

					//int4mi 181
					case 181:
					{
						operandIterator++;
						b->Store("operand_temp_gt", b->Sub(b->ConvertTo(Int64, op_2nd_top), b->ConvertTo(Int64, op_top)));
						operandStack[operandIterator] = b->Load("operand_temp_gt");

						break;
					}

					//int4div 154
					case 154:
					{
						operandIterator++;
						b->Store("operand_temp_gt", b->Div(b->ConvertTo(Int64, op_2nd_top), b->ConvertTo(Int64, op_top)));
						operandStack[operandIterator] = b->Load("operand_temp_gt");

						break;
					}

					//float8pl 218
					case 218:
					{
						break;
					}

				}
				b->AddFallThroughBuilder(builders[opno+1]);
				break;
			}
			case EEOP_QUAL:
			{
				IlBuilder *stackcheck_true  = NULL;
				IlBuilder *stackcheck_false = NULL;

				b->IfThenElse(&stackcheck_true, &stackcheck_false, b->EqualTo(operandStack[operandIterator], b->ConstInt64(1)));
				stackcheck_true->Store("r_val", stackcheck_true->ConstInt32(1));
				stackcheck_true->Store("r_null", stackcheck_true->ConstInt32(0));

				stackcheck_false->Store("r_val", stackcheck_false->ConstInt32(0));
				stackcheck_false->Store("r_null", stackcheck_false->ConstInt32(1));

				IlBuilder *compare_intlt = NULL;
				IlBuilder *compare_intlt_else = NULL;

				b->IfThenElse(&compare_intlt, &compare_intlt_else, b->EqualTo(b->Load("r_val"), b->ConstInt32(1)  ));

				r_resvalue = compare_intlt->ConstAddress((void *)1);
				r_resnull  = compare_intlt->ConstInt16(0);

				compare_intlt->StoreIndirect("ExprState", "resvalue", compare_intlt->Load("state"), compare_intlt->ConstInt64(1));
				compare_intlt->StoreIndirect("ExprState", "resnull",  compare_intlt->Load("state"), compare_intlt->ConstInt8(0));


				r_resvalue = compare_intlt_else->ConstAddress((void *)0);
				r_resnull  = compare_intlt_else->ConstInt16(1);

				compare_intlt_else->StoreIndirect("ExprState", "resvalue", compare_intlt_else->Load("state"), compare_intlt_else->ConstInt64(0));
				//b->Call("print_func",2, b->Load("r_val"), b->Load("r_null"));

				b->IfCmpEqual(builders[op->d.qualexpr.jumpdone], b->Load("r_val"), b->ConstInt32(0) );

				b->AddFallThroughBuilder(builders[opno+1]);
				break;

			}
		}

		opno = GetNextBytecodeFromWorklist();
	}
	free(builders);

    op_omr = op;
    free(operandStack);
    AppendBytecodeBuilder(DONE_builder);
	Return(LoadIndirect("ExprState", "resvalue", Load("state")));
	return true;
   }

/*
 *
 * end
 *
 * */

extern "C" {
PG_MODULE_MAGIC;

/*function to compile expression Jit code*/
OMRJIT_slot_deformFunctionType * omr_compile(ExprState *state,
						TupleTableSlot *resultslot, TupleTableSlot *innerslot, TupleTableSlot *outerslot, TupleTableSlot *scanslot)
{

	StructTypeDictionary joinmethodTypes;

	omrjit_compile_expr method(&joinmethodTypes, state, resultslot, innerslot, outerslot, scanslot);
	void *entry=0;
	int32_t rc = compileMethodBuilder(&method, &entry);

	if (rc != 0)
	{

	   elog(INFO, "FAIL: compilation error %d\n", rc);
	   exit(-2);
	}

	return (OMRJIT_slot_deformFunctionType *)entry;

}

/*function to compile tuple deformation Jit code*/
OMRJIT_slot_deformFunctionType_att * omr_compile_att(int32_t natts, TupleDesc tupleDesc, TupleTableSlot *slot){

	StructTypeDictionary joinmethodTypes;

	slot_compile_deform method(&joinmethodTypes, natts, tupleDesc, slot);
	void *entry=0;
	int32_t rc = compileMethodBuilder(&method, &entry);

	if (rc != 0)
	{

		elog(INFO, "FAIL: compilation error %d\n", rc);
	   exit(-2);
	}
	return (OMRJIT_slot_deformFunctionType_att *)entry;

}


}














