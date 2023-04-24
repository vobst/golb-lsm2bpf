CC ?= clang

memfd_exec: memfd_exec.c memfd_exec_prog.h
	$(CC) -o $@ $<

memfd_exec_prog.h: memfd_exec_prog
	xxd -i $< > $@
	rm $<

memfd_exec_prog: memfd_exec_prog.c
	$(CC) -o $@ $<
