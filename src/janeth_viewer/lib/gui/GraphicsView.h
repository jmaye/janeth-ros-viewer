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

/** \file GraphicsView.h
    \brief This file implements a graphics view.
  */

#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <unordered_map>

#include <QtGui/QGraphicsPixmapItem>
#include <QtGui/QGraphicsTextItem>

#include "gui/view.h"
#include "gui/GraphicsDisplay.h"

class Ui_GraphicsView;

/** The GraphicsView class represents a graphics view.
    \brief Graphics view.
  */
class GraphicsView :
  public View {
Q_OBJECT
  /** \name Private constructors
  @{
  */
  /// Copy constructor
  GraphicsView(const GraphicsView& other);
  /// Assignment operator
  GraphicsView& operator = (const GraphicsView& other);
  /** @}
    */

public:
  /** \name Constructors/destructor
    @{
    */
  /// Constructs the view
  GraphicsView();
  /// Destructor
  ~GraphicsView();
  /** @}
    */

  /** \name Accessors
    @{
    */
  /// Returns the size of the view
  Size getSize() const;
  /// Sets the projection
  void setProjection(const Projection& projection);
  /// Returns the projection
  Projection getProjection() const;
  /// Sets the transformation
  void setTransformation(const Transformation& transformation);
  /// Returns the transformation
  Transformation getTransformation() const;
  /// Returns the display
  GraphicsDisplay& getDisplay();
  /// Returns the display
  const GraphicsDisplay& getDisplay() const;
  /// Sets the dump directory
  void setDumpDirectory(const QString& dirname);
  /// Sets the dump frame size
  void setDumpFrameSize(size_t width, size_t height);
  /// Sets the dump format
  void setDumpFormat(const QString& format);
  /// Sets the dump all flag
  void setDumpAll(bool dumpAll);
  /** @}
    */

  /** \name Methods
    @{
    */
  /// Saves the transformation
  void saveTransformation();
  /// Restores the transformation
  void restoreTransformation();
  /// Transforms the view
  void transform(const Transformation& transformation);
  /// Translates the view
  void translate(const Translation& translation);
  /// Rotates the view
  void rotate(const Rotation& rotation);
  /// Scales the view
  void scale(const Scale& scale);
  /// Clears the color
  void clear(const QColor& color);
  /// Enable fog
  void enableFog(const QColor& color, double start, double end, double density);
  /// Renders points
  void render(const Points<double, 3>& vertices, const QColor& color,
    double size, bool smooth);
  /// Renders points with weights
  void render(const Points<double, 3>& vertices, const
    std::vector<double>& weights, const QColor& fromColor, const QColor&
    toColor, double fromSize, double toSize, bool smooth);
  /// Renders lines
  void render(const Line<double, 3>& edges, const QColor& color);
  /// Renders lines with weights
  void render(const Line<double, 3>& edges, double weight,
    const QColor& fromColor, const QColor& toColor);
  /// Renders text
  void render(const QString& text, const QColor& color);
  /// Renders an image
  void render(const QImage& image, const QRectF& target,
    const Transformation& transformation, const std::string& serial,
    size_t imageId);
  /** @}
    */

  using View::render;

protected:
  /** \name Protected methods
    @{
    */
  /// Dump a frame
  bool dumpFrame(const QString& filename, size_t width, size_t height);
  /** @}
    */

  /** \name Protected members
    @{
    */
  /// Qt user interface
  Ui_GraphicsView* _ui;
  /// Images map
  std::unordered_map<std::string, std::pair<size_t, QGraphicsPixmapItem*> >
    _imagesMap;
  /// Labels map
  std::unordered_map<std::string, QGraphicsTextItem*> _labelsMap;
  /** @}
    */

protected slots:
  /** \name Qt slots
    @{
    */
  /// Dump dir clicked
  void dumpDirBrowseClicked();
  /// Dump clicked
  void dumpClicked();
  /// Dump all toggled
  void dumpAllToggled(bool checked);
  /// Update the view
  void update();
  /// Dump the frame
  void dumpFrame();
  /// View resized
  void resized();
  /** @}
    */

};

#endif // GRAPHICSVIEW
