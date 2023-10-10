FetchContent_Declare (
	resources
	URL [[http://fileadmin.cs.lth.se/cs/Education/EDA221/assignments/EDAF80_resources.zip]]
	URL_HASH [[SHA512=1ed9a167e4b06eaa7f0a000881ce7bc315cdc8fd38156cf66adc194f0922d1930ef7591275b37229a0261a84ed1c6869a1b3ac237a3cdca1e8b21d90f484dd11]]
	SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/res"
)

FetchContent_Declare (
	game_resources
	URL [[https://drive.google.com/uc?export=download&id=1e0YcE70xmH61IamBoIfCSN9N34FAB8Lr]]
	SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets"
)

FetchContent_GetProperties (resources)
if (NOT resources_POPULATED)
	message (STATUS "Downloading resource archive…")
	FetchContent_Populate (resources)
endif ()
FetchContent_GetProperties (game_resources)
if (NOT game_resources_POPULATED)
	message (STATUS "Downloading game resource archive…")
	FetchContent_Populate (game_resources)
endif ()
