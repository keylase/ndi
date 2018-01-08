ifeq ($(NDILIB),)
	CC    ?= gcc
	NDILIB = $(shell $(CC) -dumpmachine)
endif

BMDSDK = NDIlib_Send_BMD/BMDSDK/Linux

PROJECTS = \
		   NDIlib_DynamicLoad \
		   NDIlib_Find \
		   NDIlib_Recv \
		   NDIlib_Recv_Audio_16bpp \
		   NDIlib_Recv_Multichannel \
		   NDIlib_Recv_PTZ \
		   NDIlib_Recv_Record \
		   NDIlib_Recv_WebControl \
		   NDIlib_Routing \
		   NDIlib_Send_Audio \
		   NDIlib_Send_Audio_16bpp \
		   NDIlib_Send_Benchmark \
		   NDIlib_Send_BMD \
		   NDIlib_Send_PNG \
		   NDIlib_Send_Video \
		   NDIlib_Send_Video_Advanced \
		   NDIlib_Send_Video_and_Audio \
		   NDIlib_Send_Video_Async

.PHONY: all
all:
	@test -d ../../lib/$(NDILIB) || (echo NDILIB not found: $(NDILIB) >&2 ; false)
	-@for proj in $(PROJECTS); do \
		$(MAKE) -C $$proj -f ../Makefile.proj NDILIB=$(NDILIB); \
	done

.PHONY: clean
clean:
	-@for proj in $(PROJECTS); do \
		$(MAKE) -C $$proj -f ../Makefile.proj clean; \
	done
