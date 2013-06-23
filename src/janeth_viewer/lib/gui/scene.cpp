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

#include "gui/scene.h"

#include "utils/polar.h"

/*****************************************************************************/
/* Constructors and Destructor                                               */
/*****************************************************************************/

Scene::Scene() :
  translation(Translation::Zero()),
  rotation(Rotation::Zero()),
  scale(1.0) {
}

Scene::~Scene() {
}

/*****************************************************************************/
/* Accessors                                                                 */
/*****************************************************************************/

void Scene::setTranslation(const Translation& translation) {
  if (this->translation != translation) {
    this->translation = translation;
    emit translationChanged(*this);
  }
}

void Scene::setTranslation(double x, double y, double z) {
  setTranslation(Translation(x, y, z));
}

const Scene::Translation& Scene::getTranslation() const {
  return translation;
}

void Scene::setRotation(const Rotation& rotation) {
  Rotation corrected;
  for (int k = 0; k < 3; ++k)
    corrected[k] = Polar<double, 3>::correctAngle(rotation[k]);

  if (this->rotation != corrected) {
    this->rotation = corrected;
    emit rotationChanged(*this);
  }
}

void Scene::setRotation(double yaw, double pitch, double roll) {
  setRotation(Rotation(yaw, pitch, roll));
}

const Scene::Rotation& Scene::getRotation() const {
  return rotation;
}

void Scene::setScale(double scale) {
  if (this->scale != scale) {
    this->scale = scale;
    emit scaleChanged(*this);
  }
}

double Scene::getScale() const {
  return scale;
}

Scene::Transformation Scene::getTransformation() const {
  Eigen::AngleAxis<double> R_yaw(rotation[0],
    Eigen::Matrix<double, 3, 1>::UnitZ());
  Eigen::AngleAxis<double> R_pitch(rotation[1],
    Eigen::Matrix<double, 3,1>::UnitY());
  Eigen::AngleAxis<double> R_roll(rotation[2],
    Eigen::Matrix<double, 3, 1>::UnitX());

  return Eigen::Scaling(scale)*R_roll*R_pitch*R_yaw*
    Eigen::Translation<double, 3>(translation);
}

/*****************************************************************************/
/* Methods                                                                   */
/*****************************************************************************/

void Scene::transform(Point& point) const {
  Vertex v = Vertex::Ones();
  v.segment(0, 3) = point;
  v = getTransformation()*v;

  point = v.segment(0, 3);
}

void Scene::untransform(Point& point) const {
  Vertex v = Vertex::Ones();
  v.segment(0, 3) = point;
  v = getTransformation().inverse()*v;

  point = v.segment(0, 3);
}

void Scene::setup(View& view) {
  view.transform(getTransformation());
}
