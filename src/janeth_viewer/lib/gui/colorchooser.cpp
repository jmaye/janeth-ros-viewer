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

#include <QtGui/QLabel>
#include <QtGui/QFrame>
#include <QtGui/QPushButton>
#include <QtGui/QColorDialog>

#include "gui/colorchooser.h"

/*****************************************************************************/
/* Constructors and Destructor                                               */
/*****************************************************************************/

ColorChooser::ColorChooser(QWidget* parent) :
  QWidget(parent),
  layout(this),
  palette(0) {
  layout.setContentsMargins(0, 0, 0, 0);
    
  connect(&mapper, SIGNAL(mapped(const QString&)), this,
    SLOT(buttonClicked(const QString&)));
}

ColorChooser::~ColorChooser() {
}

/*****************************************************************************/
/* Accessors                                                                 */
/*****************************************************************************/

void ColorChooser::setPalette(Palette* palette) {
  while (layout.count()) {
    if (layout.itemAt(0)->widget())
      delete layout.itemAt(0)->widget();
  }
  if (this->palette)
    this->palette->disconnect(
      SIGNAL(colorChanged(const QString&, const QColor&)));
  
  this->palette = palette;
  
  if (this->palette) {
    connect(this->palette, SIGNAL(colorChanged(const QString&, const QColor&)),
      this, SLOT(colorChanged(const QString&, const QColor&)));
    for (Palette::Iterator it = this->palette->getColorBegin();
        it != this->palette->getColorEnd(); ++it)
      colorChanged(this->palette->getRole(it), this->palette->getColor(it));
  }
}

Palette* ColorChooser::getPalette() const {
  return palette;
}

/*****************************************************************************/
/* Methods                                                                   */
/*****************************************************************************/

void ColorChooser::buttonClicked(const QString& role) {
  QColor color = QColorDialog::getColor(palette->getColor(role), this,
    "Choose "+role+" Color");
  if (color.isValid())
    palette->setColor(role, color);
}

void ColorChooser::colorChanged(const QString& role, const QColor& color) {
  if (!mapper.mapping(role)) {
    int row = layout.rowCount();

    QLabel* label = new QLabel(role+" color:", this);
    QFrame* frame = new QFrame(this);
    frame->setAutoFillBackground(true);
    frame->setFrameShape(QFrame::Box);
    frame->setFrameShadow(QFrame::Raised);
    frame->setMinimumWidth(50);
    QPushButton* button = new QPushButton("Choose...", this);

    layout.addWidget(label, row, 0);
    layout.setColumnStretch(1, 1);
    layout.addWidget(frame, row, 2);
    layout.addWidget(button, row, 3);

    connect(button, SIGNAL(clicked()), &mapper, SLOT(map()));
    mapper.setMapping(button, role);
  }

  QPalette palette;
  palette.setColor(QPalette::Window, color);
  int index = layout.indexOf((QWidget*)mapper.mapping(role));
  QWidget* frame = layout.itemAt(index-1)->widget();
  frame->setPalette(palette);
}
