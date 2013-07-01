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

/** \file CameraControl.h
    \brief This file defines a Qt control for displaying camera data.
  */

#ifndef CAMERACONTROL_H
#define CAMERACONTROL_H

#include <rosbag/message_instance.h>

#include <mv_cameras/ImageSnappyMsg.h>

#include "gui/palette.h"
#include "gui/view.h"
#include "gui/control.h"

class Ui_CameraControl;

/** The CameraControl class represents a Qt control for displaying
    camera data.
    \brief Qt control for displaying camera data.
  */
class CameraControl :
  public Control {
Q_OBJECT
  /** \name Private constructors
    @{
    */
  /// Copy constructor
  CameraControl(const CameraControl& other);
  /// Assignment operator
  CameraControl& operator = (const CameraControl& other);
  /** @}
    */

public:
  /** \name Constructors/destructor
    @{
    */
  /// Constructs the control
  CameraControl(const std::string& serial, const Eigen::Affine3d& T_i_c,
    bool showImage = true, bool showAxes = true);
  /// Destructor
  ~CameraControl();
  /** @}
    */

  /** \name Accessors
    @{
    */
  /// Shows the image
  void setShowImage(bool showImage);
  /// Shows the axes
  void setShowAxes(bool showAxes);
  /// Sets axes color
  void setAxesColor(const QColor& color);
  /// Sets the transformation between Velodyne and IMU
  void setTransformation(double tx, double ty, double tz, double rz, double ry,
    double rx);
  /// Sets the rendering rate
  void setRenderingRate(size_t rate);
  /** @}
    */

protected:
  /** \name Protected methods
    @{
    */
  /// Render the current image
  void renderImage(View& view);
  /// Render the current axes
  void renderAxes(View& view, const QColor& color, double length);
  /** @}
    */

  /** \name Protected members
    @{
    */
  /// Qt user interface
  Ui_CameraControl* _ui;
  /// Color palette
  Palette _palette;
  /// Transformation from IMU to world
  Eigen::Affine3d _T_w_i;
  /// Transformation from camera to IMU
  Eigen::Affine3d _T_i_c;
  /// Camera serial
  std::string _serial;
  /// Image width
  size_t _imageWidth;
  /// Image height
  size_t _imageHeight;
  /// Image data
  std::string _imageData;
  /// Image id
  size_t _imageId;
  /// Grayscale color table
  QVector<QRgb> _grayscaleColorTable;
  /** @}
    */

protected slots:
  /** \name Qt slots
    @{
    */
  /// Color changed
  void colorChanged(const QString& role, const QColor& color);
  /// Show image toggled
  void showImageToggled(bool checked);
  /// Render the current view  
  void renderView(View& view);
  /// New message read from the bag file
  void messageRead(const rosbag::MessageInstance& message);
  /// Camera message received
  void messageRead(const mv_cameras::ImageSnappyMsgConstPtr& msg);
  /// Pose update
  void poseUpdate(const Eigen::Affine3d& T_w_i);
  /// Show axes toggled
  void showAxesToggled(bool checked);
  /// Transformation changed
  void transformationChanged();
  /** @}
    */


};

#endif // CAMERACONTROL_H
