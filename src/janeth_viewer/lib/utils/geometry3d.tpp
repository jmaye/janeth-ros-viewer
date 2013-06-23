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

/*****************************************************************************/
/* Constructors and Destructor                                               */
/*****************************************************************************/

template <typename T>
Geometry<T, 3>::Geometry(const Origin& origin, const Orientation&
    orientation, const Size& size) :
  origin(origin),
  orientation(orientation),
  size(size) {
}

template <typename T>
Geometry<T, 3>::Geometry(const Geometry<T, 3>& src) :
  origin(src.origin),
  orientation(src.orientation),
  size(src.size) {
}

template <typename T>
Geometry<T, 3>::~Geometry() {
}

/*****************************************************************************/
/* Accessors                                                                 */
/*****************************************************************************/

template <typename T>
void Geometry<T, 3>::setOrigin(const Origin& origin) {
  this->origin = origin;
}

template <typename T>
const typename Geometry<T, 3>::Origin& Geometry<T, 3>::getOrigin() const {
  return origin;
}

template <typename T>
void Geometry<T, 3>::setOrientation(const Orientation& orientation) {
  this->orientation = orientation;
}

template <typename T>
const typename Geometry<T, 3>::Orientation& Geometry<T, 3>::getOrientation()
    const {
  return orientation;
}

template <typename T>
void Geometry<T, 3>::setSize(const Size& size) {
  this->size = size;
}

template <typename T>
const typename Geometry<T, 3>::Size& Geometry<T, 3>::getSize() const {
  return size;
}

/*****************************************************************************/
/* Methods                                                                   */
/*****************************************************************************/

template <typename T>
Geometry<T, 3>& Geometry<T, 3>::operator=(const Geometry<T, 3>& src) {
  origin = src.origin;
  orientation = src.orientation;
  size = src.size;

  return *this;
}
