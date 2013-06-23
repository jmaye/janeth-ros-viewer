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
#include <stdexcept>

/*****************************************************************************/
/* Constructors and Destructor                                               */
/*****************************************************************************/

template <typename T>
Polar<T, 2>::Polar(const T& theta, const T& radius) :
  theta(theta),
  radius(radius) {
}

template <typename T>
Polar<T, 2>::Polar(const Cartesian& cartesian) {
  operator=(cartesian);
}

template <typename T>
Polar<T, 2>::Polar(const Polar<T, 2>& src) :
  theta(src.theta),
  radius(src.radius) {
}

template <typename T>
Polar<T, 2>::~Polar() {
}

/*****************************************************************************/
/* Accessors                                                                 */
/*****************************************************************************/

template <typename T>
void Polar<T, 2>::setTheta(const T& theta) {
  this->theta = correctAngle(theta);
}

template <typename T>
const T& Polar<T, 2>::getTheta() const {
  return theta;
}

template <typename T>
void Polar<T, 2>::setRadius(const T& radius) {
  this->radius = radius;
}

template <typename T>
const T& Polar<T, 2>::getRadius() const {
  return radius;
}

template <typename T>
const T& Polar<T, 2>::getAngle(int k) const {
  if (!k)
    return theta;
  else
    throw std::runtime_error("Bad angle");
}

/*****************************************************************************/
/* Methods                                                                   */
/*****************************************************************************/

template <typename T>
Polar<T, 2>& Polar<T, 2>::operator=(const Cartesian& cartesian) {
  theta = atan2(cartesian[1], cartesian[0]);
  radius = sqrt(cartesian[0]*cartesian[0]+cartesian[1]*cartesian[1]);
  
  return *this;
}

template <typename T>
Polar<T, 2>& Polar<T, 2>::operator=(const Polar<T, 2>& src) {
  theta = src.theta;
  radius = src.radius;

  return *this;
}

template <typename T>
Polar<T, 2>::operator Cartesian() const {
  Cartesian cartesian;
  toCartesian(cartesian);

  return cartesian;
}

template <typename T>
const Polar<T, 2>& Polar<T, 2>::toCartesian(Cartesian& cartesian) const {
  cartesian[0] = radius*cos(theta);
  cartesian[1] = radius*sin(theta);

  return *this;
}

template <typename T>
T Polar<T, 2>::correctAngle(const T& angle) {
  T result = angle;

  if (result >= T(0))
    while (result >= T(M_PI)) result -= T(2)*T(M_PI);
  else
    while (result < -T(M_PI)) result += T(2)*T(M_PI);

  return result;
}
