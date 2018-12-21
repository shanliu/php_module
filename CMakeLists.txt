# Clion 用到的CMAK文件
aux_source_directory(. EXT_LLY_SRC)
add_library(_lly_src SHARED ${EXT_LLY_SRC})
add_custom_target(lly_min COMMAND make && sudo make install
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/ext/lly)
add_custom_target(lly_all COMMAND phpize --clean && phpize && ./configure &&  make && sudo make install
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/ext/lly)