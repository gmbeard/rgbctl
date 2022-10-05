find_package(PkgConfig)
pkg_check_modules(PC_Tcc QUIET Tcc)

find_path(Tcc_INCLUDE_DIR
    NAMES libtcc.h
    PATHS ${PC_Tcc_INCLUDE_DIRS}
    PATH_SUFFIXES tcc
)

find_library(Tcc_LIBRARY
    NAMES tcc
    PATHS ${PC_Tcc_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Tcc
    FOUND_VAR Tcc_FOUND
    REQUIRED_VARS
        Tcc_LIBRARY
        Tcc_INCLUDE_DIR
)

if(Tcc_FOUND)
    set(Tcc_LIBRARIES ${Tcc_LIBRARY})
    set(Tcc_INCLUDE_DIRS ${Tcc_INCLUDE_DIR})
    set(Tcc_DEFINITIONS ${PC_Tcc_CFLAGS_OTHER})
endif()

if(Tcc_FOUND AND NOT TARGET Tcc::Tcc)
    add_library(Tcc::Tcc UNKNOWN IMPORTED)
    set_target_properties(Tcc::Tcc
        PROPERTIES
            IMPORTED_LOCATION "${Tcc_LIBRARY}"
            INTERFACE_COMPILE_OPTIONS "${PC_Tcc_CFLAGS_OTHER}"
            INTERFACE_INCLUDE_DIRECTORIES "${Tcc_INCLUDE_DIR}"
    )
endif()

mark_as_advanced(
    Tcc_INCLUDE_DIR
    Tcc_LIBRARY
)
