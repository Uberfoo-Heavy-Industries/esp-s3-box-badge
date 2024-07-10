idf_component_register(SRCS "main.cpp" "fire.cpp"
                    INCLUDE_DIRS ".")

spiffs_create_partition_image(storage ../spiffs FLASH_IN_PROJECT)