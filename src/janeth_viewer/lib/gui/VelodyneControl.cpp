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

#include "gui/VelodyneControl.h"

#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>

#include <libvelodyne/sensor/Calibration.h>
#include <libvelodyne/sensor/DataPacket.h>
#include <libvelodyne/sensor/Converter.h>
#include <libvelodyne/data-structures/VdynePointCloud.h>
#include <libvelodyne/exceptions/IOException.h>

#include <libsnappy/snappy.h>

#include "gui/BagControl.h"
#include "gui/PoslvControl.h"
#include "gui/RosControl.h"

#include "ui_VelodyneControl.h"

/******************************************************************************/
/* Constructors and Destructor                                                */
/******************************************************************************/

VelodyneControl::VelodyneControl(const Eigen::Affine3d& T_i_v, bool showPoints,
    bool showAxes) :
    _ui(new Ui_VelodyneControl()),
    _minRange(Converter::mMinDistance),
    _maxRange(Converter::mMaxDistance),
    _lastStartAngle(0),
    _revolutionPacketCounter(0),
    _T_w_i(Eigen::Translation3d(0, 0, 0)
      * Eigen::AngleAxisd(0, Eigen::Vector3d::UnitZ())
      * Eigen::AngleAxisd(0, Eigen::Vector3d::UnitY())
      * Eigen::AngleAxisd(0, Eigen::Vector3d::UnitX())),
    _T_i_v(T_i_v) {
  _ui->setupUi(this);
  _ui->colorChooser->setPalette(&_palette);
  connect(&_palette, SIGNAL(colorChanged(const QString&, const QColor&)),
    this, SLOT(colorChanged(const QString&, const QColor&)));
  connect<View>(SIGNAL(render(View&)), SLOT(renderView(View&)));
  connect<BagControl>(SIGNAL(messageRead(const rosbag::MessageInstance&)),
    SLOT(messageRead(const rosbag::MessageInstance&)));
  connect<RosControl>(
    SIGNAL(messageRead(const velodyne::BinarySnappyMsgConstPtr&)),
    SLOT(messageRead(const velodyne::BinarySnappyMsgConstPtr&)));
  connect<PoslvControl>(SIGNAL(poseUpdate(const Eigen::Affine3d&)),
    SLOT(poseUpdate(const Eigen::Affine3d&)));
  setPointColor(Qt::gray);
  setPointSize(1.0);
  setShowPoints(showPoints);
  setSmoothPoints(true);
  setCalibrationFilename("/etc/libvelodyne/calib-HDL-64E.dat");
  setRangeSupport(_ui->minRangeSpinBox->value(), _ui->maxRangeSpinBox->value());
  setAxesColor(Qt::yellow);
  setShowAxes(showAxes);
}

VelodyneControl::~VelodyneControl() {
  delete _ui;
}

/******************************************************************************/
/* Accessors                                                                  */
/******************************************************************************/

void VelodyneControl::setPointColor(const QColor& color) {
  _palette.setColor("Point", color);
}

void VelodyneControl::setPointSize(double pointSize) {
  _ui->pointSizeSpinBox->setValue(pointSize);
  emit updateViews();
}

void VelodyneControl::setShowPoints(bool showPoints) {
  _ui->showPointsCheckBox->setChecked(showPoints);
  emit updateViews();
}

void VelodyneControl::setSmoothPoints(bool smoothPoints) {
  _ui->smoothPointsCheckBox->setChecked(smoothPoints);
  emit updateViews();
}

void VelodyneControl::setCalibrationFilename(const QString& filename) {
  _ui->calibrationEdit->setText(filename);
  QFileInfo fileInfo(filename);
  if (fileInfo.isFile() && fileInfo.isReadable()) {
    std::ifstream calibFile(filename.toStdString());
    _calibration.reset(new Calibration());
    try {
      calibFile >> *_calibration;
    }
    catch (const IOException& e) {
      QMessageBox::information(this, "VelodyneControl",
        tr("Exception: %1.").arg(e.what()));
    }
  }
}

void VelodyneControl::setRangeSupport(double minRange, double maxRange) {
  _minRange = minRange;
  _maxRange = maxRange;
  _ui->minRangeSpinBox->setValue(_minRange);
  _ui->maxRangeSpinBox->setValue(_maxRange);
}

void VelodyneControl::setAxesColor(const QColor& color) {
  _palette.setColor("Axes", color);
}

void VelodyneControl::setShowAxes(bool showAxes) {
  _ui->showAxesCheckBox->setChecked(showAxes);
  emit updateViews();
}

void VelodyneControl::setTransformation(double tx, double ty, double tz,
    double rz, double ry, double rx) {
  _ui->txSpinBox->setValue(tx);
  _ui->tySpinBox->setValue(ty);
  _ui->tzSpinBox->setValue(tz);
  _ui->rxSpinBox->setValue(rx);
  _ui->rySpinBox->setValue(ry);
  _ui->rzSpinBox->setValue(rz);
  _T_i_v = Eigen::Translation3d(tx, ty, tz)
    * Eigen::AngleAxisd(rz, Eigen::Vector3d::UnitZ())
    * Eigen::AngleAxisd(ry, Eigen::Vector3d::UnitY())
    * Eigen::AngleAxisd(rx, Eigen::Vector3d::UnitX());
   emit updateViews();
}

void VelodyneControl::setRenderingRate(size_t rate) {
  _ui->revolutionSpinBox->setValue(rate);
}

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

void VelodyneControl::renderAxes(View& view, const QColor& color,
    double length) {
  Line<double, 3> l_1, l_2, l_3;
  l_1[1][0] = length;
  view.render(l_1, color, _T_w_i * _T_i_v);
  l_2[1][1] = length;
  view.render(l_2, color, _T_w_i * _T_i_v);
  l_3[1][2] = length;
  view.render(l_3, color, _T_w_i * _T_i_v);
  Eigen::Vector3d xLabelPosition = _T_w_i * _T_i_v * l_1[1];
  Eigen::Vector3d yLabelPosition = _T_w_i * _T_i_v * l_2[1];
  Eigen::Vector3d zLabelPosition = _T_w_i * _T_i_v * l_3[1];
  view.render("X", xLabelPosition, color, 0.2*length);
  view.render("Y", yLabelPosition, color, 0.2*length);
  view.render("Z", zLabelPosition, color, 0.2*length);
  Eigen::Vector3d labelPosition = _T_w_i * _T_i_v *
    Eigen::Vector3d(0, 0, length + 0.1);
  view.render("velodyne", labelPosition, color, 0.2 * length);
}

void VelodyneControl::renderPoints(View& view, const QColor& color, double size,
    bool smooth) {
  for (auto it = _pointCloudsDisp.cbegin(); it != _pointCloudsDisp.cend(); ++it)
    view.render(it->first, color, size, smooth, it->second * _T_i_v);
}

void VelodyneControl::calibrationBrowseClicked() {
  QString filename = QFileDialog::getOpenFileName(this,
    "Open Calibration File", _ui->calibrationEdit->text(),
    "Velodyne calibration files (*.dat)");
  if (!filename.isNull())
    setCalibrationFilename(filename);
}

void VelodyneControl::colorChanged(const QString& role, const QColor& color) {
  emit updateViews();
}

void VelodyneControl::pointSizeChanged(double pointSize) {
  setPointSize(pointSize);
}

void VelodyneControl::showPointsToggled(bool checked) {
  setShowPoints(checked);
}

void VelodyneControl::smoothPointsToggled(bool checked) {
  setSmoothPoints(checked);
}

void VelodyneControl::rangeSupportChanged() {
  setRangeSupport(_ui->minRangeSpinBox->value(), _ui->maxRangeSpinBox->value());
}

void VelodyneControl::renderView(View& view) {
  if (_ui->showPointsCheckBox->isChecked())
    renderPoints(view, _palette.getColor("Point"),
      _ui->pointSizeSpinBox->value(),
      _ui->smoothPointsCheckBox->isChecked());
  if (_ui->showAxesCheckBox->isChecked())
    renderAxes(view, _palette.getColor("Axes"), 0.5);
}


void VelodyneControl::messageRead(
    const velodyne::BinarySnappyMsgConstPtr& msg) {
  std::string uncompressedData;
  snappy::Uncompress(
    reinterpret_cast<const char*>(msg->data.data()),
    msg->data.size(), &uncompressedData);
  DataPacket dataPacket;
  std::istringstream binaryStream(uncompressedData);
  dataPacket.readBinary(binaryStream);
  VdynePointCloud pointCloud;
  Converter::toPointCloud(dataPacket, *_calibration, pointCloud, _minRange,
    _maxRange);
  const double startAngle = Calibration::deg2rad(
    dataPacket.getDataChunk(0).mRotationalInfo /
    (double)DataPacket::mRotationResolution);
  const double endAngle = Calibration::deg2rad(
    dataPacket.getDataChunk(DataPacket::mDataChunkNbr - 1).mRotationalInfo /
    (double)DataPacket::mRotationResolution);
  if ((_lastStartAngle > endAngle || startAngle > endAngle) &&
      _revolutionPacketCounter) {
    _revolutionPacketCounter = 0;
    static size_t turnDispCount = 0;
    turnDispCount++;
    if (turnDispCount >= _ui->revolutionSpinBox->value()) {
      turnDispCount = 0;
      _pointCloudsDisp.clear();
    }
    _pointCloudsDisp.reserve(_pointCloudsDisp.size() + _pointCloudsAcq.size());
    for (auto it = _pointCloudsAcq.cbegin(); it != _pointCloudsAcq.cend();
        ++it)
       _pointCloudsDisp.push_back(*it);
    _pointCloudsAcq.clear();
    emit updateViews();
  }
  else {
    _revolutionPacketCounter++;
  }
  _lastStartAngle = startAngle;
  Points<double, 3> points;
  _pointCloudsAcq.push_back(std::make_pair(points, _T_w_i));
  _pointCloudsAcq.back().first.setNumPoints(pointCloud.getSize());
  size_t i = 0;
  for (auto it = pointCloud.getPointBegin(); it != pointCloud.getPointEnd();
      ++it) {
    Points<double, 3>::Point point(it->mX, it->mY, it->mZ);
    _pointCloudsAcq.back().first[i++] = point;
  }
}

void VelodyneControl::messageRead(const rosbag::MessageInstance& message) {
  if (message.isType<velodyne::BinarySnappyMsg>()) {
    velodyne::BinarySnappyMsgPtr binarySnappyMsg(
      message.instantiate<velodyne::BinarySnappyMsg>());
    messageRead(binarySnappyMsg);
  }
}

void VelodyneControl::clearClicked() {
  _pointCloudsAcq.clear();
  _pointCloudsDisp.clear();
  emit updateViews();
}

void VelodyneControl::showAxesToggled(bool checked) {
  setShowAxes(checked);
}

void VelodyneControl::transformationChanged() {
  setTransformation(_ui->txSpinBox->value(), _ui->tySpinBox->value(),
    _ui->tzSpinBox->value(), _ui->rzSpinBox->value(), _ui->rySpinBox->value(),
    _ui->rxSpinBox->value());
}

void VelodyneControl::poseUpdate(const Eigen::Affine3d& T_w_i) {
  _T_w_i = T_w_i;
}
