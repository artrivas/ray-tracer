add_library(raylib SHARED IMPORTED GLOBAL)
set_target_properties(
        raylib
        PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include/"
        IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/libraylib.a"
)

