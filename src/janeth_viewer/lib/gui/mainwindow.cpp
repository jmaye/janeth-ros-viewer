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

#include "gui/mainwindow.h"

#include "gui/framework.h"

#include "ui_mainwindow.h"

/*****************************************************************************/
/* Constructors and Destructor                                               */
/*****************************************************************************/

MainWindow::MainWindow(const QString& subtitle) :
  ui(new Ui_MainWindow()),
  viewActions(this),
  controlActions(this) {
  ui->setupUi(this);

  setSubtitle(subtitle);

  while (ui->viewToolBox->count())
    ui->viewToolBox->removeItem(0);
  while (ui->controlToolBox->count())
    ui->controlToolBox->removeItem(0);

  Framework::getInstance().registerWindow(this);

  ui->menuView->addMenu("Views");
  ui->menuView->addMenu("Controls");

  connect(&viewActions, SIGNAL(triggered(QAction*)), this,
    SLOT(viewTriggered(QAction*)));
  connect(&controlActions, SIGNAL(triggered(QAction*)), this,
    SLOT(controlTriggered(QAction*)));
}

MainWindow::~MainWindow() {
  delete ui;
}

/*****************************************************************************/
/* Accessors                                                                 */
/*****************************************************************************/

void MainWindow::setSubtitle(const QString& subtitle) {
  this->subtitle = subtitle;

  setWindowTitle(Framework::getInstance().getProjectFullName());
  if (!this->subtitle.isEmpty())
    setWindowTitle(windowTitle()+" - "+this->subtitle);
}

const QString& MainWindow::getSubtitle() const {
  return subtitle;
}

/*****************************************************************************/
/* Methods                                                                   */
/*****************************************************************************/

void MainWindow::addView(const QString& title, View& view) {
  QAction* viewAction =
    ui->menuView->actions().at(0)->menu()->addAction(title);

  viewAction->setShortcut(Qt::Key_F1+views.size());
  viewAction->setCheckable(true);
  viewActions.addAction(viewAction);

  if (!view.getMenu().isEmpty()) {
    if (views.empty())
      ui->menuView->addSeparator();
    ui->menuView->addMenu(&view.getMenu())->setText(title);
  }

  int viewIndex = ui->viewToolBox->addItem(&view, title);
  viewAction->setData(viewIndex);
  views.insert(std::make_pair(title, &view));

  Framework::getInstance().registerWidget(&view);
  emit viewAdded(view);
}

void MainWindow::addControl(const QString& title, Control& control) {
  QAction* controlAction =
    ui->menuView->actions().at(1)->menu()->addAction(title);

  controlAction->setShortcut(Qt::SHIFT+Qt::Key_F1+controls.size());
  controlAction->setCheckable(true);
  controlActions.addAction(controlAction);

  if (!control.getMenu().isEmpty())
    ui->menuBar->addMenu(&control.getMenu())->setText(title);

  int controlIndex = ui->controlToolBox->addItem(&control, title);
  controlAction->setData(controlIndex);
  controls.insert(std::make_pair(title, &control));

  Framework::getInstance().registerWidget(&control);
  emit controlAdded(control);
}

void MainWindow::viewChanged(int index) {
  if ((index >= 0) && (index < viewActions.actions().size()))
    viewActions.actions().at(index)->setChecked(true);
}

void MainWindow::controlChanged(int index) {
  if ((index >= 0) && (index < controlActions.actions().size()))
    controlActions.actions().at(index)->setChecked(true);
}

void MainWindow::viewTriggered(QAction* action) {
  ui->viewToolBox->setCurrentIndex(action->data().toInt());
}

void MainWindow::controlTriggered(QAction* action) {
  ui->controlToolBox->setCurrentIndex(action->data().toInt());
}
