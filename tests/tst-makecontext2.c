/* Copyright (C) 2008 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ucontext.h>
#include "tst-stack-align.h"

ucontext_t ucp, ucp2;
char st1[262144] __attribute__((aligned (16)));

void
cf (int i, int j)
{
  if (i != 78 || j != 274)
    {
      printf ("error: i %d j %d\n", i, j);
      exit (1);
    }
  else if (TEST_STACK_ALIGN ())
    {
      puts ("insufficiently aligned stack");
      exit (2);
    }
}

int
main (void)
{
  size_t j;
  for (j = 32; j < 64; j += sizeof (long))
    {
      if (getcontext (&ucp) != 0)
	{
	  if (errno == ENOSYS)
	    {
	      puts ("context handling not supported");
	      return 0;
	    }

	  puts ("getcontext failed");
	  return 1;
	}
      ucp.uc_link = &ucp2;
      ucp.uc_stack.ss_sp = st1;
      ucp.uc_stack.ss_size = sizeof (st1) - j;
      memset (&st1[sizeof (st1) - j], 0x55, j);
      makecontext (&ucp, (void (*) (void)) cf, 2, 78, 274);
      if (swapcontext (&ucp2, &ucp) != 0)
	{
	  puts ("setcontext failed");
	  return 1;
	}
      size_t i;
      for ( i = j; i > 0; i--)
	if (st1[sizeof (st1) - j + i - 1] != 0x55)
	  { printf ("fail %zd %zd\n", i, j); break; }
    }

  return 0;
}

