# *******************************************************************************************
# * Copyright (c) 2006-7 Laboratorio di Sistemi di Elaborazione e Bioingegneria Informatica *
# *                      Universita' Campus BioMedico - Italy                               *
# *                                                                                         *
# * This program is free software; you can redistribute it and/or modify it under the terms *
# * of the GNU General Public License as published by the Free Software Foundation; either  *
# * version 2 of the License, or (at your option) any later version.                        *
# *                                                                                         *
# * This program is distributed in the hope that it will be useful, but WITHOUT ANY         *
# * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 	    *
# * PARTICULAR PURPOSE. See the GNU General Public License for more details.                *
# *                                                                                         *
# * You should have received a copy of the GNU General Public License along with this       *
# * program; if not, write to the:                                                          *
# * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,                    *
# * MA  02111-1307, USA.                                                                    *
# *                                                                                         *
# * --------------------------------------------------------------------------------------- *
# * Project:  Capwap                                                                        *
# *                                                                                         *
# * Author :  Ludovico Rossi (ludo@bluepixysw.com)                                          *  
# *           Del Moro Andrea (andrea_delmoro@libero.it)                                    *
# *           Giovannini Federica (giovannini.federica@gmail.com)                           *
# *           Massimo Vellucci (m.vellucci@unicampus.it)                                    *
# *	      Donato Capitella (d.capitella@gmail.com)  	                            *
# *******************************************************************************************



#CC = gcc

HAVE_NO_DTLS = 1
ALL_NOT_ENCRYPTED = 1
URCP_IPC = 1
WITH_URCP = 1
#LDFLAGS = -lssl -lcrypto -lpthread -ldl -D_REENTRANT
#LDFLAGS = /usr/lib/libefence.a ./static/libssl.a ./static/libcrypto.a -lpthread -ldl -D_REENTRANT
#LDFLAGS = ./static/libssl.a ./static/libcrypto.a -lpthread -ldl -D_REENTRANT
LDFLAGS = -lpthread -ldl -D_REENTRANT
CFLAGS = -Wall -g -O0 -D_REENTRANT -I./include/
ifneq ($(HAVE_NO_DTLS),)
CFLAGS += -DCW_NO_DTLS
endif
ifneq ($(ALL_NOT_ENCRYPTED),)
CFLAGS += -DCW_ALL_NOT_ENCRYPTED
else
LDFLAGS += ./static/libssl.a 
endif
ifeq ($(URCP_IPC), 1)
CFLAGS += -DURCP_IPC 
endif
CFLAGS += -DJUST_SIMPLE
OPENSSL_INCLUDE = #-I/usr/local/ssl/include/ #Openssl include files

ifeq ($(WITH_URCP), 1)
CUR = $(shell pwd)
LINUXDIR = $(CUR)/../../$(CONFIG_LINUXDIR)
CFLAGS += -Wall -I$(LINUXDIR)/include
CFLAGS += -I$(ROOTDIR)
CFLAGS += -I$(ROOTDIR)/config
CFLAGS += -I$(ROOTDIR)/uttShareHead/config
CFLAGS += -I$(LINUXDIR)/../lib/libconfig/mib
CFLAGS += -I$(LINUXDIR)/../lib/libconfig/profacc
CFLAGS += -I$(LINUXDIR)/../lib/libconfig/cli
LDFLAGS += -L$(LINUXDIR)/../lib/lib
CFLAGS += -DUTT_SWVERSION=\"$(UTTVERSION)\" -DUTT_HWMODEL=\"$(UTTHWMODEL)\"
CFLAGS += -DURCP_MODULE
ifeq ($(CONFIG_DEFAULTS_INTEL_ATOM), y)
CFLAGS += -I$(LINUXDIR)/../lib/zlib
endif
ifeq ($(CONFIG_UTT_AC), y)
CFLAGS += -I$(LINUXDIR)/../lib/libswitch
LDFLAGS += -lswitch -lconfig
else
LDFLAGS +=  -lconfig
endif
ifeq ($(CONFIG_LIB_LIBICONV_FORCE),y)
CFLAGS += -I$(LINUXDIR)/../lib/libiconv
LDFLAGS += -liconv
endif

ifneq ("$(CONFIG_LIB_LIBUSB_USER_FORCE)", "")
CFLAGS += -Wall -I$(ROOTDIR)/lib/libusb-user
LDFLAGS += -lusb-user
endif
vpath %.c urcp
endif
# Memory leak
#LDFLAGS += ../dmalloc-5.5.0/libdmallocth.a
#CFLAGS += -DDMALLOC

# Capwap Debugging
CFLAGS += -DCW_DEBUGGING
CFLAGS += -DOPENSSL_NO_KRB5

RM = /bin/rm -f 
ALL_EXEC = 

ifeq ($(WITH_URCP), 1)
URCP_OBJS = urcp_main.o  urcp.o trap_sta.o
ifneq ("$(CONFIG_UTT_AC)", "")
URCP_OBJS += urcp_cmd.o app.o urcp_hello.o app_msg.o urcp_tftp.o urcp_config.o common.o \
	 web_msg.o urcp_ap.o crc32.o urcp_debug.o hash.o urcp_sta_log.o
else
URCP_OBJS += app.o urcp_tftp.o urcp_config.o common.o web_msg.o urcp_ap.o urcp_cmd.o crc32.o \
			ap_msg.o ap_netlink.o urcp_power.o urcp_debug.o dhcp.o hash.o
endif
endif
# list of generated object files for AC. 
AC_OBJS = AC.o ACConfigFile.o ACMainLoop.o ACDiscoveryState.o ACJoinState.o ACConfigureState.o ACDataCheckState.o ACRunState.o ACProtocol_User.o ACRetransmission.o CWCommon.o CWConfigFile.o CWErrorHandling.o CWList.o CWLog.o ACMultiHomedSocket.o  ACProtocol.o CWSafeList.o CWNetwork.o CWProtocol.o CWRandom.o CWStevens.o CWThread.o CWBinding.o CWVendorPayloadsAC.o ACBinding.o ACInterface.o ACSettingsFile.o timerlib.o ACCustom.o 
ifeq ($(ALL_NOT_ENCRYPTED),)
AC_OBJS  += CWSecurity.o CWOpenSSLBio.o 
endif
ifeq ($(URCP_IPC), 1)
#AC_OBJS  += ACUrcp.o
endif
ifeq ($(WITH_URCP), 1)
AC_OBJS += $(URCP_OBJS)
endif
AC_SRCS = $(AC_OBJS:.o=.c) 
AC_DEPS := $(AC_OBJS:.o=.d)
AC_NAME = acd 
WUM_NAME = WUM
ifeq ($(CONFIG_USER_CAPWAP_AC),y)
ALL_EXEC = $(AC_NAME)
endif


# list of generated object files for WTP. 
WTP_OBJS = WTP.o WTPConfigFile.o WTPProtocol.o WTPProtocol_User.o WTPDiscoveryState.o WTPJoinState.o WTPConfigureState.o WTPDataCheckState.o WTPRunState.o WTPRunStateCheck.o WTPRetransmission.o WTPSulkingState.o CWCommon.o CWConfigFile.o CWErrorHandling.o CWSafeList.o CWList.o CWLog.o CWNetwork.o CWProtocol.o CWRandom.o CWStevens.o CWThread.o CWBinding.o CWVendorPayloadsWTP.o WTPBinding.o WTPDriverInteraction.o WTPSettingsFile.o timerlib.o WTPCustom.o
ifeq ($(ALL_NOT_ENCRYPTED),)
WTP_OBJS += CWSecurity.o CWOpenSSLBio.o 
endif
ifeq ($(URCP_IPC), 1)
#WTP_OBJS += WTPUrcp.o
endif
ifeq ($(WITH_URCP), 1)
WTP_OBJS += $(URCP_OBJS)
endif
WUA_OBJS = WUA.o
WTP_SRCS = $(WTP_OBJS:.o=.c)
WUA_SRCS = $(WUA:OBJS:.o=.c)
WTP_DEPS := $(WTP_OBJS:.o=.d)
WUA_DEPS := $(WUA_OBJS:.o=.d)

WTP_NAME = apd
WUA_NAME = WUA
ifeq ($(CONFIG_USER_CAPWAP_WTP),y)
ALL_EXEC = $(WTP_NAME)
endif


.PHONY: deps clean clean_libs libs

# top-level rule, to compile everything. 
all: $(ALL_EXEC) 

ifeq ($(CONFIG_USER_CAPWAP_AC),y)
$(AC_NAME): $(AC_OBJS) 
	$(CC) $(AC_OBJS) $(CC_FLAGS) $(OPENSSL_INCLUDE) $(LDFLAGS) -o $(AC_NAME) 
	$(STRIP) $(AC_NAME)
$(WUM_NAME): 
	$(MAKE) -C wum
	$(STRIP) wum/wum
endif

ifeq ($(CONFIG_USER_CAPWAP_WTP),y)
$(WTP_NAME): $(WTP_OBJS) 
	$(CC) -DWRITE_STD_OUTPUT $(WTP_OBJS) $(CC_FLAGS) $(LDFLAGS) -o $(WTP_NAME) 
	$(STRIP) $(WTP_NAME)

$(WUA_NAME): $(WUA_OBJS) 
	$(CC) $(WUA_OBJS) $(CC_FLAGS)  $(LDFLAGS) -o $(WUA_NAME) 
	$(STRIP) $(WUA_NAME)
endif

romfs:
	for i in $(ALL_EXEC) ; do $(ROMFSINST) $$i /bin/$$i; done
ifeq ($(CONFIG_USER_CAPWAP_AC),y)
#	$(MAKE) romfs -C wum   	
	$(ROMFSINST) /etc/config.ac
	$(ROMFSINST) /etc/settings.ac
endif
ifeq ($(CONFIG_USER_CAPWAP_WTP),y)
	$(ROMFSINST) /etc/config.ap
	$(ROMFSINST) /etc/settings.ap
endif
clean: 
	-$(RM) $(AC_NAME) $(WTP_NAME) $(WUA_NAME) $(AC_OBJS) $(WTP_OBJS) $(WUA_OBJS) $(AC_DEPS) $(WTP_DEPS)	
	$(MAKE) clean -C wum

clean_deps:
	-$(AC_DEPS) $(WTP_DEPS)
	
deps: $(AC_SRC) $(WTP_SRC)
	$(CC) -MD -E $(AC_SRCS) $(CFLAGS) >/dev/null
	$(CC) -MD -E -DWRITE_STD_OUTPUT $(WTP_SRCS) $(CFLAGS) >/dev/null

-include $(AC_DEPS)
-include $(WTP_DEPS)
