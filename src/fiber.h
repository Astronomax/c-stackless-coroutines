#ifndef C_STACKLESS_COROUTINES_FIBER_H
#define C_STACKLESS_COROUTINES_FIBER_H

#include <stdlib.h>
#include <stdbool.h>
#include "rlist.h"
#include "setjmp.h"
#include "assert.h"

#define fiber() frame->fiber

#define fiber_args(...) struct fiber_stack_frame *frame, __VA_ARGS__

#define fiber_yield_impl(x)							\
	do {									\
    		ctx->line = x;							\
    		longjmp(fiber()->env, 1);					\
		case x:;							\
        } while (0)

#define fiber_yield() fiber_yield_impl(__COUNTER__)

#define fiber_call_impl(x, r, f, ...) 						\
	    	ctx->line = x;							\
		case x:                 					\
		r = f(rlist_next_entry(frame, in_stack), __VA_ARGS__);

#define fiber_call(r, f, ...) fiber_call_impl(__COUNTER__, r, f, __VA_ARGS__)

#define fiber_wakeup(fiber, ...) 						\
	do {                                  					\
                if (setjmp(fiber->env) == 0)					\
			fiber->ret = fiber->f(rlist_first_entry(          	\
						&fiber->stack,                  \
						struct fiber_stack_frame,       \
						in_stack), __VA_ARGS__);        \
	} while(0)

#define fiber_start(fiber, ...) fiber_wakeup(fiber, __VA_ARGS__)

struct fiber_stack_frame {
    struct fiber *fiber;
    void *ctx;
    struct rlist in_stack;
};

struct fiber_stack_frame *
fiber_stack_frame_new(struct fiber *fiber);

void
fiber_stack_frame_del(struct fiber_stack_frame *frame);

#define fiber_begin_context  struct fiber_context_tag { int line
#define fiber_end_context } 							\
	*ctx = (struct fiber_context_tag *)frame->ctx;				\

#define fiber_push_new_frame_impl(x) 						\
	ctx->line = x; 								\
	rlist_add_tail_entry(           					\
		&fiber()->stack, 						\
		fiber_stack_frame_new(fiber()),					\
		in_stack);             						\
	case x:;

#define fiber_push_new_frame fiber_push_new_frame_impl(__COUNTER__)

#define fiber_func_body_begin 							\
	if(!ctx) {                         	 				\
		ctx = (struct fiber_context_tag *)malloc(sizeof(*ctx));		\
                frame->ctx = (void *)ctx;					\
		ctx->line = -1;                    				\
	}									\
	switch(ctx->line) {                 					\
		case -1:;							\
		fiber_push_new_frame

#define fiber_return_impl							\
	do {									\
                struct fiber_stack_frame *next_frame =				\
			rlist_next_entry(frame, in_stack);              	\
		rlist_del_entry(next_frame, in_stack);				\
		fiber_stack_frame_del(next_frame);				\
                if (rlist_first_entry(&fiber()->stack,				\
			struct fiber_stack_frame, in_stack) == frame)		\
                	fiber()->dead = true;                    		\
                assert(frame->ctx != NULL);                			\
                free(frame->ctx);                				\
		frame->ctx = NULL;             					\
	} while(0)

#define fiber_return(v) fiber_return_impl; return v;

#define fiber_func_body_end fiber_return_impl; }

typedef int (*fiber_func)(struct fiber_stack_frame *, int);

struct fiber {
	const char *name;
    	jmp_buf env;
	fiber_func f;
	struct rlist stack;
	bool dead;
	int ret;
};

struct fiber *
fiber_new(const char *name, fiber_func f);

void
fiber_del(struct fiber *fiber);

#endif //C_STACKLESS_COROUTINES_FIBER_H
