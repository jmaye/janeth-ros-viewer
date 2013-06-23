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

/** \file VelodyneControl.h
    \brief This file defines a Qt control for displaying Velodyne HDL data.
  */

#ifndef VELODYNECONTROL_H
#define VELODYNECONTROL_H

#include <memory>

#include <rosbag/message_instance.h>

#include "gui/palette.h"
#include "gui/view.h"
#include "gui/control.h"

class Ui_VelodyneControl;
class Calibration;

/** The VelodyneControl class represents a Qt control for displaying
    Velodyne HDL data.
    \brief Qt control for displaying Velodyne HDL data.
  */
class VelodyneControl :
  public Control {
Q_OBJECT
  /** \name Private constructors
    @{
    */
  /// Copy constructor
  VelodyneControl(const VelodyneControl& other);
  /// Assignment operator
  VelodyneControl& operator = (const VelodyneControl& other);
  /** @}
    */

public:
  /** \name Constructors/destructor
    @{
    */
  /// Constructs the control
  VelodyneControl(bool showPoints = true);
  /// Destructor
  ~VelodyneControl();
  /** @}
    */

  /** \name Accessors
    @{
    */
  /// Sets the point color
  void setPointColor(const QColor& color);
  /// Sets the point size
  void setPointSize(double pointSize);
  /// Shows the points
  void setShowPoints(bool showPoints);
  /// Smoothes the points
  void setSmoothPoints(bool smoothPoints);
  /// Sets the calibration file name
  void setCalibrationFilename(const QString& filename);
  /// Sets the range support
  void setRangeSupport(double minRange, double maxRange);
  /** @}
    */

protected:
  /** \name Protected methods
    @{
    */
  /// Render the current points
  void renderPoints(View& view, const QColor& color, double size, bool smooth);
  /** @}
    */

  /** \name Protected members
    @{
    */
  /// Qt user interface
  Ui_VelodyneControl* _ui;
  /// Color palette
  Palette _palette;
  /// Velodyne calibration
  std::shared_ptr<Calibration> _calibration;
  /// Min range
  double _minRange;
  /// Max range
  double _maxRange;
  /// Point cloud displaying
  std::vector<Points<double, 3> > _pointCloudsDisp;
  /// Point cloud acquiring
  std::vector<Points<double, 3> > _pointCloudsAcq;
  /// Last start angle
  double _lastStartAngle;
  /// Packet counter for one sensor revolution
  size_t _revolutionPacketCounter;
  /// Origin
  Eigen::Matrix<double, 3, 1> _origin;
  /// Orientation
  Eigen::Matrix<double, 3, 1> _orientation;
  /** @}
    */

protected slots:
  /** \name Qt slots
    @{
    */
  /// Calibration browsing clicked
  void calibrationBrowseClicked();
  /// Color changed
  void colorChanged(const QString& role, const QColor& color);
  /// Point size changed
  void pointSizeChanged(double pointSize);
  /// Show points toggled
  void showPointsToggled(bool checked);
  /// Smooth points toggled
  void smoothPointsToggled(bool checked);
  /// Range support changed
  void rangeSupportChanged();
  /// Render the current view  
  void renderView(View& view);
  /// New message read from the bag file
  void messageRead(const rosbag::MessageInstance& message);
  /// Pose update
  void poseUpdate(double x, double y, double z, double yaw, double pitch,
    double roll);
  /// Clear points clicked
  void clearClicked();
  /** @}
    */


};

#endif // VELODYNECONTROL_H
