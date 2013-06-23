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

#include "gui/palette.h"

/*****************************************************************************/
/* Constructors and Destructor                                               */
/*****************************************************************************/

Palette::Palette() {
}

Palette::~Palette() {
}

/*****************************************************************************/
/* Accessors                                                                 */
/*****************************************************************************/

Palette::Iterator Palette::getColorBegin() const {
  return colors.begin();
}

Palette::Iterator Palette::getColorEnd() const {
  return colors.end();
}

const QString& Palette::getRole(const Iterator& it) const {
  return it->first;
}

void Palette::setColor(const QString& role, const QColor& color) {
  if (colors[role] != color) {
    colors[role] = color;
    emit colorChanged(role, color);
  }
}

const QColor& Palette::getColor(const Iterator& it) const {
  return it->second;
}

const QColor& Palette::getColor(const QString& role) const {
  std::map<QString, QColor>::const_iterator it = colors.find(role);

  if (it != colors.end())
    return it->second;
  else
    throw std::runtime_error("Color role undefined");
}
