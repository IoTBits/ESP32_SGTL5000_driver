deps_config := \
	/home/esp32-dev/Desktop/GitHub/esp-idf/components/aws_iot/Kconfig \
	/home/esp32-dev/Desktop/GitHub/esp-idf/components/bt/Kconfig \
	/home/esp32-dev/Desktop/GitHub/esp-idf/components/esp32/Kconfig \
	/home/esp32-dev/Desktop/GitHub/esp-idf/components/ethernet/Kconfig \
	/home/esp32-dev/Desktop/GitHub/esp-idf/components/fatfs/Kconfig \
	/home/esp32-dev/Desktop/GitHub/esp-idf/components/freertos/Kconfig \
	/home/esp32-dev/Desktop/GitHub/esp-idf/components/log/Kconfig \
	/home/esp32-dev/Desktop/GitHub/esp-idf/components/lwip/Kconfig \
	/home/esp32-dev/Desktop/GitHub/esp-idf/components/mbedtls/Kconfig \
	/home/esp32-dev/Desktop/GitHub/esp-idf/components/openssl/Kconfig \
	/home/esp32-dev/Desktop/GitHub/esp-idf/components/spi_flash/Kconfig \
	/home/esp32-dev/Desktop/GitHub/esp-idf/components/bootloader/Kconfig.projbuild \
	/home/esp32-dev/Desktop/GitHub/esp-idf/components/esptool_py/Kconfig.projbuild \
	/home/esp32-dev/Desktop/GitHub/esp-idf/components/partition_table/Kconfig.projbuild \
	/home/esp32-dev/Desktop/GitHub/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)


$(deps_config): ;
