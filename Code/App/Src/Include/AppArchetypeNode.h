#ifndef FRONTIERX_APP_ARCHETYPE_NODE_H
#define FRONTIERX_APP_ARCHETYPE_NODE_H
// Original private node from ObjList.cpp; fields and lifetime are unchanged.
// Copyright (C) 1997 by Digital Anvil, Inc.
#include "../ObjList.h"
#include <TSmartPointer.h>
#include <cstring>

struct ARCHDATATYPE;

struct ARCHNODE
{
	struct ARCHNODE * prev, * next;
	U32 usage;
	ARCHDATATYPE * archDataType;
	COMPTR<IObjectFactory> factory;
	HANDLE hArchetype;

#ifndef FINAL_RELEASE

	struct NAMENODE
	{
		NAMENODE * pNext;
		const char * szName;
	} * nameList;

	~ARCHNODE (void)
	{
		reset();
	}

	void reset (void)		// flush the names list
	{
		NAMENODE * node = nameList;

		while (node)
		{
			nameList = node->pNext;
			delete node;
			node = nameList;
		}
	}

	void addName (const char * name)
	{
		if (name)
		{
			NAMENODE * node = new NAMENODE;
			node->pNext = nameList;
			nameList = node;
			node->szName = name;
		}
	}

	void removeName (const char * name)
	{
		if (name)
		{
			NAMENODE * node = nameList, *prev=0;

			while (node)
			{
	//			if (strcmp(name, node->szName) == 0)
				if (name == node->szName)
				{
					if (prev)
						prev->pNext = node->pNext;
					else
						nameList = node->pNext;
					delete node;
					break;
				}

				prev = node;
				node = node->pNext;
			}
		}
	}

#endif

	ARCHNODE (void)
	{
		memset(this, 0, sizeof(*this));
	}

private:
	// not allowed
	ARCHNODE & operator = (const ARCHNODE & obj)
	{
		return *this;
	}
};
#endif
