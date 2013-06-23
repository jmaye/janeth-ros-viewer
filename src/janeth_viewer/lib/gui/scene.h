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

#ifndef SCENE_H
#define SCENE_H

#include <QtCore/QObject>

#include "gui/view.h"

class Scene :
  public QObject {
Q_OBJECT
public:
  typedef View::Transformation Transformation;

  typedef View::Translation Translation;
  typedef View::Rotation Rotation;
  typedef View::Scale Scale;

  typedef View::Point Point;
  typedef View::Vertex Vertex;

  Scene();
  ~Scene();

  void setTranslation(const Translation& translation);
  void setTranslation(double x, double y, double z);
  const Translation& getTranslation() const;

  void setRotation(const Rotation& rotation);
  void setRotation(double yaw, double pitch, double roll);
  const Rotation& getRotation() const;

  void setScale(double scale);
  double getScale() const;

  Transformation getTransformation() const;

  void transform(Point& point) const;
  void untransform(Point& point) const;
  
  void setup(View& view);
protected:
  Translation translation;
  Rotation rotation;
  double scale;
signals:
  void translationChanged(const Scene& scene);
  void rotationChanged(const Scene& scene);
  void scaleChanged(const Scene& scene);
};

#endif
