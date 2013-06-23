remake_find_package(freetype2 CONFIG)
remake_find_package(ftgl CONFIG)
remake_find_package(poppler-qt4 CONFIG)
remake_find_package(libvelodyne CONFIG)
remake_find_package(libsnappy CONFIG)
remake_find_file(eigen2/Eigen/Core PACKAGE libeigen2-dev)
remake_find_package(libposlv CONFIG)
remake_find_package(libposlv-geo-tools CONFIG)

remake_include(${LIBPOSLV_INCLUDE_DIRS})
remake_include(${LIBPOSLV_GEO_TOOLS_INCLUDE_DIRS})
remake_include(${LIBEIGEN2_DEV_PATH}/eigen2)
remake_include(${LIBVELODYNE_INCLUDE_DIRS})
remake_include(${LIBSNAPPY_INCLUDE_DIRS})
remake_include(${FREETYPE2_INCLUDE_DIRS})

remake_qt4_ui(gui)
remake_qt4_moc(gui)

remake_add_library(gui LINK ${QT_QTGUI_LIBRARY} ${QT_QTOPENGL_LIBRARY}
  ${FTGL_LIBRARIES} ${POPPLER_QT4_LIBRARIES} ${LIBVELODYNE_LIBRARIES}
  ${LIBSNAPPY_LIBRARIES} ${LIBPOSLV_LIBRARIES} ${LIBPOSLV_GEO_TOOLS_LIBRARIES})
remake_add_headers(INSTALL gui)
