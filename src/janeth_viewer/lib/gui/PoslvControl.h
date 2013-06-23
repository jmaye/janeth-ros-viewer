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

/** \file PoslvControl.h
    \brief This file defines a Qt control for displaying Applanix POS LV data.
  */

#ifndef POSLVCONTROL_H
#define POSLVCONTROL_H


#include <rosbag/message_instance.h>

#include "gui/palette.h"
#include "gui/view.h"
#include "gui/control.h"

class Ui_PoslvControl;
class Calibration;

/** The PoslvControl class represents a Qt control for displaying
    Applanix POS LV data.
    \brief Qt control for displaying Applanix POS LV data.
  */
class PoslvControl :
  public Control {
Q_OBJECT
  /** \name Private constructors
    @{
    */
  /// Copy constructor
  PoslvControl(const PoslvControl& other);
  /// Assignment operator
  PoslvControl& operator = (const PoslvControl& other);
  /** @}
    */

public:
  /** \name Constructors/destructor
    @{
    */
  /// Constructs the control
  PoslvControl(bool showPath = true, bool showAxes = true,
    bool showVelocity = true);
  /// Destructor
  ~PoslvControl();
  /** @}
    */

  /** \name Accessors
    @{
    */
  /// Sets the path color
  void setPathColor(const QColor& color);
  /// Shows the path
  void setShowPath(bool showPath);
  /// Sets the axes color
  void setAxesColor(const QColor& color);
  /// Shows the axes
  void setShowAxes(bool showAxes);
  /// Shows the velocity
  void setShowVelocity(bool showVelocity);
  /// Sets the velocity color
  void setVelocityColor(const QColor& color);
  /** @}
    */

protected:
  /** \name Protected methods
    @{
    */
  /// Render the current path
  void renderPath(View& view, const QColor& color);
  /// Render the current axes
  void renderAxes(View& view, const QColor& color, double length);
  /// Render the current velocity
  void renderVelocity(View& view, const QColor& color);
  /** @}
    */

  /** \name Protected members
    @{
    */
  /// Qt user interface
  Ui_PoslvControl* _ui;
  /// Color palette
  Palette _palette;
  /// Path
  std::vector<Points<double, 3>::Point> _path;
  /// Orientation
  Eigen::Matrix<double, 3, 1> _orientation;
  /// Linear velocity
  Eigen::Vector3d _linearVelocity;
  /// Angular velocity
  Eigen::Vector3d _angularVelocity;
  /** @}
    */

protected slots:
  /** \name Qt slots
    @{
    */
  /// Color changed
  void colorChanged(const QString& role, const QColor& color);
  /// Show path toggled
  void showPathToggled(bool checked);
  /// Show axes toggled
  void showAxesToggled(bool checked);
  /// Show velocity toggled
  void showVelocityToggled(bool checked);
  /// Render the current view
  void renderView(View& view);
  /// New message read from the bag file
  void messageRead(const rosbag::MessageInstance& message);
  /// Clear path clicked
  void clearClicked();
  /** @}
    */

signals:
  /** \name Qt signals
    @{
    */
  /// Pose update
  void poseUpdate(double x, double y, double z, double yaw, double pitch,
    double roll);
  /** @}
    */

};

#endif // POSLVCONTROL_H
