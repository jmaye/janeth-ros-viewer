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

#ifndef PDFDOCUMENT_H
#define PDFDOCUMENT_H

#include <QtCore/QThread>

#include <QtCore/QMutex>

#include <QtGui/QImage>

namespace Poppler {
  class Document;
};

class PDFDocument :
  public QThread {
Q_OBJECT
public:
  PDFDocument(QObject* parent);
  ~PDFDocument();

  size_t getNumPages() const;
  QSize getPageSize(int page) const;
public slots:
  bool load(const QString& filename);
  void render(int page, size_t width, size_t height);
  void clear();
protected:
  mutable QMutex mutex;

  Poppler::Document* document;

  int page;
  QSize pageSize;

  virtual void run();
signals:
  void rendered(int page, const QImage& image);
};

#endif
