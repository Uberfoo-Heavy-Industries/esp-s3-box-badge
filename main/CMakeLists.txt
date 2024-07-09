idf_component_register(SRCS "main.cpp" "metaballs.c" "fire.cpp"
                    INCLUDE_DIRS ".")

spiffs_create_partition_image(storage ../spiffs FLASH_IN_PROJECT)