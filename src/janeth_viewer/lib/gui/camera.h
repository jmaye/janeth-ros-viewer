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

#ifndef CAMERA_H
#define CAMERA_H

#include <QtCore/QObject>

#include "gui/view.h"

class Camera :
  public QObject {
Q_OBJECT
public:
  typedef View::Projection Projection;
  typedef View::Transformation Transformation;

  typedef View::Point Point;
  typedef View::Vertex Vertex;

  typedef Eigen::Matrix<double, 3, 1> Position;
  typedef Eigen::Matrix<double, 3, 1> Viewpoint;
  typedef Eigen::Matrix<double, 2, 1> Range;

  enum Mode {
    orthogonal,
    perspective
  };

  Camera(Mode mode = orthogonal);
  ~Camera();

  void setMode(Mode mode);
  Mode getMode() const;

  void setPosition(const Position& position);
  void setPosition(double x, double y, double z);
  const Position& getPosition() const;

  void setViewpoint(const Viewpoint& viewpoint);
  void setViewpoint(double x, double y, double z);
  const Viewpoint& getViewpoint() const;

  void setRange(const Range& range);
  void setRange(double near, double far);
  const Range& getRange() const;

  Projection getProjection(double aspectRatio) const;
  Transformation getTransformation() const;
  double getViewpointDistance() const;

  void project(Point& point, double aspectRatio) const;
  void unproject(Point& point, double aspectRatio) const;

  void setup(View& view, double aspectRatio);
protected:
  Position position;
  Viewpoint viewpoint;
  Range range;

  Mode mode;
signals:
  void modeChanged(const Camera& camera);
  void positionChanged(const Camera& camera);
  void viewpointChanged(const Camera& camera);
  void rangeChanged(const Camera& camera);
};

#endif
