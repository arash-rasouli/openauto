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
    connect(ui_->pushButtonExit, &QPushButton::clicked, this, &MainWindow::exit);
    connect(ui_->pushButtonCameraShow, &QPushButton::clicked, this, &MainWindow::cameraShow);
    connect(ui_->pushButtonCameraShow, &QPushButton::clicked, this, &MainWindow::cameraControlShow);
    connect(ui_->pushButtonCameraHide, &QPushButton::clicked, this, &MainWindow::cameraHide);
    connect(ui_->pushButtonCameraHide, &QPushButton::clicked, this, &MainWindow::cameraControlHide);
    connect(ui_->pushButtonStop, &QPushButton::clicked, this, &MainWindow::cameraStop);
    connect(ui_->pushButtonRecord, &QPushButton::clicked, this, &MainWindow::cameraRecord);
    connect(ui_->pushButtonSave, &QPushButton::clicked, this, &MainWindow::cameraSave);
    connect(ui_->pushButtonToggleCursor, &QPushButton::clicked, this, &MainWindow::toggleCursor);
    connect(ui_->pushButtonDay, &QPushButton::clicked, this, &MainWindow::TriggerScriptDay);
    connect(ui_->pushButtonDay, &QPushButton::clicked, this, &MainWindow::switchGuiToDay);
    connect(ui_->pushButtonNight, &QPushButton::clicked, this, &MainWindow::TriggerScriptNight);
    connect(ui_->pushButtonNight, &QPushButton::clicked, this, &MainWindow::switchGuiToNight);
    connect(ui_->pushButtonWirelessConnection, &QPushButton::clicked, this, &MainWindow::openConnectDialog);
    connect(ui_->pushButtonBrightness, &QPushButton::clicked, this, &MainWindow::showBrightnessSlider);

    QTimer *timer=new QTimer(this);
    connect(timer, SIGNAL(timeout()),this,SLOT(showTime()));
    timer->start();

    QFileInfo cursorButtonFile("/etc/button_cursor_visible");
    bool cursorButtonForce = cursorButtonFile.exists();

    QFileInfo wifiButtonFile("/etc/button_wifi_visible");
    bool wifiButtonForce = wifiButtonFile.exists();

    QFileInfo cameraButtonFile("/etc/button_camera_visible");
    bool cameraButtonForce = cameraButtonFile.exists();

    QFileInfo brightnessButtonFile("/etc/button_brightness_visible");
    bool brightnessButtonForce = brightnessButtonFile.exists();

    if (configuration->hasTouchScreen() && !cursorButtonForce) {
        ui_->pushButtonToggleCursor->hide();
    }

    QPixmap image;
    image.load(":/bg_buttons.png");
    ui_->masterButtonBG->setPixmap(image);
    ui_->pushButtonStop->hide();
    ui_->pushButtonRecord->hide();
    ui_->pushButtonSave->hide();

    if (!cameraButtonForce) {
        ui_->pushButtonCameraShow->hide();
        ui_->pushButtonCameraHide->hide();
    } else {
        ui_->pushButtonCameraShow->show();
        ui_->pushButtonCameraHide->hide();
    }

    if (!wifiButtonForce) {
        ui_->pushButtonWirelessConnection->hide();
    }

    QFileInfo brightnessFile(brightnessFilename);

    if (!brightnessFile.exists() && !brightnessButtonForce) {
        ui_->pushButtonBrightness->hide();
    }

    ui_->horizontalSliderBrightness->hide();

    if (!configuration->showClock()) {
        ui_->Digital_clock->hide();
    }

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
	f1x::openauto::autoapp::ui::MainWindow::cameraControlHide();
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

    if (this->brightnessFile->open(QIODevice::WriteOnly)) {
        this->brightness_str[n] = '\n';
        this->brightness_str[n+1] = '\0';
        this->brightnessFile->write(this->brightness_str);
        this->brightnessFile->close();
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
    ui_->pushButtonCameraHide->hide();
    ui_->pushButtonStop->hide();
    ui_->pushButtonRecord->hide();
    ui_->pushButtonSave->hide();
    ui_->dashcamBG->hide();
    this->dashcamBGState = false;
    ui_->pushButtonCameraShow->show();
}

void f1x::openauto::autoapp::ui::MainWindow::cameraControlShow()
{
    ui_->pushButtonCameraShow->hide();
    QPixmap image;
    image.load(":/bg_dashcam.png");
    ui_->dashcamBG->setPixmap(image);
    ui_->dashcamBG->show();
    ui_->pushButtonStop->show();
    ui_->pushButtonRecord->show();
    ui_->pushButtonSave->show();
    this->dashcamBGState = true;
    ui_->pushButtonCameraHide->show();
}

void f1x::openauto::autoapp::ui::MainWindow::showTime()
{
    QTime time=QTime::currentTime();
    QString time_text=time.toString("hh : mm : ss");
    ui_->Digital_clock->setText(time_text);

    /**if (configuration_->showClock()) {
	if (ui_->Digital_clock->isVisible() == true) {
	    ui_->Digital_clock->hide();
	}
    } else {
	if (ui_->Digital_clock->isVisible() == false) {
	    ui_->Digital_clock->show();
	}
    }**/

    QFileInfo nightModeFile("/tmp/night_mode_enabled");
    this->nightModeEnabled = nightModeFile.exists();

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
}
