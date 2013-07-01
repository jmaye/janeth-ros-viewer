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

/** \file GraphicsDisplay.h
    \brief This file implements a graphics display.
  */

#ifndef GRAPHICSDISPLAY_H
#define GRAPHICSDISPLAY_H

#include <QtGui/QGraphicsView>
#include <QtGui/QGraphicsScene>

/** The GraphicsDisplay class represents a graphics display.
    \brief Graphics display.
  */
class GraphicsDisplay :
  public QGraphicsView {
Q_OBJECT
  /** \name Private constructors
  @{
  */
  /// Copy constructor
  GraphicsDisplay(const GraphicsDisplay& other);
  /// Assignment operator
  GraphicsDisplay& operator = (const GraphicsDisplay& other);
  /** @}
    */

public:
  /** \name Constructors/destructor
    @{
    */
  /// Constructs the display
  GraphicsDisplay(QWidget* parent = 0);
  /// Destructor
  ~GraphicsDisplay();
  /** @}
    */

  /** \name Accessors
    @{
    */
  /// Returns the scene
  QGraphicsScene& getScene();
  /// Returns the scene
  const QGraphicsScene& getScene() const;
  /** @}
    */

  /** \name Methods
    @{
    */
  /** @}
    */

protected:
  /** \name Protected methods
    @{
    */
  /// Paint event
  virtual void paintEvent(QPaintEvent* event);
  /// Resize event
  virtual void resizeEvent(QResizeEvent* event);
  /** @}
    */

  /** \name Protected members
    @{
    */
  /// Scene we are drawing in
  QGraphicsScene _scene;
  /** @}
    */

protected slots:
  /** \name Qt slots
    @{
    */
  /** @}
    */

signals:
  /** \name Qt signals
    @{
    */
  /// Paint
  void paint();
  /// Resized signal
  void resized();
  /** @}
    */

};

#endif // GRAPHICSDISPLAY
