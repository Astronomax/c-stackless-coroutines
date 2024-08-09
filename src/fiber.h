#ifndef C_STACKLESS_COROUTINES_FIBER_H
#define C_STACKLESS_COROUTINES_FIBER_H

#include <stdlib.h>
#include <stdbool.h>
#include "rlist.h"

#define fiber() frame->fiber

#define fiber_yield_impl(x)							\
	do {									\
    		ctx->line = x;							\
    		fiber()->in_yield = true;				 	\
    		return fiber_yield_value; case x:;				\
        } while (0)

#define fiber_yield() fiber_yield_impl(__COUNTER__)

#define fiber_call(r, f, ...) 							\
	do {           								\
		ctx->line = __LINE__; 						\
                rlist_add_tail_entry(           				\
			&fiber()->stack, 					\
			fiber_stack_frame_new(fiber()),				\
			in_stack);             					\
                case __LINE__:;							\
		r = f(rlist_next_entry(frame, in_stack), __VA_ARGS__); 		\
        	if (fiber()->in_yield) return fiber_yield_value;		\
	} while(0)

#define fiber_start(fiber, ...) 						\
	do {           								\
                rlist_add_tail_entry(           				\
			&fiber->stack, 						\
			fiber_stack_frame_new(fiber),				\
			in_stack);             					\
		fiber->ret = fiber->f(rlist_first_entry(          		\
					&fiber->stack,                    	\
					struct fiber_stack_frame,         	\
					in_stack), __VA_ARGS__); 		\
	} while(0)

#define fiber_wakeup(fiber, ...) 						\
	do {           								\
		fiber->ret = fiber->f(rlist_first_entry(          		\
					&fiber->stack,                    	\
					struct fiber_stack_frame,         	\
					in_stack), __VA_ARGS__);          	\
                fiber->in_yield = false;					\
	} while(0)

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

#define fiber_func_decl_begin(R, name, ...) 					\
	R name(struct fiber_stack_frame *frame, __VA_ARGS__) { 			\
	R fiber_yield_value;

#define fiber_func_decl_end }

#define fiber_func_body_begin 							\
	if(!ctx) {                         	 				\
	ctx = (struct fiber_context_tag *)malloc(sizeof(*ctx));			\
                frame->ctx = (void *)ctx;                			\
		ctx->line = -1;                          			\
	}									\
	switch(ctx->line) { case -1:;

#define fiber_return(v)								\
	rlist_del_entry(frame, in_stack);             	 			\
        if (rlist_empty(&fiber()->stack))            				\
		fiber()->dead = true;						\
	fiber_stack_frame_del(frame); 						\
	return v;

#define fiber_func_body_end } return(fiber_yield_value);

typedef int (*fiber_func)(struct fiber_stack_frame *, int);

struct fiber {
	const char *name;
	fiber_func f;
	struct rlist stack;
	bool in_yield;
	bool dead;
	int ret;
};

struct fiber *
fiber_new(const char *name, fiber_func f);

void
fiber_del(struct fiber *fiber);

#endif //C_STACKLESS_COROUTINES_FIBER_H
