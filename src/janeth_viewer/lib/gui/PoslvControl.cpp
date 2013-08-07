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

#include <libposlv/sensor/Utils.h>
#include <libposlv/geo-tools/Geo.h>

#include "gui/BagControl.h"
#include "gui/RosControl.h"

#include "ui_PoslvControl.h"

/******************************************************************************/
/* Constructors and Destructor                                                */
/******************************************************************************/

PoslvControl::PoslvControl(bool showPath, bool showAxes, bool showVelocity,
    bool showAcceleration) :
    _ui(new Ui_PoslvControl()),
    _path(0),
    _linearVelocity(Eigen::Vector3d::Zero()),
    _angularVelocity(Eigen::Vector3d::Zero()),
    _acceleration(Eigen::Vector3d::Zero()),
    _T_w_i(Eigen::AngleAxisd(0, Eigen::Vector3d::UnitZ())
      * Eigen::AngleAxisd(0, Eigen::Vector3d::UnitY())
      * Eigen::AngleAxisd(0, Eigen::Vector3d::UnitX())
      * Eigen::Translation3d(0, 0, 0)) {
  _ui->setupUi(this);
  _ui->colorChooser->setPalette(&_palette);
  connect<BagControl>(SIGNAL(messageRead(const rosbag::MessageInstance&)),
    SLOT(messageRead(const rosbag::MessageInstance&)));
  connect<RosControl>(
    SIGNAL(messageRead(const poslv::VehicleNavigationSolutionMsgConstPtr&)),
    SLOT(messageRead(const poslv::VehicleNavigationSolutionMsgConstPtr&)));
  connect<View>(SIGNAL(render(View&)), SLOT(renderView(View&)));
  setPathColor(Qt::red);
  setShowPath(showPath);
  setAxesColor(Qt::green);
  setShowAxes(showAxes);
  setShowVelocity(showVelocity);
  setVelocityColor(Qt::cyan);
  setShowAcceleration(showAcceleration);
  setAccelerationColor(Qt::cyan);
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

void PoslvControl::setAccelerationColor(const QColor& color) {
  _palette.setColor("Acceleration", color);
}

void PoslvControl::setShowAcceleration(bool showAcceleration) {
  _ui->showAccelerationCheckBox->setChecked(showAcceleration);
  emit updateViews();
}

void PoslvControl::setRenderingRate(size_t rate) {
  _ui->rateSpinBox->setValue(rate);
}

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

void PoslvControl::renderPath(View& view, const QColor& color) {
  view.render(_path, color);
}

void PoslvControl::renderAxes(View& view, const QColor& color, double length) {
  Line<double, 3> l_1, l_2, l_3;
  l_1[1][0] = length;
  view.render(l_1, color, _T_w_i);
  l_2[1][1] = length;
  view.render(l_2, color, _T_w_i);
  l_3[1][2] = length;
  view.render(l_3, color, _T_w_i);
  Eigen::Vector3d xLabelPosition = _T_w_i * l_1[1];
  Eigen::Vector3d yLabelPosition = _T_w_i * l_2[1];
  Eigen::Vector3d zLabelPosition = _T_w_i * l_3[1];
  view.render("X", xLabelPosition, color, 0.2*length);
  view.render("Y", yLabelPosition, color, 0.2*length);
  view.render("Z", zLabelPosition, color, 0.2*length);
  Eigen::Vector3d labelPosition = _T_w_i * Eigen::Vector3d(0, 0, length + 0.1);
  view.render("poslv", labelPosition, color, 0.2 * length);
}

void PoslvControl::renderVelocity(View& view, const QColor& color) {
  if (_path.getNumPoints()) {
    Line<double, 3> linearVelocity;
    Line<double, 3> angularVelocity;
    Eigen::Affine3d translation;
    translation = Eigen::Translation3d(_T_w_i.translation());
    linearVelocity[1] = _linearVelocity;
    view.render(linearVelocity, color, translation);
    Eigen::Vector3d labelPosition = translation * linearVelocity[1];
    view.render("v", labelPosition, color, 0.05 * 2.5);
    angularVelocity[1] = _T_w_i.rotation() * _angularVelocity;
    view.render(angularVelocity, color, _T_w_i);
    view.render("om", _T_w_i * angularVelocity[1], color, 0.2 * 0.5);
  }
}

void PoslvControl::renderAcceleration(View& view, const QColor& color) {
  if (_path.getNumPoints()) {
    Line<double, 3> acceleration;
    acceleration[1] = _T_w_i.rotation() * _acceleration;
    view.render(acceleration, color, _T_w_i);
    view.render("a", _T_w_i * acceleration[1], color, 0.2 * 0.5);
  }
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

void PoslvControl::showAccelerationToggled(bool checked) {
  setShowAcceleration(checked);
}

void PoslvControl::renderView(View& view) {
  if (_ui->showPathCheckBox->isChecked())
    renderPath(view, _palette.getColor("Path"));
  if (_ui->showAxesCheckBox->isChecked())
    renderAxes(view, _palette.getColor("Axes"), 0.5);
  if (_ui->showVelocityCheckBox->isChecked())
    renderVelocity(view, _palette.getColor("Velocity"));
  if (_ui->showAccelerationCheckBox->isChecked())
    renderAcceleration(view, _palette.getColor("Acceleration"));
}

void PoslvControl::messageRead(
    const poslv::VehicleNavigationSolutionMsgConstPtr& msg) {
  static bool firstVNS = true;
  static double latRef;
  static double longRef;
  static double altRef;
  if (firstVNS) {
    latRef = msg->latitude;
    longRef = msg->longitude;
    altRef = msg->altitude;
    firstVNS = false;
  }
  double x_ecef, y_ecef, z_ecef;
  Geo::wgs84ToEcef(msg->latitude, msg->longitude, msg->altitude, x_ecef,
    y_ecef, z_ecef);
  double x_enu, y_enu, z_enu;
  Geo::ecefToEnu(x_ecef, y_ecef, z_ecef, latRef, longRef, altRef, x_enu,
    y_enu, z_enu);
  Eigen::Vector3d orientation =
    Eigen::Vector3d(Utils::deg2rad(-msg->heading) + M_PI / 2.0,
    Utils::deg2rad(-msg->pitch), Utils::deg2rad(msg->roll));
  _linearVelocity = Geo::R_ENU_NED::getInstance().getMatrix() *
    Eigen::Vector3d(msg->northVelocity, msg->eastVelocity, msg->downVelocity);
  _angularVelocity = Eigen::Vector3d(Utils::deg2rad(msg->angularRateLong),
    Utils::deg2rad(-msg->angularRateTrans),
    Utils::deg2rad(-msg->angularRateDown));
  _acceleration = Eigen::Vector3d(msg->accLong, -msg->accTrans, -msg->accDown);
  _T_w_i = Eigen::Translation3d(x_enu, y_enu, z_enu)
    * Eigen::AngleAxisd(orientation(0), Eigen::Vector3d::UnitZ())
    * Eigen::AngleAxisd(orientation(1), Eigen::Vector3d::UnitY())
    * Eigen::AngleAxisd(orientation(2), Eigen::Vector3d::UnitX());
  emit poseUpdate(_T_w_i);
  static size_t updateCount = 0;
  updateCount++;
  if (updateCount >= _ui->rateSpinBox->value()) {
    _path += Points<double, 3>::Point(x_enu, y_enu, z_enu);
    emit updateViews();
    updateCount = 0;
  }
}

void PoslvControl::messageRead(const rosbag::MessageInstance& message) {
  if (message.isType<poslv::VehicleNavigationSolutionMsg>()) {
    poslv::VehicleNavigationSolutionMsgPtr vns(
      message.instantiate<poslv::VehicleNavigationSolutionMsg>());
    messageRead(vns);
  }
}

void PoslvControl::clearClicked() {
  _path.clear();
  _linearVelocity = Eigen::Vector3d::Zero();
  _angularVelocity = Eigen::Vector3d::Zero();
  _acceleration = Eigen::Vector3d::Zero();
  _T_w_i = Eigen::AngleAxisd(0, Eigen::Vector3d::UnitZ())
    * Eigen::AngleAxisd(0, Eigen::Vector3d::UnitY())
    * Eigen::AngleAxisd(0, Eigen::Vector3d::UnitX())
    * Eigen::Translation3d(0, 0, 0);
  emit updateViews();
}
