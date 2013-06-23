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

#include "gui/scenecontrol.h"

#include "gui/PoslvControl.h"

#include "ui_scenecontrol.h"

/*****************************************************************************/
/* Constructors and Destructor                                               */
/*****************************************************************************/

SceneControl::SceneControl(bool showFog, bool showGround, bool showAxes) :
  ui(new Ui_SceneControl()) {
  ui->setupUi(this);

  ui->colorChooser->setPalette(&palette);
  connect(&palette, SIGNAL(colorChanged(const QString&, const QColor&)),
    this, SLOT(colorChanged(const QString&, const QColor&)));
  
  connect(&camera, SIGNAL(positionChanged(const Camera&)), this,
    SLOT(cameraPositionChanged(const Camera&)));
  connect(&camera, SIGNAL(viewpointChanged(const Camera&)), this,
    SLOT(cameraViewpointChanged(const Camera&)));
  connect(&camera, SIGNAL(rangeChanged(const Camera&)), this,
    SLOT(cameraRangeChanged(const Camera&)));
  connect(&camera, SIGNAL(modeChanged(const Camera&)), this,
    SLOT(cameraModeChanged(const Camera&)));

  connect(&scene, SIGNAL(translationChanged(const Scene&)), this,
    SLOT(sceneTranslationChanged(const Scene&)));
  connect(&scene, SIGNAL(rotationChanged(const Scene&)), this,
    SLOT(sceneRotationChanged(const Scene&)));
  connect(&scene, SIGNAL(scaleChanged(const Scene&)), this,
    SLOT(sceneScaleChanged(const Scene&)));

  connect<View>(SIGNAL(transform(View&, const QPoint&, const QPoint&, int,
      Qt::MouseButtons)),
    SLOT(transformView(View&, const QPoint&, const QPoint&, int,
      Qt::MouseButtons)));
  connect<View>(SIGNAL(prepare(View&)), SLOT(prepareView(View&)));
  connect<View>(SIGNAL(render(View&)), SLOT(renderView(View&)));

  connect<PoslvControl>(
    SIGNAL(poseUpdate(double, double, double, double, double, double)),
    SLOT(poseUpdate(double, double, double, double, double, double)));

  camera.setPosition(-20.0, 0.0, 0.0);
  camera.setViewpoint(0.0, 0.0, 0.0);
  camera.setRange(0.1, 1000.0);
  camera.setMode(Camera::perspective);

  scene.setTranslation(0.0, 0.0, 0.0);
  scene.setRotation(-10.0*M_PI/180.0, -5.0*M_PI/180.0, 0.0);
  scene.setScale(1.0);

  setBackgroundColor(Qt::white);
  setFogColor(Qt::white);
  setGroundColor(Qt::lightGray);
  setAxesColor(Qt::black);

  setGroundRadius(30.0);
  
  setShowFog(showFog);
  setShowGround(showGround);
  setShowAxes(showAxes);

  setRotateFrames(false);
  setFrameRotation(1.0*M_PI/180.0);
}

SceneControl::~SceneControl() {
  delete ui;
}

/*****************************************************************************/
/* Accessors                                                                 */
/*****************************************************************************/

Camera& SceneControl::getCamera() {
  return camera;
}

const Camera& SceneControl::getCamera() const {
  return camera;
}

Scene& SceneControl::getScene() {
  return scene;
}

const Scene& SceneControl::getScene() const {
  return scene;
}

void SceneControl::setBackgroundColor(const QColor& color) {
  palette.setColor("Background", color);
}

void SceneControl::setFogColor(const QColor& color) {
  palette.setColor("Fog", color);
}

void SceneControl::setGroundColor(const QColor& color) {
  palette.setColor("Ground", color);
}

void SceneControl::setAxesColor(const QColor& color) {
  palette.setColor("Axes", color);
}

void SceneControl::setGroundRadius(double radius) {
  ui->groundXSpinBox->setValue(radius);
  ui->groundYSpinBox->setValue(radius);

  emit updateViews();
}

void SceneControl::setGroundElevation(double elevation) {
  ui->groundZSpinBox->setValue(elevation);
  emit updateViews();
}

void SceneControl::setShowFog(bool showFog) {
  ui->showFogCheckBox->setChecked(showFog);
  emit updateViews();
}

void SceneControl::setShowGround(bool showGround) {
  ui->showGroundCheckBox->setChecked(showGround);
  emit updateViews();
}

void SceneControl::setShowAxes(bool showAxes) {
  ui->showAxesCheckBox->setChecked(showAxes);
  emit updateViews();
}

void SceneControl::setRotateFrames(bool rotateFrames) {
  ui->rotateFramesCheckBox->setChecked(rotateFrames);
}

void SceneControl::setFrameRotation(double yaw) {
  ui->frameRotationSpinBox->setValue(yaw*180.0/M_PI);
}

void SceneControl::setCameraFollow(bool follow) {
  ui->cameraFollowCheckBox->setChecked(follow);
  emit updateViews();
}

/*****************************************************************************/
/* Methods                                                                   */
/*****************************************************************************/

void SceneControl::renderBackground(View& view, const QColor& color) {
  view.clear(color);
}

void SceneControl::renderFog(View& view, const QColor& color, double start,
    double end, double density) {
  double scale = scene.getScale();
  double distance = camera.getViewpointDistance();

  view.enableFog(color, distance+start*scale, distance+end*scale, density);
}

void SceneControl::renderGround(View& view, const QColor& color, double radius,
    double elevation, double angleStep, double rangeStep) {
  for (double theta = -M_PI; theta < M_PI; theta += angleStep) {
    Line<double, 3> line;
    line[0] = Line<double, 3>::Point(rangeStep*sin(theta),
      rangeStep*cos(theta), elevation);
    line[1] = Line<double, 3>::Point(radius*sin(theta),
      radius*cos(theta), elevation);

    view.render(line, color);
  }
  
  for (double range = rangeStep; ; range += rangeStep) {
    if (range > radius)
      range = radius;

    double thetaStep = angleStep/range;
    Line<double, 3> loop(2.0*M_PI/thetaStep, true);
    for (int i = 0; i < loop.getNumPoints(); ++i)
      loop[i] = Line<double, 3>::Point(range*sin(i*thetaStep),
        range*cos(i*thetaStep), elevation);
    view.render(loop, color);

    if (range == radius)
      break;
  }
}

void SceneControl::renderAxes(View& view, const QColor& color, double length) {
  Line<double, 3> l_1, l_2, l_3;
  l_1[1][0] = length;
  view.render(l_1, color);
  l_2[1][1] = length;
  view.render(l_2, color);
  l_3[1][2] = length;
  view.render(l_3, color);

  view.render("X", length, 0.0, 0.0, color, 0.2*length);
  view.render("Y", 0.0, length, 0.0, color, 0.2*length);
  view.render("Z", 0.0, 0.0, length, color, 0.2*length);

  view.render("world", 0.0, 0.0, length + 0.5, color, 0.2 * length);
}

void SceneControl::groundRadiusChanged(double radius) {
  setGroundRadius(radius);
}

void SceneControl::groundElevationChanged(double elevation) {
  setGroundElevation(elevation);
}

void SceneControl::cameraPositionChanged() {
  camera.setPosition(
    ui->cameraXSpinBox->value(),
    ui->cameraYSpinBox->value(),
    ui->cameraZSpinBox->value());
}

void SceneControl::cameraViewpointChanged() {
  camera.setViewpoint(
    ui->cameraViewXSpinBox->value(),
    ui->cameraViewYSpinBox->value(),
    ui->cameraViewZSpinBox->value());
}

void SceneControl::cameraModeChanged(int mode) {
  camera.setMode((Camera::Mode)ui->cameraModeComboBox->currentIndex());
}

void SceneControl::cameraRangeChanged() {
  camera.setRange(
    ui->cameraNearSpinBox->value(),
    ui->cameraFarSpinBox->value());
}

void SceneControl::sceneTranslationChanged() {
  scene.setTranslation(
    ui->sceneXSpinBox->value(),
    ui->sceneYSpinBox->value(),
    ui->sceneZSpinBox->value());
}

void SceneControl::sceneRotationChanged() {
  scene.setRotation(
    ui->sceneYawSpinBox->value()*M_PI/180.0,
    ui->scenePitchSpinBox->value()*M_PI/180.0,
    ui->sceneRollSpinBox->value()*M_PI/180.0);
}

void SceneControl::sceneScaleChanged() {
  scene.setScale(ui->sceneScaleSpinBox->value());
}

void SceneControl::showFogToggled(bool checked) {
  setShowFog(checked);
}

void SceneControl::showGroundToggled(bool checked) {
  setShowGround(checked);
}

void SceneControl::showAxesToggled(bool checked) {
  setShowAxes(checked);
}

void SceneControl::colorChanged(const QString& role, const QColor& color) {
  emit updateViews();
}

void SceneControl::cameraPositionChanged(const Camera& camera) {
  ui->cameraXSpinBox->blockSignals(true);
  ui->cameraYSpinBox->blockSignals(true);
  ui->cameraZSpinBox->blockSignals(true);

  ui->cameraXSpinBox->setValue(camera.getPosition()[0]);
  ui->cameraYSpinBox->setValue(camera.getPosition()[1]);
  ui->cameraZSpinBox->setValue(camera.getPosition()[2]);

  ui->cameraXSpinBox->blockSignals(false);
  ui->cameraYSpinBox->blockSignals(false);
  ui->cameraZSpinBox->blockSignals(false);

  emit updateViews();
}

void SceneControl::cameraViewpointChanged(const Camera& camera) {
  ui->cameraViewXSpinBox->blockSignals(true);
  ui->cameraViewYSpinBox->blockSignals(true);
  ui->cameraViewZSpinBox->blockSignals(true);

  ui->cameraViewXSpinBox->setValue(camera.getViewpoint()[0]);
  ui->cameraViewYSpinBox->setValue(camera.getViewpoint()[1]);
  ui->cameraViewZSpinBox->setValue(camera.getViewpoint()[2]);

  ui->cameraViewXSpinBox->blockSignals(false);
  ui->cameraViewYSpinBox->blockSignals(false);
  ui->cameraViewZSpinBox->blockSignals(false);

  emit updateViews();
}

void SceneControl::cameraModeChanged(const Camera& camera) {
  ui->cameraModeComboBox->blockSignals(true);
  ui->cameraModeComboBox->setCurrentIndex(camera.getMode());
  ui->cameraModeComboBox->blockSignals(false);

  emit updateViews();
}

void SceneControl::cameraRangeChanged(const Camera& camera) {
  ui->cameraNearSpinBox->blockSignals(true);
  ui->cameraFarSpinBox->blockSignals(true);

  ui->cameraNearSpinBox->setValue(camera.getRange()[0]);
  ui->cameraFarSpinBox->setValue(camera.getRange()[1]);

  ui->cameraNearSpinBox->blockSignals(false);
  ui->cameraFarSpinBox->blockSignals(false);

  emit updateViews();
}

void SceneControl::sceneTranslationChanged(const Scene& scene) {
  ui->sceneXSpinBox->blockSignals(true);
  ui->sceneYSpinBox->blockSignals(true);
  ui->sceneZSpinBox->blockSignals(true);
  
  ui->sceneXSpinBox->setValue(scene.getTranslation()[0]);
  ui->sceneYSpinBox->setValue(scene.getTranslation()[1]);
  ui->sceneZSpinBox->setValue(scene.getTranslation()[2]);

  ui->sceneXSpinBox->blockSignals(false);
  ui->sceneYSpinBox->blockSignals(false);
  ui->sceneZSpinBox->blockSignals(false);

  emit updateViews();
}

void SceneControl::sceneRotationChanged(const Scene& scene) {
  ui->sceneYawSpinBox->blockSignals(true);
  ui->scenePitchSpinBox->blockSignals(true);
  ui->sceneRollSpinBox->blockSignals(true);

  ui->sceneYawSpinBox->setValue(scene.getRotation()[0]*180.0/M_PI);
  ui->scenePitchSpinBox->setValue(scene.getRotation()[1]*180.0/M_PI);
  ui->sceneRollSpinBox->setValue(scene.getRotation()[2]*180.0/M_PI);

  ui->sceneYawSpinBox->blockSignals(false);
  ui->scenePitchSpinBox->blockSignals(false);
  ui->sceneRollSpinBox->blockSignals(false);

  emit updateViews();
}

void SceneControl::sceneScaleChanged(const Scene& scene) {
  ui->sceneScaleSpinBox->blockSignals(true);
  ui->sceneScaleSpinBox->setValue(scene.getScale());
  ui->sceneScaleSpinBox->blockSignals(false);

  emit updateViews();
}

void SceneControl::transformView(View& view, const QPoint& fromMouse,
    const QPoint& toMouse, int wheel, Qt::MouseButtons mouseButtons) {
  if (mouseButtons == Qt::LeftButton) {
    View::Size size = view.getSize();
    Scene::Rotation rotation = scene.getRotation();
    
    rotation[0] -= M_PI/size[0]*(toMouse.x()-fromMouse.x());
    rotation[1] -= M_PI/size[1]*(toMouse.y()-fromMouse.y());

    scene.setRotation(rotation);
  }
  else if (mouseButtons == Qt::RightButton) {
    Scene::Translation translation = scene.getTranslation();
    View::Point viewpoint = camera.getViewpoint();
    camera.project(viewpoint, view.getAspectRatio());
    view.map(viewpoint);

    View::Point fromScene(fromMouse.x(), fromMouse.y(), viewpoint[2]);
    View::Point toScene(toMouse.x(), toMouse.y(), viewpoint[2]);

    view.unmap(fromScene);
    view.unmap(toScene);
    camera.unproject(fromScene, view.getAspectRatio());
    camera.unproject(toScene, view.getAspectRatio());
    scene.untransform(fromScene);
    scene.untransform(toScene);

    translation += toScene-fromScene;
    scene.setTranslation(translation);
  }
  else if (wheel) {
    double scaleFactor = 1e-2;
    scene.setScale(scene.getScale()*(1.0+scaleFactor*wheel/8.0));
  }
}

void SceneControl::prepareView(View& view) {
  camera.setup(view, view.getAspectRatio());
  scene.setup(view);
}

void SceneControl::renderView(View& view) {
  double radius = ui->groundXSpinBox->value();

  renderBackground(view, palette.getColor("Background"));
  if (ui->showFogCheckBox->isChecked())
    renderFog(view, palette.getColor("Fog"), radius, 2.0*radius, 1.0);
  if (ui->showGroundCheckBox->isChecked())
    renderGround(view, palette.getColor("Ground"), radius,
      ui->groundZSpinBox->value(), 30.0*M_PI/180.0, 5.0);
  if (ui->showAxesCheckBox->isChecked())
    renderAxes(view, palette.getColor("Axes"), 2.5);
}

void SceneControl::poseUpdate(double x, double y, double z, double yaw,
    double pitch, double roll) {
  if (ui->cameraFollowCheckBox->isChecked()) {
    scene.setTranslation(-x, -y, -z);
//    scene.setRotation();
  }
  if (ui->rotateFramesCheckBox->isChecked()) {
    scene.setRotation((ui->sceneYawSpinBox->value()+
      ui->frameRotationSpinBox->value())*M_PI/180.0,
      ui->scenePitchSpinBox->value()*M_PI/180.0,
      ui->sceneRollSpinBox->value()*M_PI/180.0);
  }
}
