

sdk_generate_library(uart_stdio)

sdk_add_include_directories(
    Inc
)

sdk_library_add_sources(
    uart_stdio.c
)