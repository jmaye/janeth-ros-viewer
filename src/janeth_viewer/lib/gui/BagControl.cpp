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

#include "gui/BagControl.h"

#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtCore/QDateTime>

#include <rosbag/exceptions.h>

#include "gui/framework.h"

#include "ui_BagControl.h"

/******************************************************************************/
/* Constructors and Destructor                                                */
/******************************************************************************/

BagControl::BagControl() :
    _ui(new Ui_BagControl()),
    _msgCnt(0) {
  _ui->setupUi(this);
  _timer.setSingleShot(true);
  connect(&_timer, SIGNAL(timeout()), this, SLOT(timerTimeout()));
  _ui->startTimeEdit->setReadOnly(true);
  _ui->endTimeEdit->setReadOnly(true);
  if (Framework::getInstance().hasArgument() &&
      (Framework::getInstance()[0].endsWith("bag")))
    setLogFilename(Framework::getInstance()[0]);
}

BagControl::~BagControl() {
  delete _ui;
}

/******************************************************************************/
/* Accessors                                                                  */
/******************************************************************************/

void BagControl::setLogFilename(const QString& filename) {
  _ui->logEdit->setText(filename);
  QFileInfo fileInfo(filename);
  if (fileInfo.isFile() && fileInfo.isReadable()) {
    try {
      _bag.reset(new rosbag::Bag(filename.toStdString()));
      _view.reset(new rosbag::View(*_bag));
      _bagBeginIt.reset(new rosbag::View::iterator(_view->begin()));
      _bagEndIt.reset(new rosbag::View::iterator(_view->end()));
    }
    catch (rosbag::BagException& e) {
      QMessageBox::information(this, "BagControl",
        tr("Exception: %1.").arg(e.what()));
    }
    _ui->logPlayButton->setEnabled(true);
    _ui->logStopButton->setEnabled(true);
    _ui->logForwardButton->setEnabled(true);
    _ui->msgSpinBox->setEnabled(true);
    _ui->msgSpinBox->setValue(_view->size());
    _ui->startTimeEdit->setEnabled(true);
    const double startTimestamp = _view->getBeginTime().toSec();
    const QDateTime startTime = QDateTime::fromTime_t(startTimestamp);
    QString startMsecs;
    startMsecs.sprintf("%03d", (uint)((startTimestamp -
      (size_t)startTimestamp) * 1e3));
    _ui->startTimeEdit->setText(startTime.toString(
      "yyyy-MM-dd hh:mm:ss:" + startMsecs));
    _ui->endTimeEdit->setEnabled(true);
    const double endTimestamp = _view->getEndTime().toSec();
    const QDateTime endTime = QDateTime::fromTime_t(endTimestamp);
    QString endMsecs;
    endMsecs.sprintf("%03d", (uint)((endTimestamp -
      (size_t)endTimestamp) * 1e3));
    _ui->endTimeEdit->setText(endTime.toString(
      "yyyy-MM-dd hh:mm:ss:" + endMsecs));
    logStopClicked();
  }
}

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

void BagControl::logBrowseClicked() {
  QString filename = QFileDialog::getOpenFileName(this, "Open Log File",
    _ui->logEdit->text(), "ROS bag files (*.bag)");
  if (!filename.isNull())
    setLogFilename(filename);
}

void BagControl::logPlayToggled() {
  _ui->logStopButton->setEnabled(!_ui->logPlayButton->isChecked());
  _ui->logForwardButton->setEnabled(!_ui->logPlayButton->isChecked());
  _ui->logBrowseButton->setEnabled(!_ui->logPlayButton->isChecked());
  if (_ui->logPlayButton->isChecked()) {
    _timer.start(0);
  }
  else
    _timer.stop();
}

void BagControl::logStopClicked() {
  _bagCurrIt.reset(new rosbag::View::iterator(*_bagBeginIt));
  _msgCnt = 0;
  _ui->logSlider->setSliderPosition(_ui->logSlider->minimum());
}

void BagControl::logForwardClicked() {
  if ((*_bagCurrIt) != (*_bagEndIt)) {
    (*_bagCurrIt)++;
    _msgCnt++;
  }
}

void BagControl::timerTimeout() {
  if ((*_bagCurrIt) != (*_bagEndIt)) {
    _ui->currTimeEdit->setEnabled(true);
    const double timestamp = (*_bagCurrIt)->getTime().toSec();
    const QDateTime time = QDateTime::fromTime_t(timestamp);
    QString msecs;
    msecs.sprintf("%03d", (uint)((timestamp - (size_t)timestamp) * 1e3));
    _ui->currTimeEdit->setText(time.toString(
      "yyyy-MM-dd hh:mm:ss:" + msecs));
    emit messageRead(*(*_bagCurrIt));
    (*_bagCurrIt)++;
    _msgCnt++;
    _ui->logSlider->setSliderPosition(_ui->logSlider->minimum() +
      (_ui->logSlider->maximum() - _ui->logSlider->minimum()) *
      (double)_msgCnt / (double)_view->size());
    _timer.start(0);
  }
  else
    _timer.stop();
}
