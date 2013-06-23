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

template <class V> V& MainWindow::getView() {
  for (typename std::map<QString, View*>::iterator it = views.begin();
      it != views.end(); ++it)
    if (it->second->inherits(V::staticMetaObject.className()))
      return *(V*)it->second;

  throw std::runtime_error("Bad view");
}

template <class V> const V& MainWindow::getView() const {
  for (typename std::map<QString, View*>::const_iterator it = views.begin();
      it != views.end(); ++it)
    if (it->second->inherits(V::staticMetaObject.className()))
      return *(V*)it->second;

  throw std::runtime_error("Bad view");
}

template <class C> C& MainWindow::getControl() {
  for (typename std::map<QString, Control*>::iterator it = controls.begin();
      it != controls.end(); ++it)
    if (it->second->inherits(C::staticMetaObject.className()))
      return *(C*)it->second;

  throw std::runtime_error("Bad control");
}

template <class C> const C& MainWindow::getControl() const {
  for (typename std::map<QString, Control*>::const_iterator
      it = controls.begin(); it != controls.end(); ++it)
    if (it->second->inherits(C::staticMetaObject.className()))
      return *(C*)it->second;

  throw std::runtime_error("Bad control");
}

/*****************************************************************************/
/* Methods                                                                   */
/*****************************************************************************/

template <class V, typename... P> void MainWindow::addView(const QString&
    title, const P&... parameters) {
  V* view = new V(parameters...);
  addView(title, *view);
}

template <class C, typename... P> void MainWindow::addControl(const QString&
    title, const P&... parameters) {
  C* control = new C(parameters...);
  addControl(title, *control);
}
