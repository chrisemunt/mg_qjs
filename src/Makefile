# UNIX makefile for mg_qjs module (mg_qjs.so)
#
# Build:
# make Makefile
#
# Cleanup:
# make clean
#
# Install:
# make install

# Edit the following three lines to match the paths in your installation 
# /path/to/installation/location/
MGQJSDIR=/opt/nginx1314/modules/
# -I/path/to/quickjs/
MGQJSINC=-I/opt/quickjs/
# -L/path/to/quickjs/
MGQJSLIB=-L/opt/quickjs/

MGQJSDEFINE=-DJS_SHARED_LIBRARY -DMG_XMG_GLOBAL_PREFIX

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
   MGQJSOS = Linux
   CC = cc
   CCFLAGS += -Wall -w -fPIC -DLINUX
   LINK += cc
   LINKFLAGS += -shared
endif
ifeq ($(UNAME_S),Darwin)
   MGQJSOS = OSX
   CC = cc
   CCFLAGS += -fPIC -fno-common -DMACOSX -D_NOTHREADS -DDARWIN
   LINK += cc
   LINKFLAGS += -bundle -flat_namespace -undefined suppress
endif
ifeq ($(UNAME_S),FreeBSD)
   MGQJSOS = FreeBSD
   CC = cc
   CCFLAGS += -DFREEBSD
   LINK = ld
   LINKFLAGS += -G
endif
ifeq ($(UNAME_S),AIX)
   MGQJSOS = AIX
   CC = xlc_r
   CCFLAGS += -DAIX
   LINK = xlc_r
   LINKFLAGS += -G -H512 -T512 -bM:SRE -berok -bexpall -bnoentry
endif

ifndef MGQJSOS
   $(error Cannot determine the OS type)
endif

# Build mg_qjs module
all : app

# Link the object file and dependent libraries into a binary
app : mg_dba.o mg_qjs.o
	$(LINK) -o mg_qjs.so $(LINKFLAGS) $(MGQJSLIB) mg_dba.o mg_qjs.o

# Compile the source file into object file
mg_qjs.o : mg_qjs.c
	$(CC) -o mg_qjs.o $(CCFLAGS) -DMG_QJS_DSO $(MGQJSDEFINE) $(MGQJSINC) -c mg_qjs.c

mg_dba.o : mg_dba.c
	$(CC) -o mg_dba.o $(CCFLAGS) -DMG_QJS_DSO $(MGQJSDEFINE) $(MGQJSINC) -c mg_dba.c

# Install mg_qjs module
install :
	cp mg_qjs.so $(MGQJSDIR)mg_qjs.so

# Clean target
clean :
	rm -rf mg_qjs.so mg_qjs.o mg_dba.o



