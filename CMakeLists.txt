cmake_minimum_required(VERSION 3.16)

project(clean-shell LANGUAGES C)

# 自动添加源文件
file(GLOB_RECURSE SRC_FILES "src/*.cpp" "src/*.c")

# 自动添加头文件路径
file(GLOB_RECURSE HDR_FILES "include/*.hpp" "include/*.h")
foreach(HDR_FILE IN LISTS HDR_FILES)
    get_filename_component(DIR ${HDR_FILE} DIRECTORY)
    include_directories(${DIR})
endforeach()

add_executable(${PROJECT_NAME}
    ${SRC_FILES}
)

# 添加CSS资源文件
set(CSS_FILES
    ${CMAKE_SOURCE_DIR}/ui/style.css
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)
pkg_check_modules(LIBNOTIFY REQUIRED libnotify)
pkg_check_modules(DBUS REQUIRED dbus-1)
include_directories(${GTK4_INCLUDE_DIRS})
link_directories(${GTK4_LIBRARY_DIRS})

pkg_check_modules(SDL3 REQUIRED sdl3)
include_directories(${SDL_INCLUDE_DIRS})
link_directories(${SDL_LIBRARY_DIRS})

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

target_include_directories(${PROJECT_NAME} PUBLIC ${GTK4_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${GTK4_LIBRARIES})

include_directories(./src/)
include_directories(./src/shell-base)
include_directories(./src/shell-base/wm)
include_directories(./src/shell-base/statusbar)
include_directories(./src/shell-base/statusbar/items)
include_directories(./src/mobile-shell)
include_directories(./src/libling)
include_directories(./src/libling/keyboard)
include_directories(./src/libling/assembly)
include_directories(./src/libling/notify)
include_directories(./src/shell-base/lockscreen)
include_directories(./ui)


file(GLOB MY_RESOURCES "./ui/*.css")

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)




