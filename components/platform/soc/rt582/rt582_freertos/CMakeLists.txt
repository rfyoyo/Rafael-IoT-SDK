

sdk_generate_library(rt582_freertos)

sdk_library_add_sources(
    croutine.c
    event_groups.c
    list.c
    queue.c
    stream_buffer.c
    tasks.c
    timers.c
    portable/GCC/ARM_CM3/port.c
    portable/MemMang/heap_5.c
)

sdk_add_include_directories(
    portable/GCC/ARM_CM3/
    Inc
    config
)
