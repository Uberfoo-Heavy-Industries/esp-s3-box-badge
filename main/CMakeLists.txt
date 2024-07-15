file(GLOB_RECURSE SOURCES "*.cpp")
idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS "." "demo" "ui")

spiffs_create_partition_image(storage ../spiffs FLASH_IN_PROJECT)