find_package (ogg QUIET ${LUGGCGL_OGG_DOWNLOAD_VERSION})
if (NOT ogg)
	FetchContent_Declare (
		ogg
		GIT_REPOSITORY [[https://github.com/xiph/ogg.git]]
		GIT_TAG "v${LUGGCGL_OGG_DOWNLOAD_VERSION}"
		GIT_SHALLOW ON
	)

	FetchContent_GetProperties (ogg)
	if (NOT ogg_POPULATED)
		message (STATUS "Cloning ogg container")
		FetchContent_Populate (ogg)
	endif ()

	set (ogg_INSTALL_DIR "${FETCHCONTENT_BASE_DIR}/ogg-install")
	if (NOT EXISTS "${ogg_INSTALL_DIR}")
		file (MAKE_DIRECTORY ${ogg_INSTALL_DIR})
	endif ()

	message (STATUS "Setting up CMake for ogg container…")
	execute_process (
		COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}"
		                         -A "${CMAKE_GENERATOR_PLATFORM}"
		                         -DCMAKE_INSTALL_PREFIX=${ogg_INSTALL_DIR}
		                         -DCMAKE_BUILD_TYPE=Release
		                         ${ogg_SOURCE_DIR}
		OUTPUT_VARIABLE stdout
		ERROR_VARIABLE stderr
		RESULT_VARIABLE result
		WORKING_DIRECTORY ${ogg_BINARY_DIR}
	)
	if (result)
		message (FATAL_ERROR "CMake setup for ogg failed: ${result}\n"
		                     "Standard output: ${stdout}\n"
		                     "Error output: ${stderr}")
	endif ()

	message (STATUS "Building and installing the ogg container…")
	execute_process (
		COMMAND ${CMAKE_COMMAND} --build ${ogg_BINARY_DIR}
		                         --config Release
		                         --target install
		OUTPUT_VARIABLE stdout
		ERROR_VARIABLE stderr
		RESULT_VARIABLE result
	)
	if (result)
		message (FATAL_ERROR "Build step for ogg failed: ${result}\n"
		                     "Standard output: ${stdout}\n"
		                     "Error output: ${stderr}")
	endif ()

	list (APPEND CMAKE_PREFIX_PATH ${ogg_INSTALL_DIR}/lib/cmake)

	set (ogg_INSTALL_DIR)
endif ()