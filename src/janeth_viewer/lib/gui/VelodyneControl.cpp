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

#include <velodyne/BinarySnappyMsg.h>

#include <libsnappy/snappy.h>

#include "gui/BagControl.h"
#include "gui/PoslvControl.h"

#include "ui_VelodyneControl.h"

/******************************************************************************/
/* Constructors and Destructor                                                */
/******************************************************************************/

VelodyneControl::VelodyneControl(bool showPoints) :
    _ui(new Ui_VelodyneControl()),
    _minRange(Converter::mMinDistance),
    _maxRange(Converter::mMaxDistance),
    _lastStartAngle(0),
    _revolutionPacketCounter(0),
    _origin(0.0, 0.0, 0.0),
    _orientation(0.0, 0.0, 0.0) {
  _ui->setupUi(this);
  _ui->colorChooser->setPalette(&_palette);
  connect(&_palette, SIGNAL(colorChanged(const QString&, const QColor&)),
    this, SLOT(colorChanged(const QString&, const QColor&)));
  connect<View>(SIGNAL(render(View&)), SLOT(renderView(View&)));
  connect<BagControl>(SIGNAL(messageRead(const rosbag::MessageInstance&)),
    SLOT(messageRead(const rosbag::MessageInstance&)));
  connect<PoslvControl>(SIGNAL(poseUpdate(double, double, double, double,
    double, double)),
    SLOT(poseUpdate(double, double, double, double, double, double)));
  setPointColor(Qt::gray);
  setPointSize(1.0);
  setShowPoints(showPoints);
  setSmoothPoints(true);
  setCalibrationFilename("/etc/libvelodyne/calib-HDL-64E.dat");
  setRangeSupport(_ui->minRangeSpinBox->value(), _ui->maxRangeSpinBox->value());
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

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

void VelodyneControl::renderPoints(View& view, const QColor& color, double size,
    bool smooth) {
  for (auto it = _pointCloudsDisp.cbegin(); it != _pointCloudsDisp.cend(); ++it)
    view.render(*it, color, size, smooth);
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
}

void VelodyneControl::messageRead(const rosbag::MessageInstance& message) {
  if (message.isType<velodyne::BinarySnappyMsg>()) {
    velodyne::BinarySnappyMsgPtr binarySnappyMsg(
      message.instantiate<velodyne::BinarySnappyMsg>());
    std::string uncompressedData;
    snappy::Uncompress(
      reinterpret_cast<const char*>(binarySnappyMsg->data.data()),
      binarySnappyMsg->data.size(), &uncompressedData);
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
      _pointCloudsDisp.clear();
      _pointCloudsDisp.reserve(_pointCloudsAcq.size());
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
    points.setOrigin(_origin);
    points.setOrientation(_orientation);
    _pointCloudsAcq.push_back(points);
    _pointCloudsAcq.back().setNumPoints(pointCloud.getSize());
    size_t i = 0;
    for (auto it = pointCloud.getPointBegin(); it != pointCloud.getPointEnd();
        ++it) {
      Points<double, 3>::Point point(it->mX, it->mY, it->mZ);
      _pointCloudsAcq.back()[i++] = point;
    }
  }
}

void VelodyneControl::poseUpdate(double x, double y, double z, double yaw,
    double pitch, double roll) {
  _origin = Eigen::Matrix<double, 3, 1>(x, y, z);
  _orientation = Eigen::Matrix<double, 3, 1>(yaw, pitch, roll);
}

void VelodyneControl::clearClicked() {
  _pointCloudsAcq.clear();
  _pointCloudsDisp.clear();
  emit updateViews();
}
