

sdk_generate_library(rt582_driver)

sdk_add_include_directories(
    CMSIS/Include
    Inc
)


sdk_library_add_sources(
    Src/system_cm3_mcu.c
    Src/comparator.c
    Src/dma.c
    Src/flashctl.c
    Src/gpio.c
    Src/i2c_master.c
    Src/i2s.c
    Src/lpm.c
    Src/mp_sector.c
    Src/pwm.c
    Src/qspi.c
    Src/rtc.c
    Src/sadc.c
    Src/swi.c
    Src/swt.c
    Src/sysctrl.c
    Src/sysfun.c
    Src/timer.c
    Src/wdt.c
)