macro(PackTarget targetName)
    # NOTE: 需要设置 SET_TARGET_PROPERTIES(${PROJECT_NAME}  PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    #   SET_TARGET_PROPERTIES(${PROJECT_NAME}  PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
    #   SET_TARGET_PROPERTIES(${PROJECT_NAME}  PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)
    if(WIN32 OR MINGW)
        if(${CMAKE_BUILD_TYPE} MATCHES "Rel")
            set(RelType "--release")
        endif()
    set(_${targetName}DepDir ${CMAKE_BINARY_DIR}/_${targetName}InstallDep)
    find_program(WinDeployQtTool windeployqt REQUIRED)
    add_custom_command(TARGET ${targetName} PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory ${_${targetName}DepDir})
    add_custom_command(TARGET ${targetName} POST_BUILD
            COMMAND ${CMAKE_COMMAND}
            -E copy
            ${CMAKE_BINARY_DIR}/bin/${targetName}.exe
            ${_${targetName}DepDir}
            COMMAND ${WinDeployQtTool}
            ${RelType}
            ${_${targetName}DepDir}/${targetName}.exe
            )
    install(TARGETS ${targetName} DESTINATION bin)
    install(DIRECTORY ${_${targetName}DepDir}/ TYPE BIN)

     set(CPACK_VERBATIM_VARIABLES YES)
    set(CPACK_PACKAGE_INSTALL_DIRECTORY ${targetName})
    SET(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_BINARY_DIR}/_packages")
    set(CPACK_STRIP_FILES YES)

    set(
            CPACK_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS
            OWNER_READ OWNER_WRITE OWNER_EXECUTE
            GROUP_READ GROUP_EXECUTE
            WORLD_READ WORLD_EXECUTE
    )

    set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
    set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
    set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
    set(CPACK_PACKAGE_CONTACT "nicebluechai@gmail.com")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "NiceBlueChai <${CPACK_PACKAGE_CONTACT}>")
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")

    set(CPACK_GENERATOR "7Z")

    include(CPack)
    endif()
endmacro()