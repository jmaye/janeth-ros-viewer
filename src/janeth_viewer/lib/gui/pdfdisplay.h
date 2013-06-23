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

#ifndef PDFDISPLAY_H
#define PDFDISPLAY_H

#include <QtGui/QLabel>

#include <QtCore/QFileSystemWatcher>

#include "gui/pdfdocument.h"

class PDFDisplay :
  public QLabel {
Q_OBJECT
public:
  PDFDisplay(QWidget* parent = 0);
  ~PDFDisplay();

  void setFilename(const QString& filename);
  const QString& getFilename() const;

  void setPage(int page);
  int getPage() const;

  void reload();
  void clear();
protected:
  QString filename;
  int page;

  QImage image;
  QSize pageSize;

  QFileSystemWatcher watcher;
  PDFDocument document;

  void resizeEvent(QResizeEvent* event);
protected slots:
  void fileChanged(const QString& path);
  void pageRendered(int page, const QImage& image);
};

#endif
