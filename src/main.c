#include "stdio.h"
#include "fiber.h"

int
bar(fiber_args(int v))
{
	fiber_begin_context;
	    int i;
	fiber_end_context;
	fiber_func_body_begin;
		for (ctx->i = 0; ctx->i < 5; ctx->i++) {
			printf("%d ", v);
			fflush(stdout);
			fiber_yield();
		}
		printf("\n");
		fiber_return(0);
	fiber_func_body_end;
}

int
foo(fiber_args(int from)) {
	fiber_begin_context;
	    int i;
	fiber_end_context;
	fiber_func_body_begin;
		for (ctx->i = from; ctx->i < from + 2; ctx->i++) {
			int ret;
			fiber_call(ret, bar, ctx->i);
			fiber_yield();
		}
		fiber_return(0);
	fiber_func_body_end;
}

int
main (void)
{
	struct fiber *f = fiber_new("my_fiber", foo);
	fiber_start(f, 1);
	while(!f->dead)
		fiber_wakeup(f, 1);
	fiber_del(f);
	return 0;
}