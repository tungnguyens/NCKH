	CONTIKI=../../..
CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

ifndef TARGET
TARGET=sky
USING_CC2530=0
endif
ifeq ($(TARGET),cc2530dk)
HAVE_BANKING=1
USING_CC2530=1
endif

WITH_UIP6=1
UIP_CONF_IPV6=1

CFLAGS+= -DUIP_CONF_IPV6_RPL

PROJECT_SOURCEFILES += util.c aes.c 

ifeq ($(TARGET),cc2530dk)
#PROJECT_SOURCEFILES += 
endif

CONTIKI_PROJECT = router

all: $(CONTIKI_PROJECT) 

include $(CONTIKI)/Makefile.include
