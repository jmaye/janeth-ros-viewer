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

/** \file BagControl.h
    \brief This file defines a Qt control for playing ROS bag files.
  */

#ifndef BAGCONTROL_H
#define BAGCONTROL_H

#include <memory>

#include <QtCore/QTimer>

#include <rosbag/bag.h>
#include <rosbag/view.h>
#include <rosbag/message_instance.h>

#include "gui/control.h"

class Ui_BagControl;

/** The BagControl class represents a Qt control for playing ROS bag files.
    \brief Qt control for playing ROS bag files.
  */
class BagControl :
  public Control {
Q_OBJECT
  /** \name Private constructors
    @{
    */
  /// Copy constructor
  BagControl(const BagControl& other);
  /// Assignment operator
  BagControl& operator = (const BagControl& other);
  /** @}
    */

public:
  /** \name Constructors/destructor
    @{
    */
  /// Constructs the control
  BagControl();
  /// Destructor
  ~BagControl();
  /** @}
    */

  /** \name Accessors
    @{
    */
  /// Sets the log file name to play
  void setLogFilename(const QString& filename);
  /** @}
    */

protected:
  /** \name Protected members
    @{
    */
  /// Qt user interface
  Ui_BagControl* _ui;
  /// Timer controlling the reading of the bag file
  QTimer _timer;
  /// ROS bag reader
  std::shared_ptr<rosbag::Bag> _bag;
  /// ROS bag view
  std::shared_ptr<rosbag::View> _view;
  /// Begin iterator of the bag file
  std::shared_ptr<rosbag::View::iterator> _bagBeginIt;
  /// End iterator of the bag file
  std::shared_ptr<rosbag::View::iterator> _bagEndIt;
  /// Current iterator of the bag file
  std::shared_ptr<rosbag::View::iterator> _bagCurrIt;
  /// Message count
  size_t _msgCnt;
  /** @}
    */

protected slots:
  /** \name Qt slots
    @{
    */
  /// Browse for log files
  void logBrowseClicked();
  /// Log play toggled
  void logPlayToggled();
  /// Log stop clicked
  void logStopClicked();
  /// Log forward clicked
  void logForwardClicked();
  /// Timeout of the timer
  void timerTimeout();
  /** @}
    */

signals:
  /** \name Qt signals
    @{
    */
  /// New message read from the bag file
  void messageRead(const rosbag::MessageInstance& message);
  /** @}
    */

};

#endif // BAGCONTROL_H
