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

#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>

#include "gui/gldisplay.h"

/*****************************************************************************/
/* Constructors and Destructor                                               */
/*****************************************************************************/

GLDisplay::GLDisplay(QWidget* parent) :
  QGLWidget(parent) {
}

GLDisplay::~GLDisplay() {
}

/*****************************************************************************/
/* Methods                                                                   */
/*****************************************************************************/

void GLDisplay::mousePressEvent(QMouseEvent* event) {
  QPoint position(event->x(), height()-event->y());
  emit mousePressed(position, event->buttons());
}

void GLDisplay::mouseMoveEvent(QMouseEvent* event) {
  QPoint position(event->x(), height()-event->y());
  emit mouseMoved(position, 0, event->buttons());
}

void GLDisplay::wheelEvent(QWheelEvent* event) {
  QPoint position(event->x(), height()-event->y());
  emit mouseMoved(position, event->delta(), event->buttons());
}

void GLDisplay::initializeGL() {
  glEnable(GL_DEPTH_TEST);
  
  glEnable (GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void GLDisplay::resizeGL(int width, int height) {
  glViewport(0, 0, width, height);
}

void GLDisplay::paintGL() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glDisable(GL_FOG);

  emit paint();
}
