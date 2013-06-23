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
Line<T, K>::Line(size_t numPoints, bool loop) :
  Points<T, K>(numPoints),
  loop(loop) {
}

template <typename T, size_t K>
Line<T, K>::Line(const Point& startPoint, const Point& endPoint) :
  loop(false) {
  this->points.push_back(startPoint);
  this->points.push_back(endPoint);
}

template <typename T, size_t K>
Line<T, K>::Line(const Line<T, K>& src) :
  Points<T, K>(src),
  loop(src.loop) {
}

template <typename T, size_t K>
Line<T, K>::~Line() {
}

/*****************************************************************************/
/* Accessors                                                                 */
/*****************************************************************************/

template <typename T, size_t K>
size_t Line<T, K>::getNumSegments() const {
  if (this->points.size()) {
    if (loop)
      return this->points.size();
    else
      return this->points.size()-1;
  }
  else
    return 0;
}

template <typename T, size_t K>
size_t Line<T, K>::getNumPoints() const {
  if (this->points.size())
    return this->points.size()+loop;
  else
    return 0;
}

template <typename T, size_t K>
void Line<T, K>::setLoop(bool loop) {
  this->loop = loop;
}

template <typename T, size_t K>
bool Line<T, K>::isLoop() const {
  return loop;
}

template <typename T, size_t K>
typename Line<T, K>::Point& Line<T, K>::operator[](int i) {
  if ((i >= 0) && (i < this->points.size()+loop))
    return this->points[i % this->points.size()];
  else
    throw std::runtime_error("Bad point");
}

template <typename T, size_t K>
const typename Line<T, K>::Point& Line<T, K>::operator[](int i) const {
  if ((i >= 0) && (i < this->points.size()+loop))
    return this->points[i % this->points.size()];
  else
    throw std::runtime_error("Bad point");
}

/*****************************************************************************/
/* Methods                                                                   */
/*****************************************************************************/

template <typename T, size_t K>
Line<T, K>& Line<T, K>::operator=(const Line<T, K>& src) {
  Points<T, K>::operator=(src);
  loop = src.loop;

  return *this;
}
