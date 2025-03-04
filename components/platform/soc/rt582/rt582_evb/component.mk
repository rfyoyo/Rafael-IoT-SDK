ifeq ($(strip $(PROJECT_BOARD)),evb)
include $(COMPONENT_PATH)/evb/evb.mk

ifdef CONFIG_SYS_APP_TASK_STACK_SIZE
CPPFLAGS += -DSYS_APP_TASK_STACK_SIZE=$(CONFIG_SYS_APP_TASK_STACK_SIZE)
endif

ifdef CONFIG_SYS_APP_TASK_PRIORITY
CPPFLAGS += -DSYS_APP_TASK_PRIORITY=$(CONFIG_SYS_APP_TASK_PRIORITY)
endif

ifdef CONFIG_PLATOFRM_ENABLE_SLEEP
CPPFLAGS += -DCONFIG_PLATOFRM_ENABLE_SLEEP=$(CONFIG_PLATOFRM_ENABLE_SLEEP)
endif

ifdef CONFIG_TARGET_CUSTOMER
CPPFLAGS += -DCONFIG_TARGET_CUSTOMER=\"$(CONFIG_TARGET_CUSTOMER)\"
endif


## This component's src 
COMPONENT_SRCS1 += rt_main.c
		   
COMPONENT_SRCDIRS += .
COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS1))
COMPONENT_OBJS := $(patsubst %.S,%.o, $(COMPONENT_OBJS))

else
$(error "Please Set PROJECT_BOARD:= [your board name] at Makefile")
endif
