#define WIN32_LEAN_AND_MEAN
#if defined(_WIN32)
#include <windows.h>
#endif
#include <stdio.h>
#include <cstring>

#include "STDDAT.H"
#include "DACOM.H"
#include "CQTrace.h"
#include "TempStr.h"

#include "BMPoolJr.h"
#include "RenderBatcher.h"

#define MAX_BUFFER_SIZE (1536*1024)//(1024*1024)
unsigned char TheBigBuffer[MAX_BUFFER_SIZE];
U32 TheBigPointer=0;

	void BMPOOLJR::set_size( U32 new_pool_size ) 
	{
	//	delete[] pool;
		old_used_cnt = pool_used_cnt = 0;
		pool_size = new_pool_size;
	//	pool = new unsigned char[pool_size];
		pool_flags |= BM_P_F_EMPTY;
		pool = 0;
	}

	bool BMPOOLJR::is_empty( )
	{
	//	return (pool_flags & BM_P_F_EMPTY)? true : false;
		return (pool_used_cnt == 0);
	}

	void BMPOOLJR::reallocate ()
	{
		if (TheBigPointer > MAX_BUFFER_SIZE ||
			pool_used_cnt > MAX_BUFFER_SIZE - TheBigPointer ||
			(pool_used_cnt && !pool))
		{
			CQASSERT(false);
			return;
		}
		U8 * new_pool = &TheBigBuffer[TheBigPointer];
		if (pool_used_cnt)
			memmove(new_pool,pool,pool_used_cnt);
		pool = new_pool;
		TheBigPointer += pool_used_cnt;
	}


	U32 BMPOOLJR::allocate( U32 size, bool set_non_empty)
	{
		if (size == 0)
		{
			pool_size = 0;
			return 0;
		}

		if (pool_used_cnt > MAX_BUFFER_SIZE || size > MAX_BUFFER_SIZE - pool_used_cnt ||
			(pool_used_cnt && !pool) ||
			pool_size > MAX_BUFFER_SIZE || TheBigPointer > MAX_BUFFER_SIZE)
			return U32(-1);

		const U32 required = pool_used_cnt + size;
		U32 newSize = pool_size;
		if (required > pool_size)
		{
			const U32 grown = pool_size > MAX_BUFFER_SIZE - GROW_SIZE
				? MAX_BUFFER_SIZE : pool_size + GROW_SIZE;
			newSize = grown > required ? grown : required;
		}

		if ( newSize > pool_size || pool==0)
		{
			if (newSize <= MAX_BUFFER_SIZE - TheBigPointer)
			{
				pool_size = newSize;
				U8 * new_pool = &TheBigBuffer[TheBigPointer];
				if (pool_used_cnt)
					memmove(new_pool,pool,pool_used_cnt);
				pool = new_pool;
				TheBigPointer += newSize;
			}
			else
			{
#if 0
				CQTRACE10("Render buffer is full");
#endif
				return 0xffffffff;
			}
		}

		//void *ret = &pool[pool_used_cnt];
		U32 ret = pool_used_cnt;
		old_used_cnt = pool_used_cnt;
		pool_used_cnt += size;
		if (pool_used_cnt)
		{
			bRendered = 1;
			if( set_non_empty ) {
				pool_flags &= ~(BM_P_F_EMPTY);
			}
		}

		return ret;
	}

	void BMPOOLJR::set_pool_used_cnt( U32 cnt)
	{
		if (old_used_cnt > pool_used_cnt || cnt > pool_used_cnt - old_used_cnt)
		{
			CQASSERT(false);
			return;
		}
		pool_used_cnt = old_used_cnt+cnt;
	}

	//

	void BMPOOLJR::free_all()
	{
//		CQASSERT((U32)owner == 0xffffffff || owner->prim.verts_cnt == 0);
		old_used_cnt = pool_used_cnt = 0;
		pool_flags |= BM_P_F_EMPTY;
		pool = 0;
	}

	//
	void BMPOOLJR::update( bool bInternal )
	{
		(void)bInternal;
	/*	if (bInternal)
		{
			if (pool_used_cnt == 0)
			{
				pool = 0;
			}
		}
		else
		{*/
			CQASSERT(pool_used_cnt == 0);

			old_used_cnt = pool_used_cnt = 0;

			if (!bLastRendered && !bRendered && pool_size > GROW_SIZE*4)
				pool_size = GROW_SIZE*4;

			bLastRendered = bRendered;
			bRendered = 0;
			pool = 0;
	//	}
	}
