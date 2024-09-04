file(GLOB_RECURSE SOURCES "*.cpp" "*.c")
idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS "." "demo" "ui" "service")

spiffs_create_partition_image(storage ../spiffs FLASH_IN_PROJECT)

