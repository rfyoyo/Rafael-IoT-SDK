# Automatically generated build file. Do not edit.
COMPONENT_LIB_ONLY := 1
COMPONENT_ADD_INCLUDEDIRS += include platform/osif/include
LIBS ?= zboss
COMPONENT_ADD_LDFLAGS += -L$(COMPONENT_PATH)/lib $(addprefix -l,$(LIBS))
COMPONENT_ADD_LINKER_DEPS += lib/libzboss.a