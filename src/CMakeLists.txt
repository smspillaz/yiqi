set (YIQI_MAIN_SRCS
     ${CMAKE_CURRENT_SOURCE_DIR}/yiqi_main.cpp)

add_library (${YIQI_MAIN_LIBRARY} STATIC
             ${YIQI_MAIN_SRCS})
