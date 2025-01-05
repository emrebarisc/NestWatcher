add_executable(${ProjectName}
    Main.c
    Display/SSD1306.c
)

target_include_directories(${ProjectName} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(${ProjectName} 
    pico_stdlib
    pico_multicore
	hardware_i2c
    FreeRTOS-Kernel-Heap4
    )

pico_enable_stdio_usb(${ProjectName} 1)
pico_enable_stdio_uart(${ProjectName} 0)
    
pico_add_extra_outputs(${ProjectName})