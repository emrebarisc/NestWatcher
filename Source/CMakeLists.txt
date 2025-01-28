add_executable(${ProjectName}
	Main.c
	Display/SSD1306.c
	Display/Display.c
	Network/Wifi.c
	Network/HTTPClientUtil.c
)

target_include_directories(${ProjectName} PRIVATE
	${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(${ProjectName} 
	pico_stdlib
	pico_multicore
	hardware_i2c
	FreeRTOS-Kernel-Heap4
	pico_cyw43_arch
	pico_cyw43_arch_lwip_threadsafe_background
	pico_lwip_http
	pico_lwip_mbedtls
	pico_mbedtls
	)

pico_enable_stdio_usb(${ProjectName} 1)
pico_enable_stdio_uart(${ProjectName} 0)

target_compile_definitions(${ProjectName} PRIVATE
	WIFI_SSID="FiberHGW_ZTP9NJ_2.4GHz"
	WIFI_PASSWORD="EsztXH4DydNk"
	TEST_TCP_SERVER_IP="https://www.binarytorgb.com"
)

# target_compile_definitions(${ProjectName} PRIVATE
# 	WIFI_SSID="FiberHGW_ZTP9NJ_EXT"
# 	WIFI_PASSWORD="EsztXH4DydNk"
# 	TEST_TCP_SERVER_IP="https://www.binarytorgb.com"
# )

pico_add_extra_outputs(${ProjectName})