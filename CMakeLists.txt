cmake_minimum_required(VERSION 3.16)

project(clean-shell LANGUAGES C)

add_executable(clean-shell main.c
    src/desktop/lingdesktop.h
    src/desktop/lingdesktop.c
    src/desktop/lingappviewpage.h
    src/desktop/lingappviewpage.c
    src/desktop/lingappitem.h
    src/desktop/lingappitem.c
    src/desktop/lingappdrawer.h
    src/desktop/lingappdrawer.c
    src/ling/lingoperate.c
    src/ling/lingoperate.h
    src/ling/lingsysinfo.c
    src/ling/lingsysinfo.h
    src/ling/linggrid.c
    src/ling/linggrid.h
    src/ling/lingviewpager.h
    src/ling/lingviewpager.c
    src/ling/lingdots.c
    src/ling/lingdots.h
    src/ling/lingfixed.c
    src/ling/lingfixed.h
    src/statusbar/lingstatusbar.h
    src/statusbar/lingstatusbar.c
    src/statusbar/lingnotifycenter.h
    src/statusbar/lingnotifycenter.c
    src/ling/notify/lingnotifyitem.c
    src/ling/notify/lingnotifyitem.h
    src/statusbar/lingcontrolcenter.c
    src/statusbar/lingcontrolcenter.h
    src/statusbar/items/lingcontrolscale.h
    src/statusbar/items/lingcontrolscale.c
    src/statusbar/items/lingcontrolmedia.h
    src/statusbar/items/lingcontrolmedia.c
    src/statusbar/items/lingcontrolbutton.h
    src/statusbar/items/lingcontrolbutton.c
    src/statusbar/items/lingwlanlist.h
    src/statusbar/items/lingwlanlist.c
    src/statusbar/items/lingblelist.h
    src/statusbar/items/lingblelist.c
    src/statusbar/items/lingfolder.h
    src/statusbar/items/lingfolder.c
    src/ling/lingdatasaver.c
    src/ling/lingdatasaver.h
    src/ling/lingwallpaper.c
    src/ling/lingwallpaper.h
    src/ling/lingoverlay.c
    src/ling/lingoverlay.h
    src/lockscreen/linglockscreen.c
    src/lockscreen/linglockscreen.h
    src/shell/lingshell.c
    src/shell/lingshell.h
    src/ling/keyboard/lingkeyborad.c
    src/ling/keyboard/lingkeyborad.h
    src/ling/assembly/lingtimeviewer.c
    src/ling/assembly/lingtimeviewer.h
    src/ling/notify/lingnotifyviewer.h
    src/ling/notify/lingnotifyviewer.c
    src/ling/lingsysinfoupdater.c
    src/ling/lingsysinfoupdater.h

    src/desktop/taskswitcher/lingtaskswitcher.c
    src/desktop/taskswitcher/lingtaskswitcher.c
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

# 查找FFmpeg库
# find_package(PkgConfig)
# pkg_search_module(LIBAVCODEC libavcodec)
# pkg_search_module(LIBAVFORMAT libavformat)
# pkg_search_module(LIBAVUTIL libavutil)
# pkg_search_module(LIBSWSCALE libswscale)
# pkg_search_module(LIBAVDEVICE libavdevice)


# 查找ALSA库
# find_package(ALSA REQUIRED)

# 查找SQLite3库
# find_package(SQLite3 REQUIRED)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
# 包含目录
include_directories(${LIBAVCODEC_INCLUDE_DIRS}
                    ${LIBAVFORMAT_INCLUDE_DIRS}
                    ${LIBAVUTIL_INCLUDE_DIRS}
                    ${LIBSWSCALE_INCLUDE_DIRS}
                    ${LIBAVDEVICE_INCLUDE_DIRS}
                    ${LIBNOTIFY_INCLUDE_DIRS}
                    ${DBUS_INCLUDE_DIRS}
                    ${ALSA_INCLUDE_DIRS})

# 链接库
link_directories(${LIBAVCODEC_LIBRARY_DIRS}
                 ${LIBAVFORMAT_LIBRARY_DIRS}
                 ${LIBAVUTIL_LIBRARY_DIRS}
                 ${LIBSWSCALE_LIBRARY_DIRS}
                 ${LIBAVDEVICE_LIBRARY_DIRS}
                 ${LIBNOTIFY_INCLUDE_DIRS}
                 ${DBUS_INCLUDE_DIRS})


target_include_directories(clean-shell PUBLIC ${GTK4_INCLUDE_DIRS})
target_link_libraries(clean-shell PUBLIC ${GTK4_LIBRARIES} #[[${SDL3_LIBRARIES}
${LIBAVCODEC_LIBRARIES} ${LIBAVFORMAT_LIBRARIES} ${LIBAVUTIL_LIBRARIES} ${LIBSWSCALE_LIBRARIES} ${LIBAVDEVICE_LIBRARIES}
${LIBNOTIFY_LIBRARIES}
    ${DBUS_LIBRARIES} ${ALSA_LIBRARIES} PRIVATE SQLite::SQLite3]])

include_directories(./src/)
include_directories(./src/statusbar)
include_directories(./src/statusbar/items)
include_directories(./src/desktop)
include_directories(./src/ling)
include_directories(./src/ling/keyboard)
include_directories(./src/ling/assembly)
include_directories(./src/ling/notify)
include_directories(./src/desktop/taskswitcher)
include_directories(./src/lockscreen)
include_directories(./ui)
include_directories(./src/shell)


file(GLOB MY_RESOURCES "./ui/*.css")

include(GNUInstallDirs)
install(TARGETS clean-shell
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)




