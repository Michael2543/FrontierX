#ifndef TCOMPONENT_H
#define TCOMPONENT_H
//--------------------------------------------------------------------------//
//                                                                          //
//                              TComponent.h                                //
//                                                                          //
//               COPYRIGHT (C) 1997 BY DIGITAL ANVIL, INC.                  //
//                                                                          //
//--------------------------------------------------------------------------//
/*
   $Author: Rmarr $

                       Template class for Base DA Components

*/
//--------------------------------------------------------------------------//


#ifndef DACOM_H
#include "DACOM.H"
#endif

#include "FrontierX/DacomRuntime.h"

#include <cstddef>
#include <cstring>

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4355 )		// 'this' used during construction
#define DACOM_NOINLINE __declspec(noinline)
#else
#define DACOM_NOINLINE
#endif

#if defined(GENERAL_TRACE_1) && defined(TEMPSTR)
#define DACOM_TRACE_REFERENCE(format, value) GENERAL_TRACE_1(TEMPSTR(format, value))
#else
#define DACOM_TRACE_REFERENCE(format, value) ((void)0)
#endif

//----------------------------------//

#define daoffsetofclass(base, derived) FrontierX::BaseSubobjectOffset<base, derived>()
#define daoffsetofmember(base, member) FrontierX::MemberOffset(&base::member)
#define dasizeofmember(base,member) sizeof(((base *)0)->member)


struct _DACOM_INTMAP_ENTRY
{
	const C8 *interface_name;
	FrontierX::DacomRuntimeOffset offset;
};

// DACOM_MAP macros use a lazy-fill pattern to work around MSVC 2022 ICE
// (toinil.c:899) that fires when a static local array with non-constexpr
// initializers appears inside a template member function.
// Fix: use a zero-initialized static array (constexpr, no toinil path) and
// fill entries via assignment on the first call, when the class is complete.
#define BEGIN_DACOM_MAP_INBOUND(x) public: \
	DACOM_NOINLINE static const _DACOM_INTMAP_ENTRY* COMAPI _GetEntriesIn() { \
	typedef x _DaComMapClass; \
	static bool _inited = false; \
	static _DACOM_INTMAP_ENTRY _entries[32] = {}; \
	if (!_inited) { _inited = true; int _idx = 0;

#define BEGIN_DACOM_MAP_OUTBOUND(x) public: \
	DACOM_NOINLINE static const _DACOM_INTMAP_ENTRY* COMAPI _GetEntriesOut() { \
	typedef x _DaComMapClass; \
	static bool _inited = false; \
	static _DACOM_INTMAP_ENTRY _entries[32] = {}; \
	if (!_inited) { _inited = true; int _idx = 0;

#define DACOM_INTERFACE_ENTRY(x)\
	_entries[_idx].interface_name = #x; \
	_entries[_idx++].offset = daoffsetofclass(x, _DaComMapClass);

#define DACOM_INTERFACE_ENTRY2(x,y)\
	_entries[_idx].interface_name = (x); \
	_entries[_idx++].offset = daoffsetofclass(y, _DaComMapClass);

#define DACOM_INTERFACE_ENTRY_REF(x,y)\
	_entries[_idx].interface_name = (x); \
	_entries[_idx++].offset = daoffsetofmember(_DaComMapClass, y) | FrontierX::DacomReferenceFlag;

#define DACOM_INTERFACE_ENTRY_AGGREGATE(x,y)\
	_entries[_idx].interface_name = (x); \
	_entries[_idx++].offset = daoffsetofmember(_DaComMapClass, y);

#define END_DACOM_MAP() \
	} return _entries; }




//----------------------------------//

template <class Base> struct DAComponent : public Base
{
	U32 ref_count;

	
	DAComponent (void)
	{
		ref_count=1;
	}

	/* IDAComponent methods */

	DEFMETHOD(QueryInterface) (const C8 *interface_name, void **instance);
	DEFMETHOD_(U32,AddRef)           (void);
	DEFMETHOD_(U32,Release)          (void);
};

//--------------------------------------------------------------------------//
//
template <class Base>
GENRESULT DAComponent< Base >::QueryInterface (const C8 *interface_name, void **instance)
{
	int i;
	const _DACOM_INTMAP_ENTRY * interfaces = Base::_GetEntriesIn();

	for (i = 0; interfaces[i].interface_name; i++)
	{
		if (std::strcmp(interfaces[i].interface_name, interface_name) == 0)
		{
			IDAComponent *result;
			
			if (interfaces[i].offset & FrontierX::DacomReferenceFlag)
				result = *((IDAComponent **) (((char *) this) +
					(interfaces[i].offset & ~FrontierX::DacomReferenceFlag)));
			else
				result = (IDAComponent *) (((char *) this) + interfaces[i].offset);

			result->AddRef();
			*instance = result;
			return GR_OK;
		}
	}

	*instance = 0;
	return GR_INTERFACE_UNSUPPORTED;
}
//--------------------------------------------------------------------------//
//
template <class Base>
U32 DAComponent< Base >::AddRef (void)
{
	ref_count++;
	return ref_count;
}
//--------------------------------------------------------------------------//
//
template <class Base>
U32 DAComponent< Base >::Release (void)
{
	if (ref_count > 0)
		ref_count--;

	if (ref_count == 0)
	{
		ref_count++;		// artificially add reference to prevent infinite loops
		delete this;
		return 0;
	}

	return ref_count;
}

//

//--------------------------------------------------------------------------//
// Debug version of DAComponent
//
template <class Base> 
struct DADebugComponent : public DAComponent<Base>
{
	DEFMETHOD_(U32,AddRef)(void)
	{
		U32 ret = DAComponent<Base>::AddRef();

		DACOM_TRACE_REFERENCE("AddRef %d\n", ret);

		return ret;
	}

	//

	DEFMETHOD_(U32,Release)(void)
	{
		U32 ret = DAComponent<Base>::Release();

		DACOM_TRACE_REFERENCE("RelRef %d\n", ret);

		return ret;
	}

	//
};

// 


//--------------------------------------------------------------------------//
//----------------Inner Component Implementation (for Aggregation)----------//
//--------------------------------------------------------------------------//
//

template <class Type, class Base=IDAComponent> 
struct DAComponentInner : public Base
{
	U32 ref_count;
	Type * owner;
	
	DAComponentInner (Type * _owner)
	{
		ref_count=1;
		owner = _owner;
	}

	/* IDAComponent methods */

	DEFMETHOD(QueryInterface) (const C8 *interface_name, void **instance);
	DEFMETHOD_(U32,AddRef)           (void);
	DEFMETHOD_(U32,Release)          (void);
};

//--------------------------------------------------------------------------//
//
template <class Type, class Base>
GENRESULT DAComponentInner< Type, Base >::QueryInterface (const C8 *interface_name, void **instance)
{
	int i;
	const _DACOM_INTMAP_ENTRY * interfaces = owner->_GetEntriesIn();

	for (i = 0; interfaces[i].interface_name; i++)
	{
		if (std::strcmp(interfaces[i].interface_name, interface_name) == 0)
		{
			IDAComponent *result;

			if (interfaces[i].offset & FrontierX::DacomReferenceFlag)
				result = *((IDAComponent **) (((char *) owner) +
					(interfaces[i].offset & ~FrontierX::DacomReferenceFlag)));
			else
				result = (IDAComponent *) (((char *) owner) + interfaces[i].offset);

			result->AddRef();
			*instance = result;
			return GR_OK;
		}
	}

	*instance = 0;
	return GR_INTERFACE_UNSUPPORTED;
}
//--------------------------------------------------------------------------//
//
template <class Type, class Base>
U32 DAComponentInner< Type, Base >::AddRef (void)
{
	ref_count++;
	return ref_count;
}
//--------------------------------------------------------------------------//
//
template <class Type, class Base>
U32 DAComponentInner< Type, Base >::Release (void)
{
	if (ref_count > 0)
		ref_count--;

	if (ref_count == 0)
	{
		ref_count++;		// artificially add reference to prevent infinite loops
		delete owner;
		return 0;
	}

	return ref_count;
}

//--------------------------------------------------------------------------//
//--------------------------Aggregate implementation------------------------//
//--------------------------------------------------------------------------//
//

template <class Base> 
struct DAComponentAggregate : public Base
{
	IDAComponent *outerComponent;
	DAComponentInner<Base> innerComponent;

	
	DAComponentAggregate (struct AGGDESC * desc) : innerComponent(this)
	{
		if ((outerComponent = desc->outer) == 0)
			outerComponent = &innerComponent;
		else
			*(desc->inner) = &innerComponent;
	}

	/* IDAComponent methods */

	DEFMETHOD(QueryInterface) (const C8 *interface_name, void **instance);
	DEFMETHOD_(U32,AddRef)           (void);
	DEFMETHOD_(U32,Release)          (void);
};

//--------------------------------------------------------------------------//
//
template <class Base>
GENRESULT DAComponentAggregate< Base >::QueryInterface (const C8 *interface_name, void **instance)
{
	return outerComponent->QueryInterface(interface_name, instance);
}
//--------------------------------------------------------------------------//
//
template <class Base>
U32 DAComponentAggregate< Base >::AddRef (void)
{
	return outerComponent->AddRef();
}
//--------------------------------------------------------------------------//
//
template <class Base>
U32 DAComponentAggregate< Base >::Release (void)
{
	return outerComponent->Release();
}

//--------------------------------------------------------------------------//
//-------------------Component Factory Implementation-----------------------//
//--------------------------------------------------------------------------//
//
template <class ClassType, class DescType>
struct DACOM_NO_VTABLE DAComponentFactoryBase : public IComponentFactory
{
	U32 ref_count;
	const char * className;

	DAComponentFactoryBase (const char * _className)
	{
		ref_count=1;
		if ((className = _className) == 0)
			className = ClassType::_GetEntriesIn()->interface_name;
	}

	/* IDAComponent methods */

	DEFMETHOD(QueryInterface) (const C8 *interface_name, void **instance);
	DEFMETHOD_(U32,AddRef)           (void);
	DEFMETHOD_(U32,Release)          (void);
};
//--------------------------------------------------------------------------//
//
template <class ClassType, class DescType>
GENRESULT DAComponentFactoryBase<ClassType,DescType>::QueryInterface (const C8 *interface_name, void **instance)
{
	*instance = 0;
	if (std::strcmp(interface_name, "IComponentFactory") != 0)
		return GR_INTERFACE_UNSUPPORTED;

	*instance = this;
	AddRef();
	return GR_OK;
}
//--------------------------------------------------------------------------//
//
template <class ClassType, class DescType>
U32 DAComponentFactoryBase<ClassType,DescType>::AddRef (void)
{
	ref_count++;
	return ref_count;
}
//--------------------------------------------------------------------------//
//
template <class ClassType, class DescType>
U32 DAComponentFactoryBase<ClassType,DescType>::Release (void)
{
	if (ref_count > 0)
		ref_count--;
	if (ref_count == 0)
	{
		ref_count++;		// artificially add reference to prevent infinite loops
		delete this;
		return 0;
	}
	return ref_count;
}
//--------------------------------------------------------------------------//
//
template <class ClassType, class DescType>
struct DAComponentFactory : public DAComponentFactoryBase<ClassType,DescType>
{
	DAComponentFactory (const char * _className) : DAComponentFactoryBase<ClassType,DescType>(_className)
	{
	}

	/* IComponentFactory methods */

	DEFMETHOD(CreateInstance) (DACOMDESC *descriptor, void **instance);
};
//--------------------------------------------------------------------------//
//
template <class ClassType, class DescType>
GENRESULT DAComponentFactory<ClassType,DescType>::CreateInstance (DACOMDESC *descriptor, void **instance)
{
	GENRESULT result = GR_OK;
	ClassType *pNewInstance = 0;
	DescType *lpDesc = (DescType *) descriptor;

	//
	// If unsupported interface requested, fail call
	//

	if ((lpDesc->size != sizeof(*lpDesc)) || std::strcmp(lpDesc->interface_name, this->className))
	{
		result = GR_INTERFACE_UNSUPPORTED;
		goto Done;
	}

	//
	// Create an instance of ClassType
	//

	if ((pNewInstance = new ClassType) == 0)
	{
		result = GR_OUT_OF_MEMORY;
		goto Done;
	}

	if ((result = pNewInstance->init(lpDesc)) != GR_OK)
	{
		// 
		// initialization failed!
		//
		delete pNewInstance;
		pNewInstance = 0;
	}
Done:
	*instance = pNewInstance;
	return result;
}
//--------------------------------------------------------------------------//
//-------------------Component Factory Implementation 2---------------------//
//--------------------------------------------------------------------------//
//
template <class ClassType, class DescType>
struct DAComponentFactory2 : public DAComponentFactoryBase<ClassType,DescType>
{
	DAComponentFactory2 (const char * _className) : DAComponentFactoryBase<ClassType,DescType>(_className)
	{
	}

	
	/* IComponentFactory methods */

	DEFMETHOD(CreateInstance) (DACOMDESC *descriptor, void **instance);
};
//--------------------------------------------------------------------------//
//
template <class ClassType, class DescType>
GENRESULT DAComponentFactory2<ClassType,DescType>::CreateInstance (DACOMDESC *descriptor, void **instance)
{
	GENRESULT result = GR_OK;
	ClassType *pNewInstance = 0;
	DescType *lpDesc = (DescType *) descriptor;

	//
	// If unsupported interface requested, fail call
	//

	if ((lpDesc->size != sizeof(*lpDesc)) || std::strcmp(lpDesc->interface_name, this->className))
	{
		result = GR_INTERFACE_UNSUPPORTED;
		goto Done;
	}

	//
	// Create an instance of ClassType
	//

	if ((pNewInstance = new ClassType(lpDesc)) == 0)
	{
		result = GR_OUT_OF_MEMORY;
		goto Done;
	}

	if ((result = pNewInstance->init(lpDesc)) != GR_OK)
	{
		if (lpDesc->inner)
			*(lpDesc->inner) = 0;		// reset it to NULL
		// 
		// initialization failed!
		//
		delete pNewInstance;
		pNewInstance = 0;
	}
Done:
	*instance = pNewInstance;
	return result;
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#undef DACOM_TRACE_REFERENCE

//--------------------------------------------------------------------------//
//-------------------------End TComponent.h---------------------------------//
//--------------------------------------------------------------------------//
#endif
