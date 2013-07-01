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

#include <stdexcept>

/*****************************************************************************/
/* Constructors and Destructor                                               */
/*****************************************************************************/

template <typename T, size_t K>
Points<T, K>::Points(size_t numPoints) :
  points(numPoints) {
}

template <typename T, size_t K>
Points<T, K>::Points(const Points<T, K>& src) :
  points(src.points) {
}

template <typename T, size_t K>
Points<T, K>::~Points() {
}

/*****************************************************************************/
/* Accessors                                                                 */
/*****************************************************************************/

template <typename T, size_t K>
void Points<T, K>::setNumPoints(size_t numPoints) {
  points.resize(numPoints);
}

template <typename T, size_t K>
size_t Points<T, K>::getNumPoints() const {
  return points.size();
}

template <typename T, size_t K>
std::vector<typename Points<T, K>::Point> Points<T, K>::getPoints() {
  return points;
}

template <typename T, size_t K>
const std::vector<typename Points<T, K>::Point> Points<T, K>::getPoints()
    const {
  return points;
}

template <typename T, size_t K>
typename Points<T, K>::Point& Points<T, K>::operator[](int i) {
  if ((i >= 0) && (i < points.size()))
    return points[i];
  else
    throw std::runtime_error("Bad point");
}

template <typename T, size_t K>
const typename Points<T, K>::Point& Points<T, K>::operator[](int i) const {
  if ((i >= 0) && (i < points.size()))
    return points[i];
  else
    throw std::runtime_error("Bad point");
}

/*****************************************************************************/
/* Methods                                                                   */
/*****************************************************************************/

template <typename T, size_t K>
Points<T, K>& Points<T, K>::operator=(const Points<T, K>& src) {
  points = src.points;
  return *this;
}

template <typename T, size_t K>
Points<T, K>& Points<T, K>::operator+=(const Point& point) {
  points.push_back(point);
  return *this;
}

template <typename T, size_t K>
void Points<T, K>::clear() {
  points.clear();
}
