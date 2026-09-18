#ifndef TCONNPOINT_H
#define TCONNPOINT_H
//--------------------------------------------------------------------------//
//                                                                          //
//                                TConnPoint.h                              //
//                                                                          //
//                  COPYRIGHT (C) 1997 BY DIGITAL ANVIL, INC.               //
//                                                                          //
//--------------------------------------------------------------------------//
/*

    $Author: Rmarr $
*/			    
//--------------------------------------------------------------------------//

#ifndef ICONNECTION_H
#include "IConnection.h"
#endif

#ifndef TCOMPONENT_H
#include "TComponent.h"
#endif


//--------------------------------------------------------------------------
//  
template <class Type=IDAComponent> 
struct CONNECTION_NODE
{
	CONNECTION_NODE<Type> *pNext;
	Type *client;
	FrontierX::ConnectionToken connectionToken;
};

template <class Base, class Type=IDAComponent> 
struct ConnectionPoint : public IDAConnectionPoint
{
	CONNECTION_NODE<Type> *pClientList;
	int index;

	ConnectionPoint (int i);

	~ConnectionPoint (void);
	
	/* IDAComponent members */

	DEFMETHOD(QueryInterface) (const C8 *interface_name, void **instance);

	DEFMETHOD_(U32,AddRef) (void);

	DEFMETHOD_(U32,Release) (void);

	/* IDAConnectionPoint members */

	DEFMETHOD_(U32,GetOutgoingInterface) (C8 *interfaceName, U32 bufferLength);

	DEFMETHOD(GetContainer) (IDAConnectionPointContainer **container);

	DEFMETHOD(Advise) (IDAComponent *component, U32 *handle);

	DEFMETHOD(Unadvise) (U32 handle);

	DEFMETHOD_(BOOL32,EnumerateConnections) (CONNECTION_ENUM_PROC proc, void *context=0);

	/* ConnectionPoint members */

};

//--------------------------------------------------------------------------//
//
template <class Base, class Type>
GENRESULT ConnectionPoint<Base,Type>::QueryInterface (const C8 *interface_name, void **instance)
{
	(void) interface_name;
	*instance = 0;
	return GR_INTERFACE_UNSUPPORTED;
}
//--------------------------------------------------------------------------//
//
template <class Base, class Type>
U32 ConnectionPoint<Base,Type>::AddRef (void)
{
	return 1;
}
//--------------------------------------------------------------------------//
//
template <class Base, class Type>
U32 ConnectionPoint<Base,Type>::Release (void)
{
	return 1;
}
//--------------------------------------------------------------------------//
//
template <class Base, class Type>
U32 ConnectionPoint< Base,Type >::GetOutgoingInterface (C8 *interfaceName, U32 bufferLength)
{
	const _DACOM_INTMAP_ENTRY *array = Base::_GetEntriesOut();
	U32 len = strlen(array[index].interface_name) + 1;
	
	if (len > bufferLength)
		len = bufferLength;

	memcpy(interfaceName, array[index].interface_name, len);

	return (len)?len-1:0;
}
//--------------------------------------------------------------------------//
//
template <class Base, class Type>
GENRESULT ConnectionPoint< Base,Type >::GetContainer (IDAConnectionPointContainer **container)
{
	const _DACOM_INTMAP_ENTRY *array = Base::_GetEntriesOut();

	Base *owner = FrontierX::OwnerFromMember<Base>(this, array[index].offset);
	*container = static_cast<IDAConnectionPointContainer *>(owner);

	(*container)->AddRef();			

	return GR_OK;
}
//--------------------------------------------------------------------------//
//
template <class Base, class Type>
GENRESULT ConnectionPoint< Base,Type >::Advise (IDAComponent *component, U32 *handle)
{
	CONNECTION_NODE<Type> *pList = pClientList, *pPrev=0;
	Type *client;
	const _DACOM_INTMAP_ENTRY *array = Base::_GetEntriesOut();
	const FrontierX::ConnectionToken connectionToken =
		FrontierX::NextConnectionToken(pClientList);
	if (connectionToken == 0)
		return GR_OUT_OF_SPACE;
	// check to make sure we have the right interface pointer

	if (component->QueryInterface(array[index].interface_name, (void **) &client) != GR_OK)
		return GR_GENERIC;
	client->Release();		// release the reference early

	while (pList)
	{
		pPrev = pList;
		pList = pList->pNext;
	}

	if (pPrev)
	{
		if ((pPrev->pNext = new CONNECTION_NODE<Type>) != 0)
			pPrev = pPrev->pNext;
	}
	else
	if ((pPrev = new CONNECTION_NODE<Type>) != 0)
		pClientList = pPrev;

	if (pPrev)
	{
		pPrev->pNext=0;
		pPrev->client = client;
		pPrev->connectionToken = connectionToken;
		*handle = connectionToken;
		return GR_OK;
	}

	return GR_OUT_OF_MEMORY;
}
//--------------------------------------------------------------------------//
//
template <class Base, class Type>
GENRESULT ConnectionPoint< Base,Type >::Unadvise (U32 handle)
{
	CONNECTION_NODE<Type> *pList = pClientList, *pPrev=0;


	while (pList)
	{
		if (pList->connectionToken == handle)
		{
			if (pPrev)
				pPrev->pNext = pList->pNext;
			else
				pClientList = pList->pNext;
			delete pList;
			return GR_OK;
		}
		pPrev = pList;
		pList = pList->pNext;
	}

	return GR_GENERIC;
}
//--------------------------------------------------------------------------//
//
template <class Base, class Type>
BOOL32 ConnectionPoint< Base,Type >::EnumerateConnections (CONNECTION_ENUM_PROC proc, void *context)
{
	BOOL32 result;
	CONNECTION_NODE<Type> *pList;

	result = ((pList = pClientList) != 0);

	while (result && pList)
	{
		result = proc(this, pList->client, context);
		pList = pList->pNext;
	}

	return result;
}
//--------------------------------------------------------------------------//
//
template <class Base, class Type>
ConnectionPoint< Base,Type >::ConnectionPoint (int i)
{
	pClientList=0;
	index = i;
}
//--------------------------------------------------------------------------//
//
template <class Base, class Type>
ConnectionPoint< Base,Type >::~ConnectionPoint (void)
{
	CONNECTION_NODE<Type> *pNode;

	while (pClientList)
	{
		pNode = pClientList->pNext;
		delete pClientList;
		pClientList = pNode;
	}
}
//--------------------------------------------------------------------------//
//----------------------------End TConnPoint.h------------------------------//
//--------------------------------------------------------------------------//
#endif
