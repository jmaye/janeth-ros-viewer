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

#ifndef GLVIEW_H
#define GLVIEW_H

#include <QtOpenGL/QGLWidget>

#include "gui/view.h"

#include "gui/gldisplay.h"
#include "gui/camera.h"
#include "gui/scene.h"
#include "gui/palette.h"

class FTPolygonFont;

class Ui_GLView;

class GLView :
  public View {
Q_OBJECT
public:
  typedef Eigen::Matrix<int, 4, 1> Viewport;

  GLView();
  ~GLView();

  GLDisplay& getDisplay();
  const GLDisplay& getDisplay() const;

  void setFont(const QString& filename);
  const QString& getFont() const;

  Size getSize() const;

  Viewport getViewport() const;
  void setProjection(const Projection& projection);
  Projection getProjection() const;
  void setTransformation(const Transformation& transformation);
  Transformation getTransformation() const;

  void setDumpDirectory(const QString& dirname);
  void setDumpFrameSize(size_t width, size_t height);
  void setDumpFormat(const QString& format);
  void setDumpAll(bool dumpAll);

  void saveTransformation();
  void restoreTransformation();

  void transform(const Transformation& transformation);
  using View::transform;
  void translate(const Translation& translation);
  using View::translate;
  void rotate(const Rotation& rotation);
  using View::rotate;
  void scale(const Scale& scale);
  using View::scale;

  void clear(const QColor& color);

  void enableFog(const QColor& color, double start, double end,
    double density);

  void render(const QImage& image, const QRectF& target,
    const Transformation& transformation, const std::string& serial,
    size_t imageId);
  void render(const Points<double, 3>& vertices, const QColor& color,
    double size, bool smooth);
  void render(const Points<double, 3>& vertices, const std::vector<double>&
    weights, const QColor& fromColor, const QColor& toColor, double fromSize,
    double toSize, bool smooth);
  void render(const Line<double, 3>& edges, const QColor& color);
  void render(const Line<double, 3>& edges, double weight, const QColor&
    fromColor, const QColor& toColor);
  void render(const QString& text, const QColor& color);

  void render(const Ellipsoid<double, 3>& ellipsoid, size_t numSegments,
    const QColor& color);

  using View::render;

  bool dumpFrame(const QString& filename, size_t width, size_t height);
public slots:
  void update();
  void render();
  void dumpFrame();
protected:
  Ui_GLView* ui;

  QAction* shadeAction;

  QString fontFilename;
  GLUquadricObj* quadric;
  FTPolygonFont* font;

  QPoint mousePosition;
protected slots:
  void mousePressed(const QPoint& position, Qt::MouseButtons buttons);
  void mouseMoved(const QPoint& position, int wheel, Qt::MouseButtons buttons);

  void fontBrowseClicked();
  void shadedClicked();

  void dumpDirBrowseClicked();
  void dumpClicked();
  void dumpAllToggled(bool checked);
signals:
  void fontChanged(const QString& filename);
};

#endif
