remake_ros_package(
  janeth_viewer
  DEPENDS roscpp rosbag mv_cameras velodyne poslv
  DESCRIPTION "Viewer for the JanETH project."
  EXTRA_BUILD_DEPENDS libvelodyne-dev libsnappy-dev libposlv-dev
    libposlv-geo-tools-dev libeigen2-dev libqt4-opengl-dev libftgl-dev
    libpoppler-qt4-dev pkg-config libeigen3-dev
    ros-${ROS_DISTRIBUTION}-mv-cameras-dev
    ros-${ROS_DISTRIBUTION}-velodyne-dev
    ros-${ROS_DISTRIBUTION}-poslv-dev
  EXTRA_RUN_DEPENDS libvelodyne libsnappy libposlv libposlv-geo-tools
    libqt4-opengl-dev libftgl2 libpoppler-qt4-3
)
