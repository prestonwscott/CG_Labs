FetchContent_Declare (
	resources
	URL [[http://fileadmin.cs.lth.se/cs/Education/EDA221/assignments/EDAF80_resources.zip]]
	URL_HASH [[SHA512=1ed9a167e4b06eaa7f0a000881ce7bc315cdc8fd38156cf66adc194f0922d1930ef7591275b37229a0261a84ed1c6869a1b3ac237a3cdca1e8b21d90f484dd11]]
	SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/res"
)

FetchContent_Declare (
	game_resources
	URL [[https://rkgmmg.dm.files.1drv.com/y4my-sTBEwNDaUsAPiRKCo6vUashMq5gAnrEzkLTmU3bm-A2h2bo_7Xv7V7rFKZOqy0qPeayU_gpzmVjrDVX4xpNXWqiDqeGYdfr4VdAj9EvkB48AYdlbXkA5aP4q4Ux65ioBeZtDlF0vfikfOc2IX2WudNRdDZ7LKEkX6etFqAhEYl3jXcjcQvWKpw9rOdXv_0/assets.zip?download&psid=1]]
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
