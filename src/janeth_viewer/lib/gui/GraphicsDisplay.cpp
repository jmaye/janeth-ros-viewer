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

#include "gui/GraphicsDisplay.h"

/******************************************************************************/
/* Constructors and Destructor                                                */
/******************************************************************************/

GraphicsDisplay::GraphicsDisplay(QWidget* parent) :
    QGraphicsView(parent) {
  setScene(&_scene);
  setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
  setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
  setResizeAnchor(QGraphicsView::AnchorUnderMouse);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

GraphicsDisplay::~GraphicsDisplay() {
}

/******************************************************************************/
/* Accessors                                                                  */
/******************************************************************************/

QGraphicsScene& GraphicsDisplay::getScene() {
  return _scene;
}

const QGraphicsScene& GraphicsDisplay::getScene() const {
  return _scene;
}

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

void GraphicsDisplay::paintEvent(QPaintEvent* event) {
  QGraphicsView::paintEvent(event);
  emit paint();
}

void GraphicsDisplay::resizeEvent(QResizeEvent* event) {
  QGraphicsView::resizeEvent(event);
  emit resized();
}
