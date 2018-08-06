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

#include <QApplication>
#include <f1x/openauto/autoapp/UI/MainWindow.hpp>
#include <QFileInfo>
#include <QFile>
#include "ui_mainwindow.h"
#include <QTimer>
#include <QDateTime>
#include <QMessageBox>
#include <QTextStream>
#include <QFontDatabase>
#include <QFont>
#include <thread>
#include <chrono>
#include <string>

namespace f1x
{
namespace openauto
{
namespace autoapp
{
namespace ui
{

MainWindow::MainWindow(configuration::IConfiguration::Pointer configuration, QWidget *parent)
    : QMainWindow(parent)
    , ui_(new Ui::MainWindow)
{
    this->setStyleSheet("QMainWindow {background-color: rgb(0,0,0);} \
        QPushButton { background: url(:/circle.png); border: 0; } \
        QPushButton:hover { background: url(:/circle.png); } \
        QPushButton:focus { background: url(:/circle.png); } \
        QPushButton:pressed { background: url(:/circle-pressed.png); } \
        QSlider:horizontal { background: url(:/slider.png); border: 1px solid #ffffff; border-radius: 2px; min-height: 32px;} \
        QSlider::groove:horizontal { background: #6d6d6d; height: 32px;} \
        QSlider::handle:horizontal { background: white; height: 52px; width: 52px; margin: 0 0;} \
        QLabel { color: #ffffff; font-weight: bold;} \
    ");

    // Set default font and size
    int id = QFontDatabase::addApplicationFont(":/Roboto-Regular.ttf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    QFont _font(family, 11);
    qApp->setFont(_font);

    // inits by files
    QFileInfo wallpaperDayFile("wallpaper.png");
    this->wallpaperDayFileExists = wallpaperDayFile.exists();

    QFileInfo wallpaperNightFile("wallpaper-night.png");
    this->wallpaperNightFileExists = wallpaperNightFile.exists();

    QFileInfo wallpaperDevFile("wallpaper-devmode.png");
    this->wallpaperDevFileExists = wallpaperDevFile.exists();

    QFileInfo wallpaperDevNightFile("wallpaper-devmode-night.png");
    this->wallpaperDevNightFileExists = wallpaperDevNightFile.exists();

    QFileInfo nightModeFile("/tmp/night_mode_enabled");
    this->nightModeEnabled = nightModeFile.exists();

    QFileInfo devModeFile("/tmp/dev_mode_enabled");
    this->devModeEnabled = devModeFile.exists();

    if (wallpaperDayFile.isSymLink()) {
        QFileInfo linkTarget(wallpaperDayFile.symLinkTarget());
        this->wallpaperDayFileExists = linkTarget.exists();
    }

    if (wallpaperNightFile.isSymLink()) {
        QFileInfo linkTarget(wallpaperNightFile.symLinkTarget());
        this->wallpaperNightFileExists = linkTarget.exists();
    }

    if (wallpaperDevFile.isSymLink()) {
        QFileInfo linkTarget(wallpaperDevFile.symLinkTarget());
        this->wallpaperDevFileExists = linkTarget.exists();
    }

    if (wallpaperDevNightFile.isSymLink()) {
        QFileInfo linkTarget(wallpaperDevNightFile.symLinkTarget());
        this->wallpaperDevNightFileExists = linkTarget.exists();
    }

    ui_->setupUi(this);
    connect(ui_->pushButtonSettings, &QPushButton::clicked, this, &MainWindow::openSettings);
    connect(ui_->pushButtonSettings, &QPushButton::clicked, this, &MainWindow::cameraHide);
    connect(ui_->pushButtonSettings, &QPushButton::clicked, this, &MainWindow::cameraControlHide);
    connect(ui_->pushButtonExit, &QPushButton::clicked, this, &MainWindow::toggleExit);
    connect(ui_->pushButtonExit, &QPushButton::clicked, this, &MainWindow::cameraStop);
    connect(ui_->pushButtonExit, &QPushButton::clicked, this, &MainWindow::cameraHide);
    connect(ui_->pushButtonExit, &QPushButton::clicked, this, &MainWindow::cameraControlHide);
    connect(ui_->pushButtonShutdown, &QPushButton::clicked, this, &MainWindow::exit);
    connect(ui_->pushButtonReboot, &QPushButton::clicked, this, &MainWindow::reboot);
    connect(ui_->pushButtonCancel, &QPushButton::clicked, this, &MainWindow::toggleExit);
    connect(ui_->pushButtonToggleCursor, &QPushButton::clicked, this, &MainWindow::toggleCursor);
    connect(ui_->pushButtonDay, &QPushButton::clicked, this, &MainWindow::TriggerScriptDay);
    connect(ui_->pushButtonDay, &QPushButton::clicked, this, &MainWindow::switchGuiToDay);
    connect(ui_->pushButtonNight, &QPushButton::clicked, this, &MainWindow::TriggerScriptNight);
    connect(ui_->pushButtonNight, &QPushButton::clicked, this, &MainWindow::switchGuiToNight);
    connect(ui_->pushButtonWirelessConnection, &QPushButton::clicked, this, &MainWindow::openConnectDialog);
    connect(ui_->pushButtonKodi, &QPushButton::clicked, this, &MainWindow::startKodi);
    connect(ui_->pushButtonBrightness, &QPushButton::clicked, this, &MainWindow::showBrightnessSlider);
    connect(ui_->systemDebugging, &QPushButton::clicked, this, &MainWindow::createDebuglog);

    QTimer *timer=new QTimer(this);
    connect(timer, SIGNAL(timeout()),this,SLOT(showTime()));
    timer->start();

    // Get version string
    QFileInfo vFile("/etc/crankshaft.build");
    if (vFile.exists()) {
        QFile versionFile(QString("/etc/crankshaft.build"));
        versionFile.open(QIODevice::ReadOnly);
        QTextStream data_version(&versionFile);
        QString lineversion = data_version.readAll();
        versionFile.close();
        ui_->versionString->setText(lineversion);
    } else {
        ui_->versionString->setText("");
    }

    // Get date string
    QFileInfo dFile("/etc/crankshaft.build");
    if (dFile.exists()) {
        QFile dateFile(QString("/etc/crankshaft.date"));
        dateFile.open(QIODevice::ReadOnly);
        QTextStream data_date(&dateFile);
        QString linedate = data_date.readAll();
        dateFile.close();
        ui_->dateString->setText(linedate);
    } else {
        ui_->dateString->setText("");
    }

    QFileInfo cursorButtonFile("/etc/button_cursor_visible");
    bool cursorButtonForce = cursorButtonFile.exists();

    QFileInfo wifiButtonFile("/etc/button_wifi_visible");
    this->wifiButtonForce = wifiButtonFile.exists();

    QFileInfo cameraButtonFile("/etc/button_camera_visible");
    this->cameraButtonForce = cameraButtonFile.exists();

    if (this->cameraButtonForce) {
        connect(ui_->pushButtonCameraShow, &QPushButton::clicked, this, &MainWindow::cameraShow);
        connect(ui_->pushButtonCameraShow, &QPushButton::clicked, this, &MainWindow::cameraControlShow);
        connect(ui_->pushButtonCameraHide, &QPushButton::clicked, this, &MainWindow::cameraHide);
        connect(ui_->pushButtonCameraHide, &QPushButton::clicked, this, &MainWindow::cameraControlHide);
        connect(ui_->pushButtonStop, &QPushButton::clicked, this, &MainWindow::cameraStop);
        connect(ui_->pushButtonRecord, &QPushButton::clicked, this, &MainWindow::cameraRecord);
        connect(ui_->pushButtonRearcam, &QPushButton::clicked, this, &MainWindow::showRearCamBG);
        connect(ui_->pushButtonRearcam, &QPushButton::clicked, this, &MainWindow::showRearCam);
        connect(ui_->pushButtonRearcamBack, &QPushButton::clicked, this, &MainWindow::hideRearCamBG);
        connect(ui_->pushButtonRearcamBack, &QPushButton::clicked, this, &MainWindow::hideRearCam);
        connect(ui_->pushButtonRearcamBack, &QPushButton::clicked, this, &MainWindow::cameraControlHide);
        connect(ui_->pushButtonSave, &QPushButton::clicked, this, &MainWindow::cameraSave);
        ui_->pushButtonCameraShow->show();
        ui_->pushButtonCameraHide->hide();
    } else {
        ui_->pushButtonCameraShow->hide();
        ui_->pushButtonCameraHide->hide();

    }

    QFileInfo brightnessButtonFile("/etc/button_brightness_visible");
    this->brightnessButtonForce = brightnessButtonFile.exists();

    QFileInfo kodiInstalledFile("/usr/bin/kodi");
    this->kodiButtonForce = kodiInstalledFile.exists();

    if (configuration->hasTouchScreen() && !cursorButtonForce) {
        ui_->pushButtonToggleCursor->hide();
    }

    QPixmap image;
    image.load(":/bg_buttons.png");
    ui_->masterButtonBG->setPixmap(image);
    ui_->pushButtonStop->hide();
    ui_->pushButtonRecord->hide();
    ui_->pushButtonRecordActive->hide();
    ui_->pushButtonSave->hide();
    ui_->pushButtonRearcam->hide();
    ui_->pushButtonRearcamBack->hide();
    ui_->systemConfigInProgress->hide();
    ui_->systemDebugging->hide();
    ui_->pushButtonLock->hide();

    QFileInfo DebugmodeFile("/tmp/usb_debug_mode");
    if (DebugmodeFile.exists()) {
        ui_->systemDebugging->show();
        this->systemDebugmode = true;
    }

    if (!this->wifiButtonForce) {
        ui_->pushButtonWirelessConnection->hide();
    }

    QFileInfo brightnessFile(brightnessFilename);

    if (!brightnessFile.exists() && !this->brightnessButtonForce) {
        ui_->pushButtonBrightness->hide();
    }

    QFileInfo brightnessFileAlt(brightnessFilenameAlt);

    if (brightnessFileAlt.exists()) {
        ui_->pushButtonBrightness->show();
        this->customBrightnessControl = true;
    }

    ui_->kodiBG->hide();
    if (!this->kodiButtonForce) {
        ui_->pushButtonKodi->hide();
    } else {
        system("/usr/local/bin/autoapp_helper startkodimonitor &");
    }

    ui_->horizontalSliderBrightness->hide();

    if (!configuration->showClock()) {
        ui_->Digital_clock->hide();
    }

    ui_->pushButtonShutdown->hide();
    ui_->pushButtonReboot->hide();
    ui_->pushButtonCancel->hide();

    ui_->phoneConnected->hide();

    // init bg's on startup
    if (!this->nightModeEnabled) {
        if (this->devModeEnabled) {
            if (this->wallpaperDevFileExists) {
                this->setStyleSheet( this->styleSheet().append("QMainWindow { background: url(wallpaper-devmode.png) }") );
            } else {
                this->setStyleSheet( this->styleSheet().append("QMainWindow { background: url(:/connect.png) }") );
            }
        } else {
            if (this->wallpaperDayFileExists) {
                this->setStyleSheet( this->styleSheet().append("QMainWindow { background: url(wallpaper.png) }") );
            } else {
                this->setStyleSheet( this->styleSheet().append("QMainWindow { background: url(:/connect.png) }") );
            }
        }
    } else {
        if (this->devModeEnabled) {
            if (this->wallpaperDevNightFileExists) {
                this->setStyleSheet( this->styleSheet().append("QMainWindow { background: url(wallpaper-devmode-night.png) }") );
            } else {
                this->setStyleSheet( this->styleSheet().append("QMainWindow { background: url(:/connect.png) }") );
            }
        } else {
            if (this->wallpaperNightFileExists) {
                this->setStyleSheet( this->styleSheet().append("QMainWindow { background: url(wallpaper-night.png) }") );
            } else {
                this->setStyleSheet( this->styleSheet().append("QMainWindow { background: url(:/connect.png) }") );
            }
        }
    }
    system("/usr/local/bin/autoapp_helper restorevolumes");
}

MainWindow::~MainWindow()
{
    delete ui_;
}

}
}
}
}

void f1x::openauto::autoapp::ui::MainWindow::on_pushButtonBrightness_clicked()
{
    this->brightnessSliderVisible = !this->brightnessSliderVisible;
    if (this->brightnessSliderVisible) {
        if (this->cameraButtonForce) {
            f1x::openauto::autoapp::ui::MainWindow::cameraControlHide();
            f1x::openauto::autoapp::ui::MainWindow::cameraHide();
        }
        // Get the current brightness value
        this->brightnessFile = new QFile(this->brightnessFilename);
        if (this->brightnessFile->open(QIODevice::ReadOnly)) {
            QByteArray data = this->brightnessFile->readAll();
            std::string::size_type sz;
            int brightness_val = std::stoi(data.toStdString(), &sz);
            ui_->horizontalSliderBrightness->setValue(brightness_val);
            this->brightnessFile->close();
        }

        ui_->horizontalSliderBrightness->show();
    } else {
        ui_->horizontalSliderBrightness->hide();
    }
}

void f1x::openauto::autoapp::ui::MainWindow::on_horizontalSliderBrightness_valueChanged(int value)
{
    int n = snprintf(this->brightness_str, 4, "%d", value);

    this->brightnessFile = new QFile(this->brightnessFilename);
    this->brightnessFileAlt = new QFile(this->brightnessFilenameAlt);

    if (!this->customBrightnessControl) {
        if (this->brightnessFile->open(QIODevice::WriteOnly)) {
            this->brightness_str[n] = '\n';
            this->brightness_str[n+1] = '\0';
            this->brightnessFile->write(this->brightness_str);
            this->brightnessFile->close();
        }
    } else {
        if (this->brightnessFileAlt->open(QIODevice::WriteOnly)) {
            this->brightness_str[n] = '\n';
            this->brightness_str[n+1] = '\0';
            this->brightnessFileAlt->write(this->brightness_str);
            this->brightnessFileAlt->close();
            system("/usr/local/bin/autoapp_helper custombrightness &");
        }
    }
}

void f1x::openauto::autoapp::ui::MainWindow::switchGuiToNight()
{
    if (this->devModeEnabled) {
        if (this->wallpaperDevNightFileExists) {
            this->setStyleSheet( this->styleSheet().append("QMainWindow { background: url(wallpaper-devmode-night.png) }") );
        } else {
            this->setStyleSheet( this->styleSheet().append("QMainWindow { background: url(:/connect.png) }") );
        }
    } else {
        if (this->wallpaperNightFileExists) {
            this->setStyleSheet( this->styleSheet().append("QMainWindow { background: url(wallpaper-night.png) }") );
        } else {
            this->setStyleSheet( this->styleSheet().append("QMainWindow { background: url(:/connect.png) }") );
        }
    }
    ui_->pushButtonDay->show();
    ui_->pushButtonNight->hide();
    if (this->brightnessSliderVisible) {
        ui_->horizontalSliderBrightness->hide();
        this->brightnessSliderVisible = false;
    }
}

void f1x::openauto::autoapp::ui::MainWindow::switchGuiToDay()
{
    if (this->devModeEnabled) {
        if (this->wallpaperDevFileExists) {
            this->setStyleSheet( this->styleSheet().append("QMainWindow { background: url(wallpaper-devmode.png) }") );
        } else {
            this->setStyleSheet( this->styleSheet().append("QMainWindow { background: url(:/connect.png) }") );
        }
    } else {
        if (this->wallpaperDayFileExists) {
            this->setStyleSheet( this->styleSheet().append("QMainWindow { background: url(wallpaper.png) }") );
        } else {
            this->setStyleSheet( this->styleSheet().append("QMainWindow { background: url(:/connect.png) }") );
        }
    }
    ui_->pushButtonNight->show();
    ui_->pushButtonDay->hide();
    if (this->brightnessSliderVisible) {
        ui_->horizontalSliderBrightness->hide();
        this->brightnessSliderVisible = false;
    }
}

void f1x::openauto::autoapp::ui::MainWindow::cameraControlHide()
{
    if (this->cameraButtonForce) {
        ui_->pushButtonCameraHide->hide();
        ui_->pushButtonStop->hide();
        ui_->pushButtonRecord->hide();
        ui_->pushButtonRecordActive->hide();
        ui_->pushButtonRearcam->hide();
        ui_->pushButtonSave->hide();
        ui_->dashcamBG->hide();
        this->dashcamBGState = false;
        ui_->pushButtonCameraShow->show();
    }
}

void f1x::openauto::autoapp::ui::MainWindow::cameraControlShow()
{
    if (this->cameraButtonForce) {
        ui_->pushButtonCameraShow->hide();
        QPixmap image;
        image.load(":/bg_dashcam.png");
        ui_->dashcamBG->setPixmap(image);
        ui_->dashcamBG->show();
        ui_->pushButtonStop->show();
        ui_->pushButtonRecord->show();
        ui_->pushButtonRearcam->show();
        ui_->pushButtonSave->show();
        this->dashcamBGState = true;
        ui_->pushButtonCameraHide->show();
        if (this->brightnessSliderVisible) {
            ui_->horizontalSliderBrightness->hide();
            this->brightnessSliderVisible = false;
        }
    }
}

void f1x::openauto::autoapp::ui::MainWindow::toggleExit()
{
    if (!this->exitMenuVisible) {
        f1x::openauto::autoapp::ui::MainWindow::cameraControlHide();
        if (this->brightnessSliderVisible) {
            ui_->horizontalSliderBrightness->hide();
            this->brightnessSliderVisible = false;
        }
        ui_->pushButtonShutdown->show();
        ui_->pushButtonReboot->show();
        ui_->pushButtonCancel->show();
        QPixmap image;
        image.load(":/bg_exit.png");
        ui_->exitBG->setPixmap(image);
        ui_->exitBG->show();
        this->exitMenuVisible = true;
    } else {
        ui_->pushButtonShutdown->hide();
        ui_->pushButtonReboot->hide();
        ui_->pushButtonCancel->hide();
        ui_->exitBG->hide();
        this->exitMenuVisible = false;
    }
}

void f1x::openauto::autoapp::ui::MainWindow::showRearCamBG()
{
    ui_->pushButtonRearcamBack->show();
    if (this->brightnessSliderVisible) {
        ui_->horizontalSliderBrightness->hide();
        this->brightnessSliderVisible = false;
    }
}

void f1x::openauto::autoapp::ui::MainWindow::hideRearCamBG()
{
    ui_->pushButtonRearcamBack->hide();
}

void f1x::openauto::autoapp::ui::MainWindow::createDebuglog()
{
    system("/usr/local/bin/crankshaft debuglog &");
}

void f1x::openauto::autoapp::ui::MainWindow::showTime()
{
    using namespace std::this_thread; // sleep_for
    using namespace std::chrono; // milliseconds

    QTime time=QTime::currentTime();
    QString time_text=time.toString("hh : mm : ss");
    if ((time.second() % 2) == 0) {
        time_text[3] = ' ';
        time_text[8] = ' ';

        QFileInfo phoneConnectedFile("/tmp/android_device");
        if (phoneConnectedFile.exists()) {
            if (ui_->phoneConnected->isVisible() == false) {
                ui_->phoneConnected->setText("Phone connected");
                ui_->phoneConnected->show();
            }
        } else {
            if (ui_->phoneConnected->isVisible() == true) {
                ui_->phoneConnected->hide();
            }
        }

        QFileInfo configInProgressFile("/tmp/config_in_progress");
        QFileInfo debugInProgressFile("/tmp/debug_in_progress");

        if (configInProgressFile.exists() || debugInProgressFile.exists()) {
            if (ui_->systemConfigInProgress->isVisible() == false) {
                if (configInProgressFile.exists()) {
                    ui_->systemConfigInProgress->setText("System config in progress - please wait ...");
                    ui_->pushButtonSettings->hide();
                    ui_->pushButtonLock->show();
                    ui_->systemConfigInProgress->show();
                }
                if (debugInProgressFile.exists()) {
                    ui_->systemConfigInProgress->setText("Creating debug.zip on /boot - please wait ...");
                    ui_->systemDebugging->hide();
                    ui_->systemConfigInProgress->show();
                }
            }
        } else {
            if (ui_->systemConfigInProgress->isVisible() == true) {
                ui_->systemConfigInProgress->hide();
                ui_->pushButtonSettings->show();
                    ui_->pushButtonLock->hide();
                if (this->systemDebugmode) {
                    ui_->systemDebugging->show();
                }
            }
        }

        QFileInfo nightModeFile("/tmp/night_mode_enabled");
        this->nightModeEnabled = nightModeFile.exists();

        if (this->cameraButtonForce) {
            QFileInfo rearCamFile("/tmp/rearcam_enabled");
            this->rearCamEnabled = rearCamFile.exists();

            QFileInfo dashCamRecordingFile("/tmp/dashcam_is_recording");
            this->dashCamRecording = dashCamRecordingFile.exists();

            if (this->dashcamBGState) {
                if (this->dashCamRecording) {
                    if (ui_->pushButtonRecord->isVisible() == true) {
                        ui_->pushButtonRecordActive->show();
                        ui_->pushButtonRecord->hide();
                    }
                } else {
                    if (ui_->pushButtonRecordActive->isVisible() == true) {
                        ui_->pushButtonRecord->show();
                        ui_->pushButtonRecordActive->hide();
                    }
                }
            }
        }

        if (this->nightModeEnabled) {
            if (!this->DayNightModeState) {
                this->DayNightModeState = true;
                f1x::openauto::autoapp::ui::MainWindow::switchGuiToNight();
            }
        } else {
            if (this->DayNightModeState) {
                this->DayNightModeState = false;
                f1x::openauto::autoapp::ui::MainWindow::switchGuiToDay();
            }
        }

        if (this->cameraButtonForce) {
            if (this->rearCamEnabled) {
                if (!this->rearcamState) {
                    this->rearcamState = true;
                    f1x::openauto::autoapp::ui::MainWindow::cameraControlHide();
                    f1x::openauto::autoapp::ui::MainWindow::showRearCamBG();
                    f1x::openauto::autoapp::ui::MainWindow::showRearCam();
                }
            } else {
                if (this->rearcamState) {
                    this->rearcamState = false;
                    f1x::openauto::autoapp::ui::MainWindow::hideRearCamBG();
                    f1x::openauto::autoapp::ui::MainWindow::hideRearCam();
                }
            }
        }
        QFileInfo externalExitFile("/tmp/external_exit");
        if (externalExitFile.exists()) {
            f1x::openauto::autoapp::ui::MainWindow::MainWindow::exit();
        }

        if (this->kodiButtonForce) {
            QFileInfo kodiRunningFile("/tmp/kodi_running");
            if (kodiRunningFile.exists()) {
                if (ui_->kodiBG->isVisible() == false) {
                    ui_->kodiBG->hide();
                    QPixmap image;
                    image.load(":/black.png");
                    ui_->kodiBG->setPixmap(image);
                    ui_->kodiBG->show();
                }
            } else {
                if (ui_->kodiBG->isVisible() == true) {
                    ui_->kodiBG->hide();
                }
            }
        }
    }
    ui_->Digital_clock->setText(time_text);
    sleep_for(milliseconds(10));
}
