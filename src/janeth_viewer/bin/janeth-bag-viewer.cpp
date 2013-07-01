/******************************************************************************
 * Copyright (C) 2013 by Jerome Maye                                          *
 * jerome.maye@gmail.com                                                      *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the Lesser GNU General Public License as published by*
 * the Free Software Foundation; either version 3 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * Lesser GNU General Public License for more details.                        *
 *                                                                            *
 * You should have received a copy of the Lesser GNU General Public License   *
 * along with this program. If not, see <http://www.gnu.org/licenses/>.       *
 ******************************************************************************/

/** \file janeth-bag-viewer.cpp
    \brief This file launches a viewer for BAG files generated by JanETH.
  */

#include "gui/framework.h"
#include "gui/mainwindow.h"
#include "gui/glview.h"
#include "gui/plotview.h"
#include "gui/GraphicsView.h"
#include "gui/scenecontrol.h"
#include "gui/BagControl.h"
#include "gui/PoslvControl.h"
#include "gui/VelodyneControl.h"
#include "gui/CameraControl.h"

int main(int argc, char** argv) {
  Framework framework(argc, argv);
  MainWindow mainWindow("JanETH BAG Viewer");
  mainWindow.addView<GLView>("OpenGL");
  mainWindow.addView<GraphicsView>("GraphicsView");
  mainWindow.addView<PlotView>("GnuPlot");
  mainWindow.addControl<SceneControl>("Scene");
  mainWindow.addControl<BagControl>("Log");
  mainWindow.addControl<PoslvControl>("Applanix POS LV");
  mainWindow.addControl<VelodyneControl>("Velodyne HDL",
    Eigen::Translation3d(1.059, 0, 0.967) *
    Eigen::AngleAxisd(-M_PI / 2.0, Eigen::Vector3d::UnitZ()) *
    Eigen::AngleAxisd(0, Eigen::Vector3d::UnitY()) *
    Eigen::AngleAxisd(0, Eigen::Vector3d::UnitX()));
  // front global camera
  mainWindow.addControl<CameraControl>("MV GX002408 - Front global", "GX002408",
    Eigen::Translation3d(1.635, 0, 0.967) *
    Eigen::AngleAxisd(-M_PI / 2.0, Eigen::Vector3d::UnitZ()) *
    Eigen::AngleAxisd(0, Eigen::Vector3d::UnitY()) *
    Eigen::AngleAxisd(-M_PI / 2.0, Eigen::Vector3d::UnitX()));
  // front rolling camera
  mainWindow.addControl<CameraControl>("MV GX002538 - Front rolling",
    "GX002538",
    Eigen::Translation3d(1.635, 0, 0.967) *
    Eigen::AngleAxisd(-M_PI / 2.0, Eigen::Vector3d::UnitZ()) *
    Eigen::AngleAxisd(0, Eigen::Vector3d::UnitY()) *
    Eigen::AngleAxisd(-M_PI / 2.0, Eigen::Vector3d::UnitX()));
  // right global camera
  mainWindow.addControl<CameraControl>("MV GX002537 - Right global", "GX002537",
    Eigen::Translation3d(0.462, -0.59, 0.967) *
    Eigen::AngleAxisd(0, Eigen::Vector3d::UnitZ()) *
    Eigen::AngleAxisd(M_PI, Eigen::Vector3d::UnitY()) *
    Eigen::AngleAxisd(M_PI / 2.0, Eigen::Vector3d::UnitX()));
  // right rolling camera
  mainWindow.addControl<CameraControl>("MV GX002541 - Right rolling",
    "GX002541",
    Eigen::Translation3d(0.462, -0.59, 0.967) *
    Eigen::AngleAxisd(0, Eigen::Vector3d::UnitZ()) *
    Eigen::AngleAxisd(M_PI, Eigen::Vector3d::UnitY()) *
    Eigen::AngleAxisd(M_PI / 2.0, Eigen::Vector3d::UnitX()));
  // back global camera
  mainWindow.addControl<CameraControl>("MV GX002409 - Back global", "GX002409",
    Eigen::Translation3d(0.332, 0, 0.967) *
    Eigen::AngleAxisd(-M_PI / 2.0, Eigen::Vector3d::UnitZ()) *
    Eigen::AngleAxisd(M_PI, Eigen::Vector3d::UnitY()) *
    Eigen::AngleAxisd(M_PI / 2.0, Eigen::Vector3d::UnitX()));
  // back rolling camera
  mainWindow.addControl<CameraControl>("MV GX002540 - Back rolling", "GX002540",
    Eigen::Translation3d(0.332, 0, 0.967) *
    Eigen::AngleAxisd(-M_PI / 2.0, Eigen::Vector3d::UnitZ()) *
    Eigen::AngleAxisd(M_PI, Eigen::Vector3d::UnitY()) *
    Eigen::AngleAxisd(M_PI / 2.0, Eigen::Vector3d::UnitX()));
  // left global camera
  mainWindow.addControl<CameraControl>("MV GX002536 - Left global", "GX002536",
    Eigen::Translation3d(0.462, 0.59, 0.967) *
    Eigen::AngleAxisd(0, Eigen::Vector3d::UnitZ()) *
    Eigen::AngleAxisd(0, Eigen::Vector3d::UnitY()) *
    Eigen::AngleAxisd(-M_PI / 2.0, Eigen::Vector3d::UnitX()));
  // left rolling camera
  mainWindow.addControl<CameraControl>("MV GX002539 - Left rolling", "GX002539",
    Eigen::Translation3d(0.462, 0.59, 0.967) *
    Eigen::AngleAxisd(0, Eigen::Vector3d::UnitZ()) *
    Eigen::AngleAxisd(0, Eigen::Vector3d::UnitY()) *
    Eigen::AngleAxisd(-M_PI / 2.0, Eigen::Vector3d::UnitX()));
  mainWindow.show();
  return framework.exec();
}
