#HOST_TYPE=/opt/buildroot-gcc463/usr/bin/mipsel-buildroot-linux-uclibc-

#ifdef HOST_TYPE 
#CROSS_COMPILER :=$(HOST_TYPE)
#else
#CROSS_COMPILER :=
#endif

#CC	:= $(CROSS_COMPILER)gcc
#AR	:= $(CROSS_COMPILER)ar
#LD  := $(CROSS_COMPILER)ld

C_DIR = $(shell pwd)
EXEC = alink

CFLAGS_ADT = -DOPENSSL_EXTRA -D_POSIX_THREADS  -DHAVE_THREAD_LS -DNDEBUG -pthread -DNO_DSA -DNO_PSK -DNO_DH -DNO_MD4 -DNO_HC128 -DNO_RABBIT -Wall -Wno-unused -Os -fomit-frame-pointer
  
SRCS   = $(wildcard $(C_DIR)/platform/*.c $(C_DIR)/product/*.c  $(C_DIR)/sample/sample.c )
  
## all .o based on all .c  
OBJS        := $(SRCS:.c=.o)  
## need libs, add at here  

#LDFLAGS :=  -L$(ROOTDIR)/lib/lib 


$(info $(LDFLAGS))


LIBS += -lcyassl -lconfig -lalink -lrt -lpthread 

CFLAGS += -I$(C_DIR)/include  -I$(C_DIR)/include/cyassl  

CFLAGS += -Wall -I$(ROOTDIR)/include/
CFLAGS += -I$(ROOTDIR)/config
CFLAGS += -I$(ROOTDIR)/lib/libconfig/mib
CFLAGS += -I$(ROOTDIR)/lib/libconfig/profacc
CFLAGS += -I$(ROOTDIR)/lib/libconfig/cli
CFLAGS += -I$(ROOTDIR)/uttShareHead/config
LDFLAGS += -L$(ROOTDIR)/lib/lib
CFLAGS += -DUTT_SWVERSION=\"$(UTTVERSION)\" -DUTT_HWMODEL=\"$(UTTHWMODEL)\"

$(EXEC): $(OBJS)
	$(CC)  $(CPPFLAGS)  -o $@ $(OBJS)   $(CFLAGS)  $(LDFLAGS) $(LIBS)   -pthread 





romfs:
	@cp $(EXEC) $(PWD)/../romfs/bin/ 


clean:
	#$(RM) -r $(SDIR) $(DDIR) $(ODIR)
	$(RM) -r ./*/*.o
	$(RM) -r $(EXEC)

