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

#ifndef POLAR3D_H
#define POLAR3D_H

#include <eigen3/Eigen/Dense>

template <typename T, size_t K> class Polar;

template <typename T> class Polar<T, 3> {
public:
  typedef Eigen::Matrix<T, 3, 1> Cartesian;

  inline Polar(const T& phi = T(0), const T& theta = T(0), const T&
    radius = T(0));
  inline Polar(const Cartesian& cartesian);
  inline Polar(const Polar<T, 3>& src);
  inline ~Polar();

  inline void setPhi(const T& phi);
  inline const T& getPhi() const;
  inline void setTheta(const T& theta);
  inline const T& getTheta() const;
  inline void setRadius(const T& radius);
  inline const T& getRadius() const;

  inline const T& getAngle(int k) const;

  inline Polar<T, 3>& operator=(const Cartesian& cartesian);
  inline Polar<T, 3>& operator=(const Polar<T, 3>& src);

  inline operator Cartesian() const;
  inline const Polar<T, 3>& toCartesian(Cartesian& cartesian) const;

  inline static T correctAngle(const T& angle);
protected:
  T phi;
  T theta;
  T radius;
};

#include "utils/polar3d.tpp"

#endif
