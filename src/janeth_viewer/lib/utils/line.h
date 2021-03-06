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

#ifndef LINE_H
#define LINE_H

#include "utils/points.h"

template <typename T, size_t K> class Line :
  public Points<T, K> {
public:
  typedef typename Points<T, K>::Point Point;

  inline Line(size_t numPoints = 2, bool loop = false);
  inline Line(const Point& startPoint, const Point& endPoint);
  inline Line(const Line<T, K>& src);
  inline ~Line();

  inline size_t getNumSegments() const;
  inline size_t getNumPoints() const;
  inline void setLoop(bool loop);
  inline bool isLoop() const;

  inline Point& operator[](int i);
  inline const Point& operator[](int i) const;

  inline Line<T, K>& operator=(const Line<T, K>& src);
protected:
  bool loop;
};

#include "utils/line.tpp"

#endif
