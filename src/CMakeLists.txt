remake_ros_package(
  janeth_viewer
  DEPENDS rosbag mv_cameras velodyne poslv can_prius
  DESCRIPTION "Viewer for the JanETH project."
  EXTRA_BUILD_DEPENDS libvelodyne-dev libsnappy-dev libposlv-dev
    libposlv-geo-tools-dev libeigen2-dev
  EXTRA_RUN_DEPENDS libvelodyne libsnappy libposlv libposlv-geo-tools
)
