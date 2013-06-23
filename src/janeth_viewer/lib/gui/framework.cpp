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

#include "gui/framework.h"

#include "config.h"

/*****************************************************************************/
/* Statics                                                                   */
/*****************************************************************************/

Framework* Framework::instance = 0;

/*****************************************************************************/
/* Constructors and Destructor                                               */
/*****************************************************************************/

Framework::Framework(int& argc, char** argv) :
  QApplication(argc, argv),
  projectName(PROJECT_NAME),
  projectMajorVersion(PROJECT_MAJOR),
  projectMinorVersion(PROJECT_MINOR),
  projectPatchVersion(PROJECT_PATCH),
  projectRelease(PROJECT_RELEASE),
  projectBuildType(PROJECT_BUILD_TYPE),
  window(0) {
  if (!instance) {
    for (int i = 1; i < argc; ++i) {
      QString argument = argv[i];

      if (argument.startsWith("-")) {
        QString key;

        if (argument.startsWith("--") && (argument.length() > 3)) {
          key = argument.right(2);

          if (i+1 < argc) {
            keyedArguments[key] = argv[i+1];
            ++i;
          }
          else
            throw std::runtime_error("Missing argument value");
        }
        else if (argument.length() == 2) {
          key = argument.right(1);
          keyedArguments[key] = "";
        }
        else
          throw std::runtime_error("Bad argument format");
      }
      else
        plainArguments.push_back(argument);
    }

    instance = this;
  }
  else
    throw std::runtime_error("Duplicate framework");
}

Framework::~Framework() {
  instance = 0;
}

/*****************************************************************************/
/* Accessors                                                                 */
/*****************************************************************************/

Framework& Framework::getInstance() {
  if (instance)
    return *instance;
  else
    throw std::runtime_error("Missing framework");
}

const QString& Framework::getProjectName() const {
  return projectName;
}

size_t Framework::getProjectMajorVersion() const {
  return projectMajorVersion;
}

size_t Framework::getProjectMinorVersion() const {
  return projectMinorVersion;
}

size_t Framework::getProjectPatchVersion() const {
  return projectPatchVersion;
}

const QString& Framework::getProjectRelease() const {
  return projectRelease;
}

const QString& Framework::getProjectBuildType() const {
  return projectBuildType;
}

QString Framework::getProjectFullName() const {
  return QString().sprintf("%s %d.%d.%d (%s Build)",
    projectName.toAscii().constData(),
    (int)projectMajorVersion, (int)projectMinorVersion,   
    (int)projectPatchVersion, projectBuildType.toAscii().constData());
}

size_t Framework::getNumArguments() const {
  return plainArguments.size();
}

bool Framework::hasArgument() const {
  return !plainArguments.empty();
}

bool Framework::hasArgument(const QString& key) const {
  return (keyedArguments.find(key) != keyedArguments.end());
}

const QString& Framework::operator[](int i) const {
  if ((i >= 0) && (i < plainArguments.size()))
    return plainArguments[i];
  else
    throw std::runtime_error("Bad argument");
}

const QString& Framework::operator[](const QString& key) const {
  std::map<QString, QString>::const_iterator it = keyedArguments.find(key);

  if (it != keyedArguments.end())
    return it->second;
  else
    throw std::runtime_error("Bad argument");
}

MainWindow& Framework::getWindow() {
  if (window)
    return *window;
  else
    throw std::runtime_error("Bad window");
}

const MainWindow& Framework::getWindow() const {
  if (window)
    return *window;
  else
    throw std::runtime_error("Bad window");
}

/*****************************************************************************/
/* Methods                                                                   */
/*****************************************************************************/

void Framework::registerWindow(MainWindow* window) {
  if (!this->window) {
    this->window = window;
    emit windowRegistered(*window);
  }
  else
    throw std::runtime_error("Duplicate window");
}

void Framework::registerWidget(Widget* widget) {
  std::set<Widget*>::const_iterator it = widgets.find(widget);

  if (it == widgets.end()) {
    widgets.insert(widget);
    emit widgetRegistered(*widget);
  }
  else
    throw std::runtime_error("Duplicate widget");
}
