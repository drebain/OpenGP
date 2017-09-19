find_package(PNG REQUIRED)

include_directories(${PNG_INCLUDE_DIRS})
add_definitions(${PNG_DEFINITIONS})
list(APPEND LIBRARIES ${PNG_LIBRARIES})

if(NOT PNG_FOUND)
    message(WARNING "libpng not found")
endif(NOT PNG_FOUND)
