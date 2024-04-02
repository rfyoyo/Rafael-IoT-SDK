#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#
SOC_DRV = $(shell echo $(CONFIG_CHIP_NAME) | tr A-Z a-z)
TARGET_BOARD = $(shell echo $(CONFIG_TARGET_BOARD) | tr A-Z a-z)

PROJECT_PATH := $(abspath .)
PROJECT_NAME :=  $(notdir $(PROJECT_PATH))
PROJECT_BOARD := $(TARGET_BOARD)
export PROJECT_PATH PROJECT_BOARD
#CONFIG_TOOLPREFIX :=

-include ./proj_config.mk

RAFAEL_IOT_SDK_PATH_GUESS ?= $(shell pwd)
RAFAEL_IOT_SDK_PATH ?= $(RAFAEL_IOT_SDK_PATH_GUESS)/../..
$(info ****** SDK PATH [$(RAFAEL_IOT_SDK_PATH)])
INCLUDE_COMPONENTS += $(PROJECT_NAME)
INCLUDE_COMPONENTS += $(SOC_DRV)_$(PROJECT_BOARD) $(SOC_DRV)_driver rt569_rf rt569mp ruci $(SOC_DRV)_hosal $(SOC_DRV)_freertos uart_stdio cli newlibc log

include $(RAFAEL_IOT_SDK_PATH)/make_scripts/project.mk

EXTRA_LDFLAGS += -Wl,--print-memory-usage
ifdef CONFIG_CACHE_SIZE
EXTRA_LDFLAGS += -Wl,--defsym=__CACHE_SIZE=$(CONFIG_CACHE_SIZE)
endif
