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
    ");
    QFileInfo wallpaperDayFile("wallpaper.png");
    bool wallpaperDayFileExists = wallpaperDayFile.exists();

    QFileInfo wallpaperNightFile("wallpaper-night.png");
    bool wallpaperNightFileExists = wallpaperNightFile.exists();

    QFileInfo nightSwitchFile("/tmp/night_mode_enabled");
    bool nightSwitchExists = nightSwitchFile.exists();

    if (wallpaperDayFile.isSymLink()) {
        QFileInfo linkTarget(wallpaperDayFile.symLinkTarget());
        wallpaperDayFileExists = linkTarget.exists();
    }

    if (wallpaperNightFile.isSymLink()) {
        QFileInfo linkTarget(wallpaperNightFile.symLinkTarget());
        wallpaperNightFileExists = linkTarget.exists();
    }

    ui_->setupUi(this);
    connect(ui_->pushButtonSettings, &QPushButton::clicked, this, &MainWindow::openSettings);
    connect(ui_->pushButtonExit, &QPushButton::clicked, this, &MainWindow::exit);
    connect(ui_->pushButtonToggleCamera, &QPushButton::clicked, this, &MainWindow::toggleCamera);
    connect(ui_->pushButtonToggleCursor, &QPushButton::clicked, this, &MainWindow::toggleCursor);
    connect(ui_->pushButtonDay, &QPushButton::clicked, this, &MainWindow::TriggerScriptDay);
    connect(ui_->pushButtonDay, &QPushButton::clicked, this, &MainWindow::switchGuiToDay);
    connect(ui_->pushButtonNight, &QPushButton::clicked, this, &MainWindow::TriggerScriptNight);
    connect(ui_->pushButtonNight, &QPushButton::clicked, this, &MainWindow::switchGuiToNight);
    connect(ui_->pushButtonWirelessConnection, &QPushButton::clicked, this, &MainWindow::openConnectDialog);
    connect(ui_->pushButtonBrightness, &QPushButton::clicked, this, &MainWindow::showBrightnessSlider);

    if (!nightSwitchExists) {
	ui_->pushButtonNight->show();
	ui_->pushButtonDay->hide();
	if (wallpaperDayFileExists) {
	    this->setStyleSheet( this->styleSheet().append("QMainWindow { background: url(wallpaper.png) }") );
	} else {
    	    this->setStyleSheet( this->styleSheet().append("QMainWindow { background: url(:/connect.png) }") );
	}
    } else {
	ui_->pushButtonDay->show();
	ui_->pushButtonNight->hide();
	if (wallpaperNightFileExists) {
	    this->setStyleSheet( this->styleSheet().append("QMainWindow { background: url(wallpaper-night.png) }") );
	} else {
    	    this->setStyleSheet( this->styleSheet().append("QMainWindow { background: url(:/connect.png) }") );
	}
    }
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

    if (!cameraButtonForce) {
        ui_->pushButtonToggleCamera->hide();
    }

    if (!wifiButtonForce) {
        ui_->pushButtonWirelessConnection->hide();
    }

    QFileInfo brightnessFile(brightnessFilename);

    if (!brightnessFile.exists() && !brightnessButtonForce) {
        ui_->pushButtonBrightness->hide();
    }

    ui_->horizontalSliderBrightness->hide();
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
    this->setStyleSheet( this->styleSheet().append("QMainWindow { background: url(wallpaper-night.png) }") );
    ui_->pushButtonDay->show();
    ui_->pushButtonNight->hide();
}

void f1x::openauto::autoapp::ui::MainWindow::switchGuiToDay()
{
    this->setStyleSheet( this->styleSheet().append("QMainWindow { background: url(wallpaper.png) }") );
    ui_->pushButtonNight->show();
    ui_->pushButtonDay->hide();
}
