//fleet group members
struct FleetMember
{
	FleetMember * next;
	U32 partID;
};

struct FleetGroup
{
	FleetGroup * next;
	FleetGroupDef * pData;
	FleetMember * memberList;
	U32 groupID;//the id of the group in the BT_FORMATION structure
	U32 placementID;
	SINGLE radius;

	S32 postX;
	S32 postY;
	bool bPostValid:1;

	struct AI
	{
		bool bPostMoved;
		OBJPTR<IBaseObject> targets[NUM_TARGETS];
		NETGRIDVECTOR lastPos;
		NETGRIDVECTOR rovingPost;
		bool bRoving:1;
	}ai;

	FleetGroup()
	{
		next = NULL;
		memberList = NULL;
		bPostValid= false;
		ai.bPostMoved = false;
		ai.targets[0] = NULL;
		ai.targets[1] = NULL;
		ai.bRoving = false;
	};

	void AddMember(U32 member);
};


FleetMember * freeFleetMemberList = NULL;
FleetGroup * freeFleetGroupList = NULL;

FleetMember * getNewFleetMember()
{
	if(freeFleetMemberList)
	{
		FleetMember * ret = freeFleetMemberList;
		freeFleetMemberList = ret->next;
		ret->next = NULL;
		return ret;
	}
	return new FleetMember;
}

void releaseFleetMember(FleetMember * member)
{
	member->next = freeFleetMemberList;
	freeFleetMemberList = member;
}

void clearFleetMemberList()
{
	while(freeFleetMemberList)
	{
		FleetMember * tmp = freeFleetMemberList;
		freeFleetMemberList = freeFleetMemberList->next;
		delete tmp;
	}
}

FleetGroup * getNewFleetGroup()
{
	if(freeFleetGroupList)
	{
		FleetGroup * ret = freeFleetGroupList;
		freeFleetGroupList = ret->next;
		ret->next = NULL;
		ret->bPostValid = false;
		return ret;
	}
	return new FleetGroup;
}

//this will also realse any fleet members inside the group.
void releaseFleetGroup(FleetGroup * member)
{
	while(member->memberList)
	{
		FleetMember * tmp = member->memberList;
		member->memberList = member->memberList->next;
		releaseFleetMember(tmp);
	}
	member->ai.targets[0] = NULL;
	member->ai.targets[1] = NULL;
	member->ai.bRoving = false;
	member->next = freeFleetGroupList;
	freeFleetGroupList = member;
}

void clearFleetGroupList()
{
	while(freeFleetGroupList)
	{
		FleetGroup * tmp = freeFleetGroupList;
		freeFleetGroupList = freeFleetGroupList->next;
		delete tmp;
	}
}

void FleetGroup::AddMember(U32 member)
{
	FleetMember * newMem = getNewFleetMember();
	newMem->partID = member;
	newMem->next = memberList;
	memberList = newMem;
}

