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

#include <poppler/GlobalParams.h>
#include <poppler/qt4/poppler-qt4.h>

#include "gui/pdfdocument.h"

/*****************************************************************************/
/* Constructors and Destructor                                               */
/*****************************************************************************/

PDFDocument::PDFDocument(QObject* parent) :
  QThread(parent),
  document(0) {
}

PDFDocument::~PDFDocument() {
  clear();
}

/*****************************************************************************/
/* Accessors                                                                 */
/*****************************************************************************/

size_t PDFDocument::getNumPages() const {
  if (document)
    return document->numPages();
  else
    return 0;
}

QSize PDFDocument::getPageSize(int page) const {
  QSize pageSize;

  if (document && (page >= 0) && (page < document->numPages()))
    pageSize = document->page(page)->pageSize();

  return pageSize;
}

/*****************************************************************************/
/* Methods                                                                   */
/*****************************************************************************/

bool PDFDocument::load(const QString& filename) {
  clear();

  document = Poppler::Document::load(filename);

  return document;
}

void PDFDocument::render(int page, size_t width, size_t height) {
  mutex.lock();

  this->page = page;
  this->pageSize = QSize(width, height);

  mutex.unlock();

  start();
}

void PDFDocument::clear() {
  if (document) {
    wait();

    delete document;
    document = 0;
  }
}

void PDFDocument::run() {
  QImage image;

  mutex.lock();

  if (document && (page >= 0) && (page < document->numPages())) {
    int page = this->page;
    QSize pageSize = document->page(page)->pageSize();
    double scale = (double)this->pageSize.width()/pageSize.width();

    mutex.unlock();

    document->setRenderHint(Poppler::Document::TextAntialiasing);
    image = document->page(page)->renderToImage(scale*72.0, scale*72.0);
  }
  else
    mutex.unlock();

  emit rendered(page, image);
}
