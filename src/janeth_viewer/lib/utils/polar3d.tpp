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
Polar<T, 3>::Polar(const T& phi, const T& theta, const T& radius) :
  phi(phi),
  theta(theta),
  radius(radius) {
}

template <typename T>
Polar<T, 3>::Polar(const Cartesian& cartesian) {
  operator=(cartesian);
}

template <typename T>
Polar<T, 3>::Polar(const Polar<T, 3>& src) :
  phi(src.phi),
  theta(src.theta),
  radius(src.radius) {
}

template <typename T>
Polar<T, 3>::~Polar() {
}

/*****************************************************************************/
/* Accessors                                                                 */
/*****************************************************************************/

template <typename T>
void Polar<T, 3>::setPhi(const T& phi) {
  this->phi = correctAngle(phi);
}

template <typename T>
const T& Polar<T, 3>::getPhi() const {
  return phi;
}

template <typename T>
void Polar<T, 3>::setTheta(const T& theta) {
  this->theta = correctAngle(theta);
}

template <typename T>
const T& Polar<T, 3>::getTheta() const {
  return theta;
}

template <typename T>
void Polar<T, 3>::setRadius(const T& radius) {
  this->radius = radius;
}

template <typename T>
const T& Polar<T, 3>::getRadius() const {
  return radius;
}

template <typename T>
const T& Polar<T, 3>::getAngle(int k) const {
  if (!k)
    return phi;
  else if (k == 1)
    return theta;
  else
    throw std::runtime_error("Bad angle");
}

/*****************************************************************************/
/* Methods                                                                   */
/*****************************************************************************/

template <typename T>
Polar<T, 3>& Polar<T, 3>::operator=(const Cartesian& cartesian) {
  phi = atan2(cartesian[1], cartesian[0]);
  radius = sqrt(cartesian[0]*cartesian[0]+cartesian[1]*cartesian[1]+
    cartesian[2]*cartesian[2]);
  theta = asin(cartesian[2]/radius);
  
  return *this;
}

template <typename T>
Polar<T, 3>& Polar<T, 3>::operator=(const Polar<T, 3>& src) {
  phi = src.phi;
  theta = src.theta;
  radius = src.radius;

  return *this;
}

template <typename T>
Polar<T, 3>::operator Cartesian() const {
  Cartesian cartesian;
  toCartesian(cartesian);

  return cartesian;
}

template <typename T>
const Polar<T, 3>& Polar<T, 3>::toCartesian(Cartesian& cartesian) const {
  cartesian[0] = radius*cos(phi)*cos(theta);
  cartesian[1] = radius*sin(phi)*cos(theta);
  cartesian[2] = radius*sin(theta);

  return *this;
}

template <typename T>
T Polar<T, 3>::correctAngle(const T& angle) {
  return Polar<T, 2>::correctAngle(angle);
}
