include(FindPackageHandleStandardArgs)
set(${CMAKE_FIND_PACKAGE_NAME}_CONFIG ${CMAKE_CURRENT_LIST_FILE})
find_package_handle_standard_args(mark CONFIG_MODE)

if (MSVC)
  option(MARK_MSVC_USE_STANDARD_REFERENCE_BINDING_BEHAVIOR "(recommand)use standard reference binding behavior on msvc" ON)
endif()

if(NOT TARGET mark::mark)
    include("${CMAKE_CURRENT_LIST_DIR}/markTargets.cmake")
    if((NOT TARGET ) AND
       (NOT mark_FIND_VERSION OR
        mark_FIND_VERSION VERSION_LESS 0.0.1))
        add_library(mark INTERFACE IMPORTED)
        set_target_properties(mark PROPERTIES
            INTERFACE_LINK_LIBRARIES mark::mark
        )
    endif()
endif()
