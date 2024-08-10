#include "fiber.h"
#include "assert.h"

struct fiber_stack_frame *
fiber_stack_frame_new(struct fiber *fiber)
{
	struct fiber_stack_frame *frame = (struct fiber_stack_frame *)
		malloc(sizeof(struct fiber_stack_frame));
	frame->fiber = fiber;
	frame->ctx = NULL;
	return frame;
}

void
fiber_stack_frame_del(struct fiber_stack_frame *frame)
{
	free(frame->ctx);
	free(frame);
}

struct fiber *
fiber_new(const char *name, fiber_func f)
{
	struct fiber *fiber = (struct fiber *)malloc(sizeof(*fiber));
	fiber->name = name;
	fiber->f = f;
	rlist_create(&fiber->stack);
	rlist_add_tail_entry(&fiber->stack, fiber_stack_frame_new(fiber), in_stack);
	return fiber;
}

void
fiber_del(struct fiber *fiber)
{
	struct fiber_stack_frame *first_frame =
		rlist_first_entry(&fiber->stack, struct fiber_stack_frame, in_stack);
	rlist_del_entry(first_frame, in_stack);
	fiber_stack_frame_del(first_frame);
	assert(rlist_empty(&fiber->stack));
	free(fiber);
}