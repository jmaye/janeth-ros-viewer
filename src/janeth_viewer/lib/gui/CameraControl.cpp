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

#include "gui/CameraControl.h"

#include <libsnappy/snappy.h>

#include "gui/BagControl.h"
#include "gui/RosControl.h"
#include "gui/PoslvControl.h"

#include "ui_CameraControl.h"

/******************************************************************************/
/* Constructors and Destructor                                                */
/******************************************************************************/

CameraControl::CameraControl(const std::string& serial,
    const Eigen::Affine3d& T_i_c, bool showImage, bool showAxes) :
    _ui(new Ui_CameraControl()),
    _T_w_i(Eigen::Translation3d(0, 0, 0)
      * Eigen::AngleAxisd(0, Eigen::Vector3d::UnitZ())
      * Eigen::AngleAxisd(0, Eigen::Vector3d::UnitY())
      * Eigen::AngleAxisd(0, Eigen::Vector3d::UnitX())),
    _T_i_c(T_i_c),
    _serial(serial),
    _imageWidth(0),
    _imageHeight(0),
    _imageId(0) {
  _ui->setupUi(this);
  _ui->colorChooser->setPalette(&_palette);
  connect(&_palette, SIGNAL(colorChanged(const QString&, const QColor&)),
    this, SLOT(colorChanged(const QString&, const QColor&)));
  connect<View>(SIGNAL(render(View&)), SLOT(renderView(View&)));
  connect<BagControl>(SIGNAL(messageRead(const rosbag::MessageInstance&)),
    SLOT(messageRead(const rosbag::MessageInstance&)));
  connect<RosControl>(
    SIGNAL(messageRead(const mv_cameras::ImageSnappyMsgConstPtr&)),
    SLOT(messageRead(const mv_cameras::ImageSnappyMsgConstPtr&)));
  connect<PoslvControl>(SIGNAL(poseUpdate(const Eigen::Affine3d&)),
    SLOT(poseUpdate(const Eigen::Affine3d&)));
  setShowImage(showImage);
  setAxesColor(Qt::red);
  setShowAxes(showAxes);
  _grayscaleColorTable.resize(std::numeric_limits<unsigned char>::max() + 1);
  for (size_t i = 0; i < _grayscaleColorTable.size(); ++i)
    _grayscaleColorTable[i] = qRgb(i, i, i);
  setRenderingRate(1);
}

CameraControl::~CameraControl() {
  delete _ui;
}

/******************************************************************************/
/* Accessors                                                                  */
/******************************************************************************/

void CameraControl::setShowImage(bool showImage) {
  _ui->showImageCheckBox->setChecked(showImage);
  emit updateViews();
}

void CameraControl::setAxesColor(const QColor& color) {
  _palette.setColor("Axes", color);
}

void CameraControl::setShowAxes(bool showAxes) {
  _ui->showAxesCheckBox->setChecked(showAxes);
  emit updateViews();
}

void CameraControl::setTransformation(double tx, double ty, double tz,
    double rz, double ry, double rx) {
  _ui->txSpinBox->setValue(tx);
  _ui->tySpinBox->setValue(ty);
  _ui->tzSpinBox->setValue(tz);
  _ui->rxSpinBox->setValue(rx);
  _ui->rySpinBox->setValue(ry);
  _ui->rzSpinBox->setValue(rz);
  _T_i_c = Eigen::Translation3d(tx, ty, tz)
    * Eigen::AngleAxisd(rz, Eigen::Vector3d::UnitZ())
    * Eigen::AngleAxisd(ry, Eigen::Vector3d::UnitY())
    * Eigen::AngleAxisd(rx, Eigen::Vector3d::UnitX());
   emit updateViews();
}

void CameraControl::setRenderingRate(size_t rate) {
  _ui->rateSpinBox->setValue(rate);
}

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

void CameraControl::renderAxes(View& view, const QColor& color,
    double length) {
  Line<double, 3> l_1, l_2, l_3;
  l_1[1][0] = length;
  view.render(l_1, color, _T_w_i * _T_i_c);
  l_2[1][1] = length;
  view.render(l_2, color, _T_w_i * _T_i_c);
  l_3[1][2] = length;
  view.render(l_3, color, _T_w_i * _T_i_c);
  Eigen::Vector3d xLabelPosition = _T_w_i * _T_i_c * l_1[1];
  Eigen::Vector3d yLabelPosition = _T_w_i * _T_i_c * l_2[1];
  Eigen::Vector3d zLabelPosition = _T_w_i * _T_i_c * l_3[1];
  view.render("X", xLabelPosition, color, 0.2*length);
  view.render("Y", yLabelPosition, color, 0.2*length);
  view.render("Z", zLabelPosition, color, 0.2*length);
  Eigen::Vector3d labelPosition = _T_w_i * _T_i_c *
    Eigen::Vector3d(0, 0, length + 0.1);
  view.render(QString(_serial.c_str()), labelPosition, color, 0.2 * length);
}

void CameraControl::renderImage(View& view) {
  if (_imageHeight && _imageWidth) {
    QImage image(reinterpret_cast<const unsigned char*>(_imageData.c_str()),
      _imageWidth, _imageHeight, QImage::Format_Indexed8);
    image.setColorTable(_grayscaleColorTable);
    view.render(image, QRectF(0, 0, _imageWidth / 1000.0,
      _imageHeight / 1000.0), _T_w_i, _serial, _imageId);
  }
}

void CameraControl::colorChanged(const QString& role, const QColor& color) {
  emit updateViews();
}

void CameraControl::showImageToggled(bool checked) {
  setShowImage(checked);
}

void CameraControl::renderView(View& view) {
  if (_ui->showImageCheckBox->isChecked())
    renderImage(view);
  if (_ui->showAxesCheckBox->isChecked())
    renderAxes(view, _palette.getColor("Axes"), 0.5);
}

void CameraControl::messageRead(const mv_cameras::ImageSnappyMsgConstPtr& msg) {
  if (msg->header.frame_id == ("/" + _serial + "_link")) {
    static size_t renderingCount = 0;
    renderingCount++;
    if (renderingCount >= _ui->rateSpinBox->value()) {
      std::string uncompressedData;
      snappy::Uncompress(reinterpret_cast<const char*>(msg->data.data()),
        msg->data.size(), &uncompressedData);
      _imageWidth = msg->width;
      _imageHeight = msg->height;
      _imageData = uncompressedData;
      _imageId = msg->header.seq;
      emit updateViews();
      renderingCount = 0;
    }
  }
}

void CameraControl::messageRead(const rosbag::MessageInstance& message) {
  if (message.isType<mv_cameras::ImageSnappyMsg>()) {
    mv_cameras::ImageSnappyMsgPtr imageSnappyMsg(
      message.instantiate<mv_cameras::ImageSnappyMsg>());
    messageRead(imageSnappyMsg);
  }
}

void CameraControl::showAxesToggled(bool checked) {
  setShowAxes(checked);
}

void CameraControl::transformationChanged() {
  setTransformation(_ui->txSpinBox->value(), _ui->tySpinBox->value(),
    _ui->tzSpinBox->value(), _ui->rzSpinBox->value(), _ui->rySpinBox->value(),
    _ui->rxSpinBox->value());
}

void CameraControl::poseUpdate(const Eigen::Affine3d& T_w_i) {
  _T_w_i = T_w_i;
}
