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

#ifndef PALETTE_H
#define PALETTE_H

#include <QtCore/QObject>
#include <QtGui/QColor>

#include <map>

class Palette :
  public QObject {
Q_OBJECT
public:
  typedef std::map<QString, QColor>::const_iterator Iterator;

  Palette();
  ~Palette();

  Iterator getColorBegin() const;
  Iterator getColorEnd() const;
  
  const QString& getRole(const Iterator& it) const;
  
  void setColor(const QString& role, const QColor& color);
  const QColor& getColor(const Iterator& it) const;
  const QColor& getColor(const QString& role) const;
protected:
  std::map<QString, QColor> colors;
signals:
  void colorChanged(const QString& role, const QColor& color);
};

#endif
