# Automatically generated build file. Do not edit.
COMPONENT_LIB_ONLY := 1
COMPONENT_ADD_INCLUDEDIRS += inc
LIBS ?= cpc

ifeq ($(CONFIG_CPC_CRC_0), 1)
COMPONENT_ADD_LDFLAGS += -L$(COMPONENT_PATH)/lib $(addprefix -l, cpc_crc)
COMPONENT_ADD_LINKER_DEPS += lib/libcpc_crc.a
else
COMPONENT_ADD_LDFLAGS += -L$(COMPONENT_PATH)/lib $(addprefix -l,$(LIBS))
COMPONENT_ADD_LINKER_DEPS += lib/libcpc.a
endif