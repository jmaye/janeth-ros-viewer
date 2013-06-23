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

#ifndef GEOMETRY3D_H
#define GEOMETRY3D_H

#include <eigen3/Eigen/Dense>

template <typename T, size_t K> class Geometry;

template <typename T> class Geometry<T, 3> {
public:
  typedef Eigen::Matrix<T, 3, 1> Origin;
  typedef Eigen::Matrix<T, 3, 1> Orientation;
  typedef Eigen::Matrix<T, 3, 1> Size;

  inline Geometry(const Origin& origin = Origin::Zero(), const Orientation&
    orientation = Orientation::Zero(), const Size& size = Size::Ones());
  inline Geometry(const Geometry<T, 3>& src);
  inline ~Geometry();

  inline void setOrigin(const Origin& origin);
  inline const Origin& getOrigin() const;
  inline void setOrientation(const Orientation& orientation);
  inline const Orientation& getOrientation() const;
  inline void setSize(const Size& size);
  inline const Size& getSize() const;

  inline Geometry<T, 3>& operator=(const Geometry<T, 3>& src);
protected:
  Origin origin;
  Orientation orientation;
  Size size;
};

#include "utils/geometry3d.tpp"

#endif
