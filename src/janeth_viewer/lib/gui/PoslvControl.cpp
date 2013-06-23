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

#include "gui/PoslvControl.h"

#include <eigen3/Eigen/Geometry>

#include <libposlv/sensor/Utils.h>
#include <libposlv/geo-tools/Geo.h>

#include <poslv/VehicleNavigationSolutionMsg.h>

#include "gui/BagControl.h"

#include "ui_PoslvControl.h"

/******************************************************************************/
/* Constructors and Destructor                                                */
/******************************************************************************/

PoslvControl::PoslvControl(bool showPath, bool showAxes, bool showVelocity) :
    _ui(new Ui_PoslvControl()),
    _orientation(0.0, 0.0, 0.0),
    _linearVelocity(0.0, 0.0, 0.0),
    _angularVelocity(0.0, 0.0, 0.0) {
  _ui->setupUi(this);
  _ui->colorChooser->setPalette(&_palette);
  connect<BagControl>(SIGNAL(messageRead(const rosbag::MessageInstance&)),
    SLOT(messageRead(const rosbag::MessageInstance&)));
  connect<View>(SIGNAL(render(View&)), SLOT(renderView(View&)));
  setPathColor(Qt::red);
  setShowPath(showPath);
  setAxesColor(Qt::green);
  setShowAxes(showAxes);
  setShowVelocity(showVelocity);
  setVelocityColor(Qt::cyan);
}

PoslvControl::~PoslvControl() {
  delete _ui;
}

/******************************************************************************/
/* Accessors                                                                  */
/******************************************************************************/

void PoslvControl::setPathColor(const QColor& color) {
  _palette.setColor("Path", color);
}

void PoslvControl::setShowPath(bool showPath) {
  _ui->showPathCheckBox->setChecked(showPath);
  emit updateViews();
}

void PoslvControl::setAxesColor(const QColor& color) {
  _palette.setColor("Axes", color);
}

void PoslvControl::setShowAxes(bool showAxes) {
  _ui->showAxesCheckBox->setChecked(showAxes);
  emit updateViews();
}

void PoslvControl::setVelocityColor(const QColor& color) {
  _palette.setColor("Velocity", color);
}

void PoslvControl::setShowVelocity(bool showVelocity) {
  _ui->showVelocityCheckBox->setChecked(showVelocity);
  emit updateViews();
}

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

void PoslvControl::renderPath(View& view, const QColor& color) {
  Line<double, 3> line(_path.size());
  for (size_t i = 0; i < _path.size(); ++i)
    line[i] = _path[i];
  view.render(line, color);
}

void PoslvControl::renderAxes(View& view, const QColor& color, double length) {
  Line<double, 3> l_1, l_2, l_3;

  Points<double, 3>::Point origin = Points<double, 3>::Point::Zero();

  Eigen::Matrix3d C_w_NED_i_NED;
  C_w_NED_i_NED = Eigen::AngleAxisd(_orientation(0), Eigen::Vector3d::UnitZ())
   * Eigen::AngleAxisd(_orientation(1), Eigen::Vector3d::UnitY())
   * Eigen::AngleAxisd(_orientation(2), Eigen::Vector3d::UnitX());
  Eigen::Matrix3d C_ENU_NED =
    Geo::R_ENU_NED::getInstance().getMatrix();
  const Eigen::Matrix3d C_w_ENU_i_ENU = C_w_NED_i_NED * C_ENU_NED;

  if (!_path.empty())
    origin = _path.back();

  const Eigen::Matrix<double, 3, 1> xLabelOrigin = origin +
    C_w_ENU_i_ENU * Eigen::Matrix<double, 3, 1>(length, 0, 0);
  const Eigen::Matrix<double, 3, 1> yLabelOrigin = origin +
    C_w_ENU_i_ENU * Eigen::Matrix<double, 3, 1>(0, length, 0);
  const Eigen::Matrix<double, 3, 1> zLabelOrigin = origin +
    C_w_ENU_i_ENU * Eigen::Matrix<double, 3, 1>(0, 0, length);
  view.render("X", xLabelOrigin(0), xLabelOrigin(1), xLabelOrigin(2), color,
    0.2*length);
  view.render("Y", yLabelOrigin(0), yLabelOrigin(1), yLabelOrigin(2), color,
    0.2*length);
  view.render("Z", zLabelOrigin(0), zLabelOrigin(1), zLabelOrigin(2), color,
    0.2*length);

  l_1[0] = origin;
  l_1[1] = xLabelOrigin;
  view.render(l_1, color);
  l_2[0] = origin;
  l_2[1] = yLabelOrigin;
  view.render(l_2, color);
  l_3[0] = origin;
  l_3[1] = zLabelOrigin;
  view.render(l_3, color);

  Eigen::Matrix<double, 3, 1> labelOrigin = origin +
    C_w_ENU_i_ENU * Eigen::Matrix<double, 3, 1>(0, 0, length + 0.5);
  view.render("poslv", labelOrigin(0), labelOrigin(1), labelOrigin(2), color,
    0.2 * length);
}

void PoslvControl::renderVelocity(View& view, const QColor& color) {
  Line<double, 3> linearVelocity;
  Line<double, 3> angularVelocity;
  Points<double, 3>::Point origin = Points<double, 3>::Point::Zero();
  if (!_path.empty())
    origin = _path.back();
  linearVelocity.setOrigin(origin);
  angularVelocity.setOrigin(origin);
  Eigen::Matrix3d C_ENU_NED =
    Geo::R_ENU_NED::getInstance().getMatrix();
  linearVelocity[1] = C_ENU_NED * _linearVelocity;
  view.render(linearVelocity, color);
  view.render("v", origin + linearVelocity[1], color, 0.2 * 2.5);
  angularVelocity[1] = C_ENU_NED * _angularVelocity;
  view.render(angularVelocity, color);
  view.render("om", origin + angularVelocity[1], color, 0.2 * 2.5);
}

void PoslvControl::colorChanged(const QString& role, const QColor& color) {
  emit updateViews();
}

void PoslvControl::showPathToggled(bool checked) {
  setShowPath(checked);
}

void PoslvControl::showAxesToggled(bool checked) {
  setShowAxes(checked);
}

void PoslvControl::showVelocityToggled(bool checked) {
  setShowVelocity(checked);
}

void PoslvControl::renderView(View& view) {
  if (_ui->showPathCheckBox->isChecked())
    renderPath(view, _palette.getColor("Path"));
  if (_ui->showAxesCheckBox->isChecked())
    renderAxes(view, _palette.getColor("Axes"), 2.5);
  if (_ui->showVelocityCheckBox->isChecked())
    renderVelocity(view, _palette.getColor("Velocity"));
}

void PoslvControl::messageRead(const rosbag::MessageInstance& message) {
  if (message.isType<poslv::VehicleNavigationSolutionMsg>()) {
    poslv::VehicleNavigationSolutionMsgPtr vns(
      message.instantiate<poslv::VehicleNavigationSolutionMsg>());
    static bool firstVNS = true;
    static double latRef;
    static double longRef;
    static double altRef;
    if (firstVNS) {
      latRef = vns->latitude;
      longRef = vns->longitude;
      altRef = vns->altitude;
      firstVNS = false;
    }
    double x_ecef, y_ecef, z_ecef;
    Geo::wgs84ToEcef(vns->latitude, vns->longitude, vns->altitude, x_ecef,
      y_ecef, z_ecef);
    double x_enu, y_enu, z_enu;
    Geo::ecefToEnu(x_ecef, y_ecef, z_ecef, latRef, longRef, altRef, x_enu,
      y_enu, z_enu);
    _path.push_back(Points<double, 3>::Point(x_enu, y_enu, z_enu));
    _orientation = Eigen::Matrix<double, 3, 1>(-Utils::deg2rad(vns->heading),
      Utils::deg2rad(-vns->pitch), Utils::deg2rad(-vns->roll));
    emit poseUpdate(x_enu, y_enu, z_enu,
      _orientation(0), _orientation(1), _orientation(2));
    _linearVelocity = Eigen::Vector3d(vns->northVelocity, vns->eastVelocity,
      vns->downVelocity);
    _angularVelocity = Eigen::Vector3d(Utils::deg2rad(-vns->angularRateLong),
      Utils::deg2rad(-vns->angularRateTrans),
      Utils::deg2rad(-vns->angularRateDown));
    static size_t updateCount = 0;
    updateCount++;
    if (updateCount == 10) {
      emit updateViews();
      updateCount = 0;
    }
  }
}

void PoslvControl::clearClicked() {
  _path.clear();
  _orientation = Eigen::Vector3d::Zero();
  _linearVelocity = Eigen::Vector3d::Zero();
  _angularVelocity = Eigen::Vector3d::Zero();
  emit updateViews();
}
