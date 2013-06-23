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

#include <cmath>

#include "gui/camera.h"

/*****************************************************************************/
/* Constructors and Destructor                                               */
/*****************************************************************************/

Camera::Camera(Mode mode) :
  position(Position::Zero()),
  viewpoint(Viewpoint::Zero()),
  range(0.1, 1.0),
  mode(mode) {
}

Camera::~Camera() {
}

/*****************************************************************************/
/* Accessors                                                                 */
/*****************************************************************************/

void Camera::setMode(Mode mode) {
  if (this->mode != mode) {
    this->mode = mode;
    emit modeChanged(*this);
  }
}

Camera::Mode Camera::getMode() const {
  return mode;
}

void Camera::setPosition(const Position& position) {
  if (this->position != position) {
    this->position = position;
    emit positionChanged(*this);
  }
}

void Camera::setPosition(double x, double y, double z) {
  setPosition(Position(x, y, z));
}

const Camera::Position& Camera::getPosition() const {
  return position;
}

void Camera::setViewpoint(const Viewpoint& viewpoint) {
  if (this->viewpoint != viewpoint) {
    this->viewpoint = viewpoint;
    emit viewpointChanged(*this);
  }
}

void Camera::setViewpoint(double x, double y, double z) {
  setViewpoint(Viewpoint(x, y, z));
}

const Camera::Viewpoint& Camera::getViewpoint() const {
  return viewpoint;
}

void Camera::setRange(const Range& range) {
  if (this->range != range) {
    this->range = range;
    emit rangeChanged(*this);
  }
}

void Camera::setRange(double near, double far) {
  setRange(Range(near, far));
}

const Camera::Range& Camera::getRange() const {
  return range;
}

Camera::Projection Camera::getProjection(double aspectRatio) const {
  Projection projection = Projection::Identity();

  double near = range[0];
  double far = range[1];
  double fov = 45.0*M_PI/180.0;

  double top = tan(fov*0.5)*near;
  double bottom = -top;
  double left = aspectRatio*bottom;
  double right = aspectRatio*top;

  if (mode == perspective) {
    projection(0, 0) = 2.0*near/(right-left);
    projection(1, 1) = 2.0*near/(top-bottom);
    projection(2, 2) = -(far+near)/(far-near);
    projection(2, 3) = -2.0*far*near/(far-near);
    projection(3, 2) = -1.0;
  }
  else {
    projection(0, 0) = 2.0/(right-left);
    projection(1, 1) = 2.0/(top-bottom);
    projection(2, 2) = -2.0/(far-near);
    projection(0, 3) = -(right+left)/(right-left);
    projection(1, 3) = -(top+bottom)/(top-bottom);
    projection(2, 3) = -(far+near)/(far-near);
  }

  return projection;
}

Camera::Transformation Camera::getTransformation() const {
  Transformation transformation = Transformation::Identity();

  Point lookAt = viewpoint-position;
  lookAt.normalize();
  Point up(0.0, 0.0, 1.0);
  Point s = lookAt.cross(up);
  Point u = s.cross(lookAt);

  transformation(0, 0) = s[0];
  transformation(0, 1) = s[1];
  transformation(0, 2) = s[2];
  transformation(1, 0) = u[0];
  transformation(1, 1) = u[1];
  transformation(1, 2) = u[2];
  transformation(2, 0) = -lookAt[0];
  transformation(2, 1) = -lookAt[1];
  transformation(2, 2) = -lookAt[2];

  Transformation translation = Transformation::Identity();
  translation(0, 3) = -position[0];
  translation(1, 3) = -position[1];
  translation(2, 3) = -position[2];

  return transformation*translation;
}

double Camera::getViewpointDistance() const {
  return (position-viewpoint).norm();
}

/*****************************************************************************/
/* Methods                                                                   */
/*****************************************************************************/

void Camera::project(Point& point, double aspectRatio) const {
  Vertex v = Vertex::Ones();
  v.segment(0, 3) = point;
  v = getProjection(aspectRatio)*getTransformation()*v;

  point[0] = v[0]/v[3];
  point[1] = v[1]/v[3];
  point[2] = v[2]/v[3];
}

void Camera::unproject(Point& point, double aspectRatio) const {
  Vertex v = Vertex::Ones();
  v.segment(0, 3) = point;
  v = (getProjection(aspectRatio)*getTransformation()).inverse()*v;

  point[0] = v[0]/v[3];
  point[1] = v[1]/v[3];
  point[2] = v[2]/v[3];
}

void Camera::setup(View& view, double aspectRatio) {
  view.setProjection(getProjection(aspectRatio));
  view.setTransformation(getTransformation());
}
