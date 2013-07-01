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

#include "gui/view.h"

#include "gui/framework.h"
#include "gui/control.h"

/*****************************************************************************/
/* Constructors and Destructor                                               */
/*****************************************************************************/

View::View() :
  menu(this) {
  connect<Control>(SIGNAL(updateViews()), SLOT(update()));
  connect<Control>(SIGNAL(flushViews()), SLOT(flush()));
}

View::~View() {
}

/*****************************************************************************/
/* Accessors                                                                 */
/*****************************************************************************/

QMenu& View::getMenu() {
  return menu;
}

const QMenu& View::getMenu() const {
  return menu;
}

double View::getAspectRatio() const {
  Size size = getSize();
  return size[0]/size[1];
}

/*****************************************************************************/
/* Methods                                                                   */
/*****************************************************************************/

void View::translate(double x, double y, double z) {
  translate(Translation(x, y, z));
}

void View::rotate(double yaw, double pitch, double roll) {
  rotate(Rotation(yaw, pitch, roll));
}

void View::scale(double x, double y, double z) {
  scale(Scale(x, y, z));
}

void View::scale(double xyz) {
  scale(Scale(xyz, xyz, xyz));
}

void View::render(const Pyramid<double, 3>& pyramid, const QColor& color) {
  saveTransformation();

  translate(pyramid.getOrigin());
  rotate(pyramid.getOrientation());
  scale(pyramid.getSize());

  Line<double, 3> loop(4, true);
  loop[0] = Point(-0.5, -0.5, -0.5);
  loop[1] = Point(0.5, -0.5, -0.5);
  loop[2] = Point(0.5, 0.5, -0.5);
  loop[3] = Point(-0.5, 0.5, -0.5);
  render(loop, color);

  Point apex(0.0, 0.0, 0.5);
  Line<double, 3> l_1, l_2, l_3, l_4;
  l_1[0] = Point(-0.5, -0.5, -0.5);
  l_1[1] = apex;
  render(l_1, color);
  l_2[0] = Point(0.5, -0.5, -0.5);
  l_2[1] = apex;
  render(l_2, color);
  l_3[0] = Point(0.5, 0.5, -0.5);
  l_3[1] = apex;
  render(l_3, color);
  l_4[0] = Point(-0.5, 0.5, -0.5);
  l_4[1] = apex;
  render(l_4, color);

  restoreTransformation();
}

void View::render(const Box<double, 3>& box, const QColor& color) {
  saveTransformation();

  translate(box.getOrigin());
  rotate(box.getOrientation());
  scale(box.getSize());

  Line<double, 3> loop(4, true);
  loop[0] = Point(-0.5, -0.5, -0.5);
  loop[1] = Point(0.5, -0.5, -0.5);
  loop[2] = Point(0.5, 0.5, -0.5);
  loop[3] = Point(-0.5, 0.5, -0.5);
  render(loop, color);
  loop[0] = Point(-0.5, -0.5, 0.5);
  loop[1] = Point(0.5, -0.5, 0.5);
  loop[2] = Point(0.5, 0.5, 0.5);
  loop[3] = Point(-0.5, 0.5, 0.5);
  render(loop, color);

  Line<double, 3> l_1, l_2, l_3, l_4;
  l_1[0] = Point(-0.5, -0.5, -0.5);
  l_1[1] = Point(-0.5, -0.5, 0.5);
  render(l_1, color);
  l_2[0] = Point(0.5, -0.5, -0.5);
  l_2[1] = Point(0.5, -0.5, 0.5);
  render(l_2, color);
  l_3[0] = Point(0.5, 0.5, -0.5);
  l_3[1] = Point(0.5, 0.5, 0.5);
  render(l_3, color);
  l_4[0] = Point(-0.5, 0.5, -0.5);
  l_4[1] = Point(-0.5, 0.5, 0.5);
  render(l_4, color);

  restoreTransformation();
}

void View::render(const Ellipsoid<double, 3>& ellipsoid, size_t numSegments,
    const QColor& color) {
  saveTransformation();

  translate(ellipsoid.getOrigin());
  rotate(ellipsoid.getOrientation());
  scale(ellipsoid.getSize());

  Line<double, 3> l_1, l_2, l_3;
  l_1[0][0] = -1.0;
  l_1[1][0] = 1.0;
  render(l_1, color);
  l_2[0][1] = -1.0;
  l_2[1][1] = 1.0;
  render(l_2, color);
  l_3[0][2] = -1.0;
  l_3[1][2] = 1.0;
  render(l_3, color);

  Line<double, 3> loop(2.0*M_PI*std::max(ellipsoid.getSize()[0],
    ellipsoid.getSize()[1])*numSegments, true);
  double thetaStep = 2.0*M_PI/loop.getNumPoints();
  for (int i = 0; i < loop.getNumPoints(); ++i)
    loop[i] = Point(sin(i*thetaStep), cos(i*thetaStep), 0.0);
  render(loop, color);

  loop.setNumPoints(2.0*M_PI*std::max(ellipsoid.getSize()[0],
    ellipsoid.getSize()[2])*numSegments);
  thetaStep = 2.0*M_PI/loop.getNumPoints();
  for (int i = 0; i < loop.getNumPoints(); ++i)
    loop[i] = Point(sin(i*thetaStep), 0.0, cos(i*thetaStep));
  render(loop, color);

  loop.setNumPoints(2.0*M_PI*std::max(ellipsoid.getSize()[1],
    ellipsoid.getSize()[2])*numSegments);
  thetaStep = 2.0*M_PI/loop.getNumPoints();
  for (int i = 0; i < loop.getNumPoints(); ++i)
    loop[i] = Point(0.0, sin(i*thetaStep), cos(i*thetaStep));
  render(loop, color);

  restoreTransformation();
}

void View::render(const Ellipsoid<double, 3>& ellipsoid, size_t numSegments,
    double weight, const QColor& fromColor, const QColor& toColor) {
  saveTransformation();

  translate(ellipsoid.getOrigin());
  rotate(ellipsoid.getOrientation());
  scale(ellipsoid.getSize());

  Line<double, 3> l_1, l_2, l_3;
  l_1[0][0] = -1.0;
  l_1[1][0] = 1.0;
  render(l_1, weight, fromColor, toColor);
  l_2[0][1] = -1.0;
  l_2[1][1] = 1.0;
  render(l_2, weight, fromColor, toColor);
  l_3[0][2] = -1.0;
  l_3[1][2] = 1.0;
  render(l_3, weight, fromColor, toColor);

  Line<double, 3> loop(2.0*M_PI*std::max(ellipsoid.getSize()[0],
    ellipsoid.getSize()[1])*numSegments, true);
  double thetaStep = 2.0*M_PI/loop.getNumPoints();
  for (int i = 0; i < loop.getNumPoints(); ++i)
    loop[i] = Point(sin(i*thetaStep), cos(i*thetaStep), 0.0);
  render(loop, weight, fromColor, toColor);

  loop.setNumPoints(2.0*M_PI*std::max(ellipsoid.getSize()[0],
    ellipsoid.getSize()[2])*numSegments);
  thetaStep = 2.0*M_PI/loop.getNumPoints();
  for (int i = 0; i < loop.getNumPoints(); ++i)
    loop[i] = Point(sin(i*thetaStep), 0.0, cos(i*thetaStep));
  render(loop, weight, fromColor, toColor);

  loop.setNumPoints(2.0*M_PI*std::max(ellipsoid.getSize()[1],
    ellipsoid.getSize()[2])*numSegments);
  thetaStep = 2.0*M_PI/loop.getNumPoints();
  for (int i = 0; i < loop.getNumPoints(); ++i)
    loop[i] = Point(0.0, sin(i*thetaStep), cos(i*thetaStep));
  render(loop, weight, fromColor, toColor);

  restoreTransformation();
}

void View::render(const QString& text, const Point& position, const QColor&
    color, double size) {
  saveTransformation();

  translate(position);
  scale(size);

  Transformation::MatrixType T = getTransformation().matrix();
  double scale = T.col(0).norm();
  T(0, 0) = scale;
  T(1, 1) = scale;
  T(2, 2) = scale;
  T(0, 1) = 0.0;
  T(0, 2) = 0.0;
  T(1, 0) = 0.0;
  T(1, 2) = 0.0;
  T(2, 0) = 0.0;
  T(2, 1) = 0.0;

  Transformation transformation(T);
  setTransformation(transformation);

  render(text, color);

  restoreTransformation();
}

void View::render(const QString& text, double x, double y, double z,
    const QColor& color, double size) {
  render(text, Point(x, y, z), color, size);
}

void View::render(const Points<double, 3>& vertices, const QColor& color,
    double size, bool smooth, const Transformation& transformation) {
  saveTransformation();
  setTransformation(getTransformation() * transformation);
  render(vertices, color, size, smooth);
  restoreTransformation();
}
void View::render(const Line<double, 3>& edges, const QColor& color,
    const Transformation& transformation) {
  saveTransformation();
  setTransformation(getTransformation() * transformation);
  render(edges, color);
  restoreTransformation();
}

void View::map(Point& point) const {
  Size size = getSize();

  point[0] = 0.5*size[0]*(point[0]+1.0);
  point[1] = 0.5*size[1]*(point[1]+1.0);
  point[2] = 0.5*(point[2]+1.0);
}

void View::unmap(Point& point) const {
  Size size = getSize();

  point[0] = 2.0*point[0]/size[0]-1.0;
  point[1] = 2.0*point[1]/size[1]-1.0;
  point[2] = 2.0*point[2]-1.0;
}

void View::render() {
  emit prepare(*this);
  emit render(*this);
  emit cleanup(*this);
}

void View::flush() {
  Framework::sendPostedEvents(this, 0);
}
