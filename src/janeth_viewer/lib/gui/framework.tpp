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
/* Accessors                                                                 */
/*****************************************************************************/

template <class C> C& Framework::getWidget() {
  for (std::set<Widget*>::iterator it = widgets.begin();
      it != widgets.end(); ++it)
    if ((*it)->inherits(C::staticMetaObject.className()))
      return *((C*)*it);

  throw std::runtime_error("Bad widget");
}

template <class C> const C& Framework::getWidget() const {
  for (std::set<Widget*>::const_iterator it = widgets.begin();
      it != widgets.end(); ++it)
    if ((*it)->inherits(C::staticMetaObject.className()))
      return *((const C*)*it);

  throw std::runtime_error("Bad widget");
}
