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
	return fiber;
}

void
fiber_del(struct fiber *fiber)
{
	assert(rlist_empty(&fiber->stack));
	free(fiber);
}