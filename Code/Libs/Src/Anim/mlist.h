#ifndef MLIST_H
#define MLIST_H

//

#include "STDDAT.H"
#include "mnode.h"
#include "FDump.h"
#if !defined(_WIN32)
#include <cstdlib>
#endif

//

struct MotionList : public LList<MotionStackNode>
{
#if !defined(_WIN32)
    // MotionStackNode has no virtual destructor. Its three source node kinds
    // must be released with their allocated type, without changing node vtables
    // or the supported Windows module layout.
    void free(MotionStackNode *node)
    {
        unlink(node);
        switch (node->type)
        {
        case MSN_CHANNEL: delete static_cast<ChannelNode *>(node); break;
        case MSN_TRANSITION: delete static_cast<TransitionNode *>(node); break;
        case MSN_POSE: delete static_cast<PoseNode *>(node); break;
        default: std::abort();
        }
    }

    void free()
    {
        while (first()) free(first());
    }
#endif
	const MotionList& operator = (const MotionList& other)
	{
		MotionStackNode * node = other.first();
		while (node)
		{
			switch (node->type)
			{
				case MSN_CHANNEL:
				{
					ChannelNode * chan = (ChannelNode *) node;
					ChannelNode * new_chan = new ChannelNode(chan->object);
					link(new_chan);
					break;
				}
				case MSN_TRANSITION:
				{
					TransitionNode * trans = (TransitionNode *) node;
					TransitionNode * new_trans = new TransitionNode(trans->duration);
					new_trans->time = trans->time;
					link(new_trans);
					break;
				}
				case MSN_POSE:
				{
					PoseNode * pose = (PoseNode *) node;
					PoseNode * new_pose = new PoseNode(*pose);
					link(new_pose);
					break;
				}
			}
			node = other.next(node);
		}

		return *this;
	}

	void remove(Object * obj)
	{
		MotionStackNode * node = first();
		while (node)
		{
			if (node->type == MSN_CHANNEL)
			{
				ChannelNode * chan = (ChannelNode *) node;
				if (chan->object == obj)
				{
					free(node);
					break;
				}
			}
			node = next(node);
		}
	}

// Warning: recursive function.
	void remove_transition(MotionStackNode * node, bool remove_dst)
	{
		ASSERT(node->type == MSN_TRANSITION);

		MotionStackNode * dst = node->prev;
		ASSERT(dst->type != MSN_TRANSITION);

		MotionStackNode * src = dst->prev;
		if (src->type == MSN_TRANSITION)
		{
			remove_transition(src, true);
		}
		else
		{
			if (src->type == MSN_CHANNEL)
			{
				ChannelNode * chan = (ChannelNode *) src;
				chan->object->finish();
			}
			free(src);
		}

		if (remove_dst)
		{
			if (dst->type == MSN_CHANNEL)
			{
				ChannelNode * chan = (ChannelNode *) dst;
				chan->object->finish();
			}
			free(dst);
		}

		free(node);
	}

	void verify(void)
	{
#ifndef NDEBUG	// DEBUG STUFF
		MotionStackNode * node = first();
		while (node)
		{
			if (node->is_transition())
			{
				TransitionNode * trans = (TransitionNode *) node;
				if (!trans->is_valid_transition())
				{
					int dummy = 0;
				}
			}

			node = next(node);
		}
#endif
	}

	~MotionList(void)
	{
		free();
	}
};


//

#endif
