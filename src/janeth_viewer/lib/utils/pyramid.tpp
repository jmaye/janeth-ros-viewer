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

template <typename T, size_t K>
template <typename... P> Pyramid<T, K>::Pyramid(const P&... parameters) :
  Geometry<T, K>(parameters...) {
}

template <typename T, size_t K>
Pyramid<T, K>::~Pyramid() {
}

/*****************************************************************************/
/* Methods                                                                   */
/*****************************************************************************/

template <typename T, size_t K>
Pyramid<T, K>& Pyramid<T, K>::operator=(const Pyramid<T, K>& src) {
  Geometry<T, K>::operator=(src);
  return *this;
}
