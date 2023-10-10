#[==[find_package (oggvorbis QUIET)
if (NOT oggvorbis)
	FetchContent_Declare (
		oggvorbis
		GIT_REPOSITORY [[https://github.com/Iunusov/OGG-Vorbis-CMAKE.git]]
		GIT_SHALLOW ON
	)

	FetchContent_GetProperties (oggvorbis)
	if (NOT oggvorbis_POPULATED)
		message (STATUS "Configuring ogg-vorbis codec…")
		FetchContent_Populate (oggvorbis)
	endif ()

	add_library(oggvorbis STATIC)
	link_libraries(oggvorbis STATIC vorbis)
	set (OGG_LIB "${oggvorbis_SOURCE_DIR}")
	
	execute_process (COMMAND ${CMAKE_COMMAND} ${OGG_LIB}
		OUTPUT_VARIABLE stdout
		ERROR_VARIABLE stderr
		RESULT_VARIABLE result
	)
	if (result)
		message (FATAL_ERROR "CMake setup for ogg-vorbis failed: ${result}\n"
		                     "Standard output: ${stdout}\n"
		                     "Error output: ${stderr}")
	endif ()

	list (APPEND CMAKE_PREFIX_PATH ${OGG_LIB})

	#[==[
	add_subdirectory ("${OGG_LIB}/bin")
	set (OGG_BIN "${OGG_LIB}/bin")
	add_subdirectory ("${OGG_BIN}/temp")
	set (OGG_TEMP) "${OGG_BIN}/temp"

	execute_process (
		COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}"
		                         -A "${CMAKE_GENERATOR_PLATFORM}"
		                         -DCMAKE_INSTALL_PREFIX=${OGG_BIN}
		                         -DCMAKE_BUILD_TYPE=Release
		                         ${OGG_LIB}
		OUTPUT_VARIABLE stdout
		ERROR_VARIABLE stderr
		RESULT_VARIABLE result
		WORKING_DIRECTORY ${OGG_TEMP}
	)
	if (result)
		message (FATAL_ERROR "CMake setup for ogg failed: ${result}\n"
		                     "Standard output: ${stdout}\n"
		                     "Error output: ${stderr}")
	endif ()

	execute_process (
		COMMAND ${CMAKE_COMMAND} --build ${OGG_TEMP}
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

	add_library (ogg STATIC)
	target_link_libraries(ogg ${OGG_LIB})
endif ()
]==]