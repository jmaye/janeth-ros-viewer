/***************************************************************************
 *   Copyright (C) 2010 by Ralf Kaestner, Nikolas Engelhard, Yves Pilat    *
 *   ralf.kaestner@gmail.com                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QtCore/QFileInfo>

#include <QtGui/QApplication>

#include "gui/pdfdisplay.h"

/*****************************************************************************/
/* Constructors and Destructor                                               */
/*****************************************************************************/

PDFDisplay::PDFDisplay(QWidget* parent) :
  QLabel(parent),
  page(0),
  document(this) {
  connect(&watcher, SIGNAL(fileChanged(const QString&)), this,
    SLOT(fileChanged(const QString&)));
  connect(&document, SIGNAL(rendered(int, const QImage&)), this,
    SLOT(pageRendered(int, const QImage&)));

  setAlignment(Qt::AlignCenter);
  setText("No Preview");
}

PDFDisplay::~PDFDisplay() {
}

/*****************************************************************************/
/* Accessors                                                                 */
/*****************************************************************************/

void PDFDisplay::setFilename(const QString& filename) {
  QFileInfo fileInfo(filename);

  if (fileInfo.isFile() && fileInfo.isReadable()) {
    if (this->filename != filename) {
      if (!watcher.files().empty())
        watcher.removePaths(watcher.files());
      this->filename = filename;
      watcher.addPath(this->filename);
    }

    reload();
  }
  else
    clear();
}

const QString& PDFDisplay::getFilename() const {
  return filename;
}

void PDFDisplay::setPage(int page) {
  pageSize = document.getPageSize(page);
  pageSize.scale(size(), Qt::KeepAspectRatio);

  document.render(page, pageSize.width(), pageSize.height());
}

int PDFDisplay::getPage() const {
  return page;
}

/*****************************************************************************/
/* Methods                                                                   */
/*****************************************************************************/

void PDFDisplay::reload() {
  if (filename != "") {
    setText("Loading...");
    if (document.load(filename)) {
      setText("Rendering...");
      setPage(page);
    }
    else
      clear();
  }
}

void PDFDisplay::clear() {
  document.clear();

  if (!watcher.files().empty())
    watcher.removePaths(watcher.files());
  this->filename = "";

  QLabel::clear();
  setText("No Preview");
}

void PDFDisplay::resizeEvent(QResizeEvent* event) {
  if (image.width() && image.height())
    setPixmap(QPixmap::fromImage(image).scaled(size(), Qt::KeepAspectRatio));

  setPage(page);
}

void PDFDisplay::fileChanged(const QString& path) {
  QApplication::removePostedEvents(&watcher);
  reload();
}

void PDFDisplay::pageRendered(int page, const QImage& image) {
  this->page = page;
  this->image = image;

  if (this->image.width() && this->image.height()) {
    QLabel::clear();
    setPixmap(QPixmap::fromImage(this->image).scaled(size(),
      Qt::KeepAspectRatio));

    if (image.size() != pageSize)
      document.render(page, pageSize.width(), pageSize.height());
  }
  else
    clear();
}
