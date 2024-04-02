# Automatically generated build file. Do not edit.
COMPONENT_LIB_ONLY := 1
COMPONENT_ADD_INCLUDEDIRS += Inc
LIBS ?= openthread
COMPONENT_ADD_LDFLAGS += -L$(COMPONENT_PATH)/lib $(addprefix -l,$(LIBS))
COMPONENT_ADD_LINKER_DEPS += lib/libopenthread.a