/*
*  This file is part of openauto project.
*  Copyright (C) 2018 f1x.studio (Michal Szwaj)
*
*  openauto is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 3 of the License, or
*  (at your option) any later version.

*  openauto is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with openauto. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <memory>
#include <QMainWindow>
#include <QFile>
#include <f1x/openauto/autoapp/Configuration/IConfiguration.hpp>

namespace Ui
{
class MainWindow;
}

namespace f1x
{
namespace openauto
{
namespace autoapp
{
namespace ui
{

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(configuration::IConfiguration::Pointer configuration, QWidget *parent = nullptr);
    ~MainWindow() override;

signals:
    void exit();
    void reboot();
    void openSettings();
    void toggleCursor();
    void TriggerScriptDay();
    void TriggerScriptNight();
    void cameraShow();
    void cameraHide();
    void cameraStop();
    void cameraSave();
    void cameraRecord();
    void openConnectDialog();
    void showBrightnessSlider();
    void startKodi();
    void showRearCam();
    void hideRearCam();

private slots:
    void on_horizontalSliderBrightness_valueChanged(int value);

private slots:
    void on_pushButtonBrightness_clicked();
    void switchGuiToDay();
    void switchGuiToNight();
    void showTime();
    void cameraControlShow();
    void cameraControlHide();
    void toggleExit();
    void showRearCamBG();
    void hideRearCamBG();
    void saveVolumeOnExit();

private:
    Ui::MainWindow* ui_;
    bool brightnessSliderVisible = false;
    QString brightnessFilename = "/sys/class/backlight/rpi_backlight/brightness";
    QFile *brightnessFile;
    char brightness_str[5];

    bool wifiButtonForce = false;
    bool cameraButtonForce = false;
    bool kodiButtonForce = false;
    bool brightnessButtonForce = false;

    bool nightModeEnabled = false;
    bool DayNightModeState = false;

    bool devModeEnabled = false;

    bool wallpaperDayFileExists = false;
    bool wallpaperNightFileExists = false;
    bool wallpaperDevFileExists = false;
    bool wallpaperDevNightFileExists = false;

    bool masterButtonBGState = false;
    bool exitMenuVisible = false;

    bool rearCamEnabled = false;
    bool dashcamBGState = false;
    bool rearcamState = false;

    bool dashCamRecording = false;
};

}
}
}
}
