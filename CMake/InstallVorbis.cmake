find_package (vorb QUIET ${LUGGCGL_VORBIS_DOWNLOAD_VERSION})
if (NOT vorb)
	FetchContent_Declare (
		vorb
		GIT_REPOSITORY [[https://github.com/xiph/vorbis.git]]
		GIT_TAG "v${LUGGCGL_VORBIS_DOWNLOAD_VERSION}"
		GIT_SHALLOW ON
	)

	FetchContent_GetProperties (vorb)
	if (NOT vorb_POPULATED)
		message (STATUS "Cloning vorbis codec…")
		FetchContent_Populate (vorb)
	endif ()

	set (vorb_INSTALL_DIR "${FETCHCONTENT_BASE_DIR}/vorb-install")
	if (NOT EXISTS "${vorb_INSTALL_DIR}")
		file (MAKE_DIRECTORY ${vorb_INSTALL_DIR})
	endif ()

	message (STATUS "Setting up CMake for vorbis codec…")
	execute_process (
		COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}"
		                         -A "${CMAKE_GENERATOR_PLATFORM}"
		                         -DOGG_ROOT=${CMAKE_CURRENT_SOURCE_DIR}/dependencies/ogg-install
								 -DOGG_DIR=${CMAKE_CURRENT_SOURCE_DIR}/dependencies/ogg-install
		                         -DCMAKE_INSTALL_PREFIX=${vorb_INSTALL_DIR}
		                         -DCMAKE_BUILD_TYPE=Release
		                         ${vorb_SOURCE_DIR}
		OUTPUT_VARIABLE stdout
		ERROR_VARIABLE stderr
		RESULT_VARIABLE result
		WORKING_DIRECTORY ${vorb_BINARY_DIR}
	)
	if (result)
		message (FATAL_ERROR "CMake setup for vorbis failed: ${result}\n"
		                     "Standard output: ${stdout}\n"
		                     "Error output: ${stderr}")
	endif ()

	message (STATUS "Building and installing vorbis…")
	execute_process (
		COMMAND ${CMAKE_COMMAND} --build ${vorb_BINARY_DIR}
		                         --config Release
		                         --target install
		OUTPUT_VARIABLE stdout
		ERROR_VARIABLE stderr
		RESULT_VARIABLE result
	)
	if (result)
		message (FATAL_ERROR "Build step for vorbis failed: ${result}\n"
		                     "Standard output: ${stdout}\n"
		                     "Error output: ${stderr}")
	endif ()

	list (APPEND CMAKE_PREFIX_PATH ${vorb_INSTALL_DIR}/lib/cmake)

	set (vorb_INSTALL_DIR)
endif ()
