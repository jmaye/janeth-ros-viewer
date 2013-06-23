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

#ifndef SCENECONTROL_H
#define SCENECONTROL_H

#include <map>

#include "gui/control.h"

#include "gui/palette.h"
#include "gui/view.h"
#include "gui/camera.h"
#include "gui/scene.h"

class Ui_SceneControl;

class SceneControl :
  public Control {
Q_OBJECT
public:
  SceneControl(bool showFog = true, bool showGround = true, bool
    showAxes = true);
  ~SceneControl();

  Camera& getCamera();
  const Camera& getCamera() const;
  Scene& getScene();
  const Scene& getScene() const;

  void setBackgroundColor(const QColor& color);
  void setFogColor(const QColor& color);
  void setGroundColor(const QColor& color);
  void setAxesColor(const QColor& color);
  
  void setGroundRadius(double radius);
  void setGroundElevation(double elevation);

  void setShowFog(bool showFog);
  void setShowGround(bool showGround);
  void setShowAxes(bool showAxes);

  void setRotateFrames(bool rotateFrames);
  void setFrameRotation(double yaw);
  void setCameraFollow(bool follow);
protected:
  Ui_SceneControl* ui;

  Palette palette;
  Camera camera;
  Scene scene;

  void renderBackground(View& view, const QColor& color);
  void renderFog(View& view, const QColor& color, double start, double end,
    double density);
  void renderGround(View& view, const QColor& color, double radius, double
    elevation, double angleStep, double rangeStep);
  void renderAxes(View& view, const QColor& color, double length);
protected slots:
  void cameraPositionChanged();
  void cameraViewpointChanged();
  void cameraModeChanged(int mode);
  void cameraRangeChanged();
  void sceneTranslationChanged();
  void sceneRotationChanged();
  void sceneScaleChanged();
  void groundRadiusChanged(double radius);
  void groundElevationChanged(double elevation);

  void showFogToggled(bool checked);
  void showGroundToggled(bool checked);
  void showAxesToggled(bool checked);

  void colorChanged(const QString& role, const QColor& color);

  void cameraPositionChanged(const Camera& camera);
  void cameraViewpointChanged(const Camera& camera);
  void cameraRangeChanged(const Camera& camera);
  void cameraModeChanged(const Camera& camera);

  void sceneTranslationChanged(const Scene& scene);
  void sceneRotationChanged(const Scene& scene);
  void sceneScaleChanged(const Scene& scene);

  void transformView(View& view, const QPoint& fromMouse, const QPoint&
    toMouse, int wheel, Qt::MouseButtons mouseButtons);
  void prepareView(View& view);
  void renderView(View& view);

  void poseUpdate(double x, double y, double z, double yaw, double pitch,
    double roll);
};

#endif
