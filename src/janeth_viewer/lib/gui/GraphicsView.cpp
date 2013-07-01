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

#include "gui/GraphicsView.h"

#include <QtGui/QFileDialog>

#include "ui_GraphicsView.h"

/******************************************************************************/
/* Constructors and Destructor                                                */
/******************************************************************************/

GraphicsView::GraphicsView() :
    _ui(new Ui_GraphicsView()) {
  _ui->setupUi(this);
  connect(_ui->display, SIGNAL(paint()), this, SLOT(render()));
  connect(_ui->display, SIGNAL(resized()), this, SLOT(resized()));
  setDumpDirectory(QDir::current().path());
  setDumpFrameSize(1280, 720);
  setDumpFormat("dump2d%06d.png");
  setDumpAll(false);
}

GraphicsView::~GraphicsView() {
  delete _ui;
  for (auto it = _imagesMap.begin(); it != _imagesMap.end(); ++it)
    delete it->second.second;
  for (auto it = _labelsMap.begin(); it != _labelsMap.end(); ++it)
    delete it->second;
}

/******************************************************************************/
/* Accessors                                                                  */
/******************************************************************************/

GraphicsView::Size GraphicsView::getSize() const {
  return Size(getDisplay().rect().width(), getDisplay().rect().height());
}

void GraphicsView::setProjection(const Projection& projection) {
}

GraphicsView::Projection GraphicsView::getProjection() const {
}

void GraphicsView::setTransformation(const Transformation& transformation) {
}

GraphicsView::Transformation GraphicsView::getTransformation() const {
}

GraphicsDisplay& GraphicsView::getDisplay() {
  return *_ui->display;
}

const GraphicsDisplay& GraphicsView::getDisplay() const {
  return *_ui->display;
}

void GraphicsView::setDumpDirectory(const QString& dirname) {
  QDir dir(dirname);
  _ui->dumpDirEdit->setText(dir.absolutePath());
}

void GraphicsView::setDumpFrameSize(size_t width, size_t height) {
  _ui->dumpFrameWidthSpinBox->setValue(width);
  _ui->dumpFrameHeightSpinBox->setValue(height);
}

void GraphicsView::setDumpFormat(const QString& format) {
  _ui->dumpFormatEdit->setText(format);
}

void GraphicsView::setDumpAll(bool dumpAll) {
  if (dumpAll)
    connect(_ui->display, SIGNAL(paint()), this, SLOT(dumpFrame()));
  else
    disconnect(_ui->display, SIGNAL(paint()), this, SLOT(dumpFrame()));

  _ui->dumpAllCheckBox->setChecked(dumpAll);
}

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

void GraphicsView::saveTransformation() {
}

void GraphicsView::restoreTransformation() {
}

void GraphicsView::transform(const Transformation& transformation) {
}

void GraphicsView::translate(const Translation& translation) {
}

void GraphicsView::rotate(const Rotation& rotation) {
}

void GraphicsView::scale(const Scale& scale) {
}

void GraphicsView::clear(const QColor& color) {
}

void GraphicsView::enableFog(const QColor& color, double start, double end,
    double density) {
}

void GraphicsView::render(const Points<double, 3>& vertices,
    const QColor& color, double size, bool smooth) {
}

void GraphicsView::render(const Points<double, 3>& vertices, const
    std::vector<double>& weights, const QColor& fromColor, const QColor&
    toColor, double fromSize, double toSize, bool smooth) {
}

void GraphicsView::render(const Line<double, 3>& edges, const QColor& color) {
}

void GraphicsView::render(const Line<double, 3>& edges, double weight,
    const QColor& fromColor, const QColor& toColor) {
}

void GraphicsView::render(const QString& text, const QColor& color) {
}

void GraphicsView::render(const QImage& image, const QRectF& target,
    const Transformation& transformation, const std::string& serial,
    size_t imageId) {
  const double displayWidth = getSize()(0);
  const double displayHeight = getSize()(1);
  double imageWidth = displayWidth / 4.0;
  double imageHeight = displayHeight / 2.0;
  if (_imagesMap.count(serial) && _imagesMap[serial].first != imageId) {
    getDisplay().getScene().removeItem(_imagesMap[serial].second);
    delete _imagesMap[serial].second;
  }
  if (!_imagesMap.count(serial) || _imagesMap[serial].first != imageId) {
    QImage imageScaled(image.scaled(imageWidth, imageHeight));
    _imagesMap[serial] = std::make_pair(imageId,
      getDisplay().getScene().addPixmap(QPixmap::fromImage(imageScaled)));
  }
  if (!_labelsMap.count(serial)) {
    _labelsMap[serial] =
      getDisplay().getScene().addText(serial.c_str());
    _labelsMap[serial]->setDefaultTextColor(Qt::red);
  }
  double x = -displayWidth / 2.0;
  double y = -displayHeight / 2.0;
  size_t col = 0;
  size_t row = 0;
  for (auto it = _imagesMap.cbegin(); it != _imagesMap.cend(); ++it) {
    it->second.second->setPos(x, y);
    it->second.second->setZValue(0.7);
    _labelsMap[it->first]->setPos(x + 20, y + 20);
    _labelsMap[it->first]->setZValue(0.8);
    if (col == 3) {
      col = 0;
      row++;
      y += imageHeight;
      x = -displayWidth / 2.0;
    }
    else {
      x += imageWidth;
      col++;
    }
  }
}

bool GraphicsView::dumpFrame(const QString& filename, size_t width,
    size_t height) {
  static bool dumping = false;

  if (!dumping) {
    dumping = true;
    QImage image(QSize(width, height), QImage::Format_RGB32);
    QPainter painter(&image);
    getDisplay().render(&painter);
    dumping = false;
    return image.save(filename);
  }
  else
    return false;
}

void GraphicsView::dumpDirBrowseClicked() {
  QString dirname = QFileDialog::getExistingDirectory(this,
    "Select Dump Directory", _ui->dumpDirEdit->text());
  if (!dirname.isNull())
    _ui->dumpDirEdit->setText(dirname);
}

void GraphicsView::dumpFrame() {
  QDir dir(_ui->dumpDirEdit->text());

  if (dir.isReadable()) {
    QString filename;
    filename.sprintf(_ui->dumpFormatEdit->text().toAscii().constData(),
      _ui->dumpFrameSpinBox->value());

    QFileInfo fileInfo(dir, filename);
    if (dumpFrame(fileInfo.absoluteFilePath(),
        _ui->dumpFrameWidthSpinBox->value(),
        _ui->dumpFrameHeightSpinBox->value()))
      _ui->dumpFrameSpinBox->setValue(_ui->dumpFrameSpinBox->value()+1);
  }
}

void GraphicsView::dumpClicked() {
  dumpFrame();
}

void GraphicsView::dumpAllToggled(bool checked) {
  setDumpAll(checked);
}

void GraphicsView::update() {
  getDisplay().viewport()->update();
}

void GraphicsView::resized() {
  setDumpFrameSize(getDisplay().rect().width(), getDisplay().rect().height());
}
