    set(SOURCES_LIST
        ${CMAKE_CURRENT_SOURCE_DIR}/micro_robot.c
        ${CMAKE_CURRENT_SOURCE_DIR}/LCD_1inch28.c
        ${CMAKE_CURRENT_SOURCE_DIR}/LCD_1inch28_test.c
        ${CMAKE_CURRENT_SOURCE_DIR}/image.c
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI_Paint.c
        ${CMAKE_CURRENT_SOURCE_DIR}/font24CN.c
        ${CMAKE_CURRENT_SOURCE_DIR}/font24.c
        ${CMAKE_CURRENT_SOURCE_DIR}/font20.c
        ${CMAKE_CURRENT_SOURCE_DIR}/font16.c
        ${CMAKE_CURRENT_SOURCE_DIR}/font12CN.c
        ${CMAKE_CURRENT_SOURCE_DIR}/font12.c
        ${CMAKE_CURRENT_SOURCE_DIR}/font8.c
        ${CMAKE_CURRENT_SOURCE_DIR}/DEV_Config.c
        ${CMAKE_CURRENT_SOURCE_DIR}/wifi_connect.c
    )

    set(PUBLIC_HEADER_LIST
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

    set(SOURCES "${SOURCES_LIST}" PARENT_SCOPE)
    set(PUBLIC_HEADER "${PUBLIC_HEADER_LIST}" PARENT_SCOPE)
