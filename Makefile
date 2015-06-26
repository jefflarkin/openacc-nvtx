CC=pgcc
PIC=-fpic
AR=ar

liboaccnvtx.so: liboaccnvtx.a
	$(CC) -shared -o $@ -L$(CUDA_HOME)/lib64 -lnvToolsExt -Wl,-soname=$@ -Wl,--whole-archive $< -Wl,--no-whole-archive

openacc-nvtx.o: openacc-nvtx.c
	$(CC) -ta=tesla $(PIC) -I$(CUDA_HOME)/include -c $<

liboaccnvtx.a: openacc-nvtx.o
	$(AR) cr $@ $^

