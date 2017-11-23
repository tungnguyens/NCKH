DEFINES+=PROJECT_CONF_H=\"project-conf.h\"

ifndef TARGET
TARGET=sky
endif
ifeq ($(TARGET),cc2530dk)
HAVE_BANKING=1
USING_CC2530=1
endif
# We need uIPv6, therefore we also need banking
#HAVE_BANKING=1
UIP_CONF_IPV6=1
UIP_CONF_RPL=1

PROJECT_SOURCEFILES += slip-bridge.c

CONTIKI_PROJECT = border-router

all: $(CONTIKI_PROJECT) 

CONTIKI = ../../..

include $(CONTIKI)/Makefile.include
