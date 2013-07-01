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

#include "gui/RosControl.h"

#include "ui_RosControl.h"

/******************************************************************************/
/* Constructors and Destructor                                                */
/******************************************************************************/

RosControl::RosControl(const ros::NodeHandle& nh,
    const std::vector<std::string>& cameraSerials) :
    _ui(new Ui_RosControl()),
    _nodeHandle(nh) {
  _ui->setupUi(this);
  _poslvSubscriber = _nodeHandle.subscribe("/poslv/vehicle_navigation_solution",
    1000, &RosControl::poslvCallback, this);
  _velodyneSubscriber = _nodeHandle.subscribe("/velodyne/binary_snappy",
    1000, &RosControl::velodyneCallback, this);
  _cameraSubscribers.reserve(cameraSerials.size());
  for (auto it = cameraSerials.cbegin(); it != cameraSerials.cend(); ++it)
    _cameraSubscribers.push_back(
      _nodeHandle.subscribe("/mv_cameras_manager/" + *it + "/image_snappy",
      1000, &RosControl::cameraCallback, this));
  _timer.setSingleShot(true);
  connect(&_timer, SIGNAL(timeout()), this, SLOT(timerTimeout()));
  _timer.start(0);
}

RosControl::~RosControl() {
  delete _ui;
}

/******************************************************************************/
/* Accessors                                                                  */
/******************************************************************************/

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

void RosControl::poslvCallback(
    const poslv::VehicleNavigationSolutionMsgConstPtr& msg) {
  emit messageRead(msg);
}

void RosControl::velodyneCallback(
    const velodyne::BinarySnappyMsgConstPtr& msg) {
  emit messageRead(msg);
}

void RosControl::cameraCallback(const mv_cameras::ImageSnappyMsgConstPtr& msg) {
  emit messageRead(msg);
}

void RosControl::timerTimeout() {
  _timer.start(0);
  ros::spinOnce();
}
