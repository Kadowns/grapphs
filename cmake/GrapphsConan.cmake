if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(
            DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/release/0.18/conan.cmake"
            "${CMAKE_BINARY_DIR}/conan.cmake"
            TLS_VERIFY ON
    )
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

function(grapphs_check_should_run_conan_install OUTPUT)
    if(DEFINED CONAN_EXPORTED)
        # We are being invoked from conan, nothing to do.
        set(${OUTPUT} FALSE PARENT_SCOPE)
        return()
    endif()

    if(GRAPPHS_COMPILE_TESTS OR GRAPPHS_COMPILE_SAMPLES)
        set(${OUTPUT} TRUE PARENT_SCOPE)
    else()
        set(${OUTPUT} FALSE PARENT_SCOPE)
    endif()
endfunction()

macro(grapphs_run_conan_install CONANFILE)
#    grapphs_check_should_run_conan_install(SHOULD_RUN)
#
#    if(NOT SHOULD_RUN)
#        message(STATUS "Skipping conan install")
#        return()
#    endif()

    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR})
    list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_BINARY_DIR})

    get_property(IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    if(IS_MULTI_CONFIG)
        set(CONAN_GENERATOR cmake_find_package_multi)
    else()
        set(CONAN_GENERATOR cmake_find_package)
    endif()

    message(VERBOSE "Conan Settings: ${CONAN_SETTINGS}")

    if(GRAPPHS_COMPILE_SAMPLES)
        list(APPEND CONAN_OPTIONS "samples=True")
    else()
        list(APPEND CONAN_OPTIONS "samples=False")
    endif()

    #    if(GRAPPHS_COMPILE_TESTS)
    #        list(APPEND CONAN_OPTIONS "samples=True")
    #    else()
    #        list(APPEND CONAN_OPTIONS "samples=False")
    #    endif()
    #
    #    if(GRAPPHS_COMPILE_SVG)
    #        list(APPEND CONAN_OPTIONS "samples=True")
    #    else()
    #        list(APPEND CONAN_OPTIONS "samples=False")
    #    endif()
    #
    #    if(GRAPPHS_COMPILE_GRAPHVIZ)
    #        list(APPEND CONAN_OPTIONS "samples=True")
    #    else()
    #        list(APPEND CONAN_OPTIONS "samples=False")
    #    endif()

    if(IS_MULTI_CONFIG)
        foreach(TYPE ${CMAKE_CONFIGURATION_TYPES})
            conan_cmake_autodetect(CONAN_SETTINGS BUILD_TYPE ${TYPE})
            conan_cmake_install(
                    PATH_OR_REFERENCE ${CONANFILE}
                    BUILD missing
                    SETTINGS ${CONAN_SETTINGS}
            )
        endforeach()
    else()
        conan_cmake_autodetect(CONAN_SETTINGS BUILD_TYPE ${CMAKE_BUILD_TYPE})
        conan_cmake_install(
                PATH_OR_REFERENCE ${CONANFILE}
                BUILD missing
                SETTINGS ${CONAN_SETTINGS}
        )
    endif()
endmacro()