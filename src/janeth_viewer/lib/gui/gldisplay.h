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

#ifndef GLDISPLAY_H
#define GLDISPLAY_H

#include <GL/glu.h>

#include <QtOpenGL/QGLWidget>

#include "gui/view.h"

class GLDisplay :
  public QGLWidget {
Q_OBJECT
public:
  GLDisplay(QWidget* parent = 0);
  ~GLDisplay();
protected:
  virtual void initializeGL();
  virtual void resizeGL(int width, int height);
  virtual void paintGL();

  virtual void mousePressEvent(QMouseEvent* event);
  virtual void mouseMoveEvent(QMouseEvent* event);
  virtual void wheelEvent(QWheelEvent* event);
signals:
  void mousePressed(const QPoint& position, Qt::MouseButtons buttons);
  void mouseMoved(const QPoint& position, int wheel, Qt::MouseButtons buttons);
  void paint();
};

#endif
