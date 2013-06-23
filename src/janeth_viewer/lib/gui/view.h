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

#ifndef VIEW_H
#define VIEW_H

#include <eigen3/Eigen/Geometry>

#include <QtGui/QMenu>

#include "gui/widget.h"

#include "utils/points.h"
#include "utils/line.h"
#include "utils/box.h"
#include "utils/pyramid.h"
#include "utils/ellipsoid.h"

class MainWindow;

class View :
  public Widget {
Q_OBJECT
public:
  typedef Eigen::Matrix<double, 2, 1> Size;

  typedef Eigen::Transform<double, 3, Eigen::Projective> Projection;
  typedef Eigen::Transform<double, 3, Eigen::Affine> Transformation;

  typedef Eigen::Matrix<double, 3, 1> Translation;
  typedef Eigen::Matrix<double, 3, 1> Rotation;
  typedef Eigen::Matrix<double, 3, 1> Scale;

  typedef Eigen::Matrix<double, 3, 1> Point;
  typedef Eigen::Matrix<double, 4, 1> Vertex;

  View();
  ~View();

  QMenu& getMenu();
  const QMenu& getMenu() const;

  virtual Size getSize() const = 0;

  virtual void setProjection(const Projection& projection) = 0;
  virtual Projection getProjection() const = 0;
  virtual void setTransformation(const Transformation& transformation) = 0;
  virtual Transformation getTransformation() const = 0;

  double getAspectRatio() const;

  virtual void saveTransformation() = 0;
  virtual void restoreTransformation() = 0;

  virtual void transform(const Transformation& transformation) = 0;
  virtual void translate(const Translation& translation) = 0;
  virtual void rotate(const Rotation& rotation) = 0;
  virtual void scale(const Scale& scale) = 0;

  void translate(double x, double y, double z);
  void rotate(double yaw, double pitch, double roll);
  void scale(double x, double y, double z);
  void scale(double xyz);

  virtual void clear(const QColor& color) = 0;

  virtual void enableFog(const QColor& color, double start, double end,
    double density) = 0;

  virtual void render(const Points<double, 3>& vertices,
    const QColor& color, double size, bool smooth) = 0;
  virtual void render(const Points<double, 3>& vertices, const
    std::vector<double>& weights, const QColor& fromColor, const QColor&
    toColor, double fromSize, double toSize, bool smooth) = 0;
  virtual void render(const Line<double, 3>& edges,
    const QColor& color) = 0;
  virtual void render(const Line<double, 3>& edges, double weight,
    const QColor& fromColor, const QColor& toColor) = 0;
  virtual void render(const QString& text, const QColor& color) = 0;

  virtual void render(const Box<double, 3>& box, const QColor& color);
  virtual void render(const Pyramid<double, 3>& pyramid, const QColor& color);
  virtual void render(const Ellipsoid<double, 3>& ellipsoid, size_t
    numSegments, const QColor& color);
  virtual void render(const Ellipsoid<double, 3>& ellipsoid, size_t
    numSegments, double weight, const QColor& fromColor, const QColor&
    toColor);
  virtual void render(const QString& text, const Point& position, const
    QColor& color, double size);
  void render(const QString& text, double x, double y, double z,
    const QColor& color, double size);

  void map(Point& point) const;
  void unmap(Point& point) const;
public slots:
  virtual void render();
  virtual void flush();
protected:
  QMenu menu;
signals:
  void transform(View& view, const QPoint& fromMouse, const QPoint& toMouse,
    int wheel, Qt::MouseButtons mouseButtons);

  void prepare(View& view);
  void render(View& view);
  void cleanup(View& view);
};

#endif
