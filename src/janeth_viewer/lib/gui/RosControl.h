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

/** \file RosControl.h
    \brief This file defines a ROS listener class for JanETH.
  */

#ifndef ROSCONTROL_H
#define ROSCONTROL_H

#include <QtCore/QTimer>

#include <ros/ros.h>

#include <poslv/VehicleNavigationSolutionMsg.h>
#include <velodyne/BinarySnappyMsg.h>
#include <mv_cameras/ImageSnappyMsg.h>

#include "gui/control.h"

class Ui_RosControl;

/** The RosControl class represents a ROS listener class for JanETH.
    \brief ROS listener for JanETH.
  */
class RosControl :
  public Control {
Q_OBJECT
  /** \name Private constructors
    @{
    */
  /// Copy constructor
  RosControl(const RosControl& other);
  /// Assignment operator
  RosControl& operator = (const RosControl& other);
  /** @}
    */

public:
  /** \name Constructors/destructor
    @{
    */
  /// Constructor
  RosControl(const ros::NodeHandle& nh, const std::vector<std::string>&
    cameraSerials);
  /// Destructor
  ~RosControl();
  /** @}
    */

  /** \name Accessors
    @{
    */
  /** @}
    */

protected:
  /** \name Protected methods
    @{
    */
  /// POS LV callback
  void poslvCallback(const poslv::VehicleNavigationSolutionMsgConstPtr& msg);
  /// Velodyne callback
  void velodyneCallback(const velodyne::BinarySnappyMsgConstPtr& msg);
  /// Camera callback
  void cameraCallback(const mv_cameras::ImageSnappyMsgConstPtr& msg);
  /** @}
    */

  /** \name Protected members
    @{
    */
  /// Qt user interface
  Ui_RosControl* _ui;
  /// ROS node handle
  ros::NodeHandle _nodeHandle;
  /// POS LV data subscriber
  ros::Subscriber _poslvSubscriber;
  /// Velodyne data subscriber
  ros::Subscriber _velodyneSubscriber;
  /// Camera data subscriber
  std::vector<ros::Subscriber> _cameraSubscribers;
  /// Timer for ros spin
  QTimer _timer;
  /** @}
    */

protected slots:
  /** \name Qt slots
    @{
    */
  /// Timeout of the timer
  void timerTimeout();
  /** @}
    */

signals:
  /** \name Qt signals
    @{
    */
  /// POS LV message received
  void messageRead(const poslv::VehicleNavigationSolutionMsgConstPtr& msg);
  /// Velodyne message received
  void messageRead(const velodyne::BinarySnappyMsgConstPtr& msg);
  /// Camera message received
  void messageRead(const mv_cameras::ImageSnappyMsgConstPtr& msg);
  /** @}
    */

};

#endif // ROSCONTROL_H
