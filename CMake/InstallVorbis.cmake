find_package (vorbis QUIET ${LUGGCGL_VORBIS_DOWNLOAD_VERSION})
if (NOT vorbis)
	FetchContent_Declare (
		vorbis
		GIT_REPOSITORY [[https://github.com/xiph/vorbis.git]]
		GIT_TAG "v${LUGGCGL_VORBIS_DOWNLOAD_VERSION}"
		GIT_SHALLOW ON
	)

	FetchContent_GetProperties (vorbis)
	if (NOT vorbis_POPULATED)
		message (STATUS "Cloning vorbis codec…")
		FetchContent_Populate (vorbis)
	endif ()

	set (vorbis_INSTALL_DIR "${FETCHCONTENT_BASE_DIR}/vorbis-install")
	if (NOT EXISTS "${vorbis_INSTALL_DIR}")
		file (MAKE_DIRECTORY ${vorbis_INSTALL_DIR})
	endif ()

	message (STATUS "Setting up CMake for vorbis codec…")
	execute_process (
		COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}"
		                         -A "${CMAKE_GENERATOR_PLATFORM}"
		                         -DOGG_ROOT=${CMAKE_CURRENT_SOURCE_DIR}/dependencies/ogg-install
								 -DOGG_DIR=${CMAKE_CURRENT_SOURCE_DIR}/dependencies/ogg-build
		                         -DCMAKE_INSTALL_PREFIX=${vorbis_INSTALL_DIR}
		                         -DCMAKE_BUILD_TYPE=Release
		                         ${vorbis_SOURCE_DIR}
		OUTPUT_VARIABLE stdout
		ERROR_VARIABLE stderr
		RESULT_VARIABLE result
		WORKING_DIRECTORY ${vorbis_BINARY_DIR}
	)
	if (result)
		message (FATAL_ERROR "CMake setup for vorbis failed: ${result}\n"
		                     "Standard output: ${stdout}\n"
		                     "Error output: ${stderr}")
	endif ()

	message (STATUS "Building and installing vorbis…")
	execute_process (
		COMMAND ${CMAKE_COMMAND} --build ${vorbis_BINARY_DIR}
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

	list (APPEND CMAKE_PREFIX_PATH ${vorbis_INSTALL_DIR}/lib/cmake)

	set (vorbis_INSTALL_DIR)

	add_library( vorbis::vorbis INTERFACE IMPORTED)
	set_target_properties(vorbis::vorbis PROPERTIES
		INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/vorbis-install/include"
	)
endif ()
