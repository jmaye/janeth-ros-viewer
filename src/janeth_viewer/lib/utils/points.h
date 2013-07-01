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

#ifndef POINTS_H
#define POINTS_H

#include <vector>

#include <eigen3/Eigen/Dense>

template <typename T, size_t K> class Points {
public:
  typedef Eigen::Matrix<T, K, 1> Point;

  inline Points(size_t numPoints = 0);
  inline Points(const Points<T, K>& src);
  inline ~Points();

  inline void setNumPoints(size_t numPoints);
  inline size_t getNumPoints() const;
  inline std::vector<Point> getPoints();
  inline const std::vector<Point> getPoints() const;

  inline void clear();

  inline Point& operator[](int i);
  inline const Point& operator[](int i) const;

  inline Points<T, K>& operator=(const Points<T, K>& src);

  inline Points<T, K>& operator+=(const Point& point);
protected:
  std::vector<Point> points;
};

#include "utils/points.tpp"

#endif
