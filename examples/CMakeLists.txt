# Find all directories beginning with "example"
file(GLOB EXAMPLE_DIRS
	CMAKE_CONFIGURE_DEPENDS
	LIST_DIRECTORIES true
	RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	"example*"
)

# Add each example to the build
foreach(EXAMPLE_DIR ${EXAMPLE_DIRS})
	# Make sure it's a directory
	if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${EXAMPLE_DIR}")
		# Make sure it has a CMakeLists.txt
		if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${EXAMPLE_DIR}/CMakeLists.txt")
			message("Compiling example: ${EXAMPLE_DIR}")
			add_subdirectory("${EXAMPLE_DIR}")
		endif()
	endif()
endforeach()