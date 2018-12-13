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
#include <QScreen>
#include <QRect>
#include <QFileSystemWatcher>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <unistd.h>
#include <f1x/openauto/Common/Log.hpp>

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
    // set default bg color to black
    this->setStyleSheet("QMainWindow {background-color: rgb(0,0,0);}");

    // Set default font and size
    int id = QFontDatabase::addApplicationFont(":/Roboto-Regular.ttf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    QFont _font(family, 11);
    qApp->setFont(_font);

    this->configuration_ = configuration;

    // trigger files
    this->nightModeEnabled = check_file_exist(this->nightModeFile);
    this->devModeEnabled = check_file_exist(this->devModeFile);
    this->wifiButtonForce = check_file_exist(this->wifiButtonFile);
    this->cameraButtonForce = check_file_exist(this->cameraButtonFile);
    this->brightnessButtonForce = check_file_exist(this->brightnessButtonFile);
    this->systemDebugmode = check_file_exist(this->debugModeFile);
    this->lightsensor = check_file_exist(this->lsFile);
    this->c1ButtonForce = check_file_exist(this->custom_button_file_c1);
    this->c2ButtonForce = check_file_exist(this->custom_button_file_c2);
    this->c3ButtonForce = check_file_exist(this->custom_button_file_c3);
    this->c4ButtonForce = check_file_exist(this->custom_button_file_c4);
    this->c5ButtonForce = check_file_exist(this->custom_button_file_c5);
    this->c6ButtonForce = check_file_exist(this->custom_button_file_c6);

    // wallpaper stuff
    this->wallpaperDayFileExists = check_file_exist("wallpaper.png");
    this->wallpaperNightFileExists = check_file_exist("wallpaper-night.png");
    this->wallpaperClassicDayFileExists = check_file_exist("wallpaper-classic.png");
    this->wallpaperClassicNightFileExists = check_file_exist("wallpaper-classic-night.png");
    this->wallpaperEQFileExists = check_file_exist("wallpaper-eq.png");

    ui_->setupUi(this);
    connect(ui_->pushButtonSettings, &QPushButton::clicked, this, &MainWindow::openSettings);
    connect(ui_->pushButtonSettings2, &QPushButton::clicked, this, &MainWindow::openSettings);
    connect(ui_->pushButtonWifiSetup, &QPushButton::clicked, this, &MainWindow::openWifiDialog);
    connect(ui_->pushButtonExit, &QPushButton::clicked, this, &MainWindow::toggleExit);
    connect(ui_->pushButtonExit2, &QPushButton::clicked, this, &MainWindow::toggleExit);
    connect(ui_->pushButtonShutdown, &QPushButton::clicked, this, &MainWindow::exit);
    connect(ui_->pushButtonShutdown2, &QPushButton::clicked, this, &MainWindow::exit);
    connect(ui_->pushButtonReboot, &QPushButton::clicked, this, &MainWindow::reboot);
    connect(ui_->pushButtonReboot2, &QPushButton::clicked, this, &MainWindow::reboot);
    connect(ui_->pushButtonCancel, &QPushButton::clicked, this, &MainWindow::toggleExit);
    connect(ui_->pushButtonCancel2, &QPushButton::clicked, this, &MainWindow::toggleExit);
    connect(ui_->pushButtonDay, &QPushButton::clicked, this, &MainWindow::TriggerScriptDay);
    connect(ui_->pushButtonDay, &QPushButton::clicked, this, &MainWindow::switchGuiToDay);
    connect(ui_->pushButtonDay2, &QPushButton::clicked, this, &MainWindow::TriggerScriptDay);
    connect(ui_->pushButtonDay2, &QPushButton::clicked, this, &MainWindow::switchGuiToDay);
    connect(ui_->pushButtonNight, &QPushButton::clicked, this, &MainWindow::TriggerScriptNight);
    connect(ui_->pushButtonNight, &QPushButton::clicked, this, &MainWindow::switchGuiToNight);
    connect(ui_->pushButtonNight2, &QPushButton::clicked, this, &MainWindow::TriggerScriptNight);
    connect(ui_->pushButtonNight2, &QPushButton::clicked, this, &MainWindow::switchGuiToNight);
    connect(ui_->pushButtonBrightness, &QPushButton::clicked, this, &MainWindow::showBrightnessSlider);
    connect(ui_->pushButtonBrightness2, &QPushButton::clicked, this, &MainWindow::showBrightnessSlider);
    connect(ui_->pushButtonVolume, &QPushButton::clicked, this, &MainWindow::showVolumeSlider);
    connect(ui_->pushButtonVolume2, &QPushButton::clicked, this, &MainWindow::showVolumeSlider);
    connect(ui_->pushButtonDebug, &QPushButton::clicked, this, &MainWindow::createDebuglog);
    connect(ui_->pushButtonDebug2, &QPushButton::clicked, this, &MainWindow::createDebuglog);
    connect(ui_->pushButtonBluetooth, &QPushButton::clicked, this, &MainWindow::setPairable);
    connect(ui_->pushButtonMute, &QPushButton::clicked, this, &MainWindow::toggleMuteButton);
    connect(ui_->pushButtonMute, &QPushButton::clicked, this, &MainWindow::setMute);
    connect(ui_->pushButtonUnMute, &QPushButton::clicked, this, &MainWindow::toggleMuteButton);
    connect(ui_->pushButtonUnMute, &QPushButton::clicked, this, &MainWindow::setUnMute);
    connect(ui_->pushButtonToggleGUI, &QPushButton::clicked, this, &MainWindow::toggleGUI);
    connect(ui_->pushButtonToggleGUI2, &QPushButton::clicked, this, &MainWindow::toggleGUI);
    connect(ui_->pushButtonWifi, &QPushButton::clicked, this, &MainWindow::openConnectDialog);
    connect(ui_->pushButtonWifi2, &QPushButton::clicked, this, &MainWindow::openConnectDialog);
    connect(ui_->pushButtonMusic, &QPushButton::clicked, this, &MainWindow::playerShow);
    connect(ui_->pushButtonMusic2, &QPushButton::clicked, this, &MainWindow::playerShow);
    connect(ui_->pushButtonBack, &QPushButton::clicked, this, &MainWindow::playerHide);
    connect(ui_->pushButtonPlayerBack, &QPushButton::clicked, this, &MainWindow::playerHide);
    connect(ui_->pushButtonAndroidAuto, &QPushButton::clicked, this, &MainWindow::TriggerAppStart);
    connect(ui_->pushButtonAndroidAuto, &QPushButton::clicked, this, &MainWindow::setRetryUSBConnect);
    connect(ui_->pushButtonAndroidAuto2, &QPushButton::clicked, this, &MainWindow::TriggerAppStart);
    connect(ui_->pushButtonAndroidAuto2, &QPushButton::clicked, this, &MainWindow::setRetryUSBConnect);

    // by default hide bluetooth button on init
    ui_->pushButtonBluetooth->hide();

    // by default hide media player
    ui_->mediaWidget->hide();
    ui_->pushButtonUSB->hide();

    ui_->SysinfoTopLeft->hide();
    ui_->pushButtonWifiSetup->hide();

    ui_->pushButtonAndroidAuto->hide();
    ui_->pushButtonAndroidAuto2->hide();

    ui_->SysinfoTopLeft2->hide();

    QTimer *timer=new QTimer(this);
    connect(timer, SIGNAL(timeout()),this,SLOT(showTime()));
    timer->start(1000);

    // enable connects while cam is enabled
    if (this->cameraButtonForce) {
        connect(ui_->pushButtonCameraShow, &QPushButton::clicked, this, &MainWindow::cameraShow);
        connect(ui_->pushButtonCameraShow, &QPushButton::clicked, this, &MainWindow::cameraControlShow);
        connect(ui_->pushButtonCameraShow2, &QPushButton::clicked, this, &MainWindow::cameraShow);
        connect(ui_->pushButtonCameraShow2, &QPushButton::clicked, this, &MainWindow::cameraControlShow);
        connect(ui_->pushButtonCameraHide, &QPushButton::clicked, this, &MainWindow::cameraHide);
        connect(ui_->pushButtonCameraHide, &QPushButton::clicked, this, &MainWindow::cameraControlHide);
        connect(ui_->pushButtonStop, &QPushButton::clicked, this, &MainWindow::cameraStop);
        connect(ui_->pushButtonRecord, &QPushButton::clicked, this, &MainWindow::cameraRecord);
        connect(ui_->pushButtonSave, &QPushButton::clicked, this, &MainWindow::cameraSave);
    } else {
        ui_->pushButtonCameraShow->hide();
        ui_->pushButtonCameraShow2->hide();
    }

    // show debug button if enabled
    if (!this->systemDebugmode) {
        ui_->pushButtonDebug->hide();
        ui_->pushButtonDebug2->hide();
    }

    ui_->pushButtonLock->hide();
    ui_->pushButtonLock2->hide();
    ui_->btDevice->hide();

    // hide brightness slider of control file is not existing
    QFileInfo brightnessFile(brightnessFilename);
    if (!brightnessFile.exists() && !this->brightnessButtonForce) {
        ui_->pushButtonBrightness->hide();
        ui_->pushButtonBrightness2->hide();
    }

    // as default hide brightness slider
    ui_->BrightnessSliderControl->hide();

    // as default hide volume slider player
    ui_->VolumeSliderControlPlayer->hide();

    // as default hide power buttons
    ui_->exitWidget->hide();
    ui_->horizontalWidgetPower->hide();

    // as default hide muted button
    ui_->pushButtonUnMute->hide();

    // hide wifi if not forced
    if (!this->wifiButtonForce) {
        ui_->AAWIFIWidget->hide();
        ui_->AAWIFIWidget2->hide();
    } else {
        ui_->AAUSBWidget->hide();
        ui_->AAUSBWidget2->hide();
    }

    // set custom buttons if file enabled by trigger file
    if (!this->c1ButtonForce) {
        ui_->pushButton_c1->hide();
    } else {
        // read button config 1
        QFile paramFile(this->custom_button_file_c1);
        paramFile.open(QIODevice::ReadOnly);
        QTextStream data(&paramFile);
        QStringList params = data.readAll().split("#");
        paramFile.close();
        ui_->pushButton_c1->setText(params[0].simplified());
        this->custom_button_command_c1 = params[1].simplified();
        if (params[2] != "") {
            this->custom_button_color_c1 = params[2].simplified();
        }
        connect(ui_->pushButton_c1, &QPushButton::clicked, this, &MainWindow::customButtonPressed1);
    }

    if (!this->c2ButtonForce) {
        ui_->pushButton_c2->hide();
    } else {
        // read button config 2
        QFile paramFile(this->custom_button_file_c2);
        paramFile.open(QIODevice::ReadOnly);
        QTextStream data(&paramFile);
        QStringList params = data.readAll().split("#");
        paramFile.close();
        ui_->pushButton_c2->setText(params[0].simplified());
        this->custom_button_command_c2 = params[1].simplified();
        if (params[2] != "") {
            this->custom_button_color_c2 = params[2].simplified();
        }
        connect(ui_->pushButton_c2, &QPushButton::clicked, this, &MainWindow::customButtonPressed2);
    }

    if (!this->c3ButtonForce) {
        ui_->pushButton_c3->hide();
    } else {
        // read button config 3
        QFile paramFile(this->custom_button_file_c3);
        paramFile.open(QIODevice::ReadOnly);
        QTextStream data(&paramFile);
        QStringList params = data.readAll().split("#");
        paramFile.close();
        ui_->pushButton_c3->setText(params[0].simplified());
        this->custom_button_command_c3 = params[1].simplified();
        if (params[2] != "") {
            this->custom_button_color_c3 = params[2].simplified();
        }
        connect(ui_->pushButton_c3, &QPushButton::clicked, this, &MainWindow::customButtonPressed3);
    }

    if (!this->c4ButtonForce) {
        ui_->pushButton_c4->hide();
    } else {
        // read button config 4
        QFile paramFile(this->custom_button_file_c4);
        paramFile.open(QIODevice::ReadOnly);
        QTextStream data(&paramFile);
        QStringList params = data.readAll().split("#");
        paramFile.close();
        ui_->pushButton_c4->setText(params[0].simplified());
        this->custom_button_command_c4 = params[1].simplified();
        if (params[2] != "") {
            this->custom_button_color_c4 = params[2].simplified();
        }
        connect(ui_->pushButton_c4, &QPushButton::clicked, this, &MainWindow::customButtonPressed4);
    }

    if (!this->c5ButtonForce) {
        ui_->pushButton_c5->hide();
    } else {
        // read button config 5
        QFile paramFile(this->custom_button_file_c5);
        paramFile.open(QIODevice::ReadOnly);
        QTextStream data(&paramFile);
        QStringList params = data.readAll().split("#");
        paramFile.close();
        ui_->pushButton_c5->setText(params[0].simplified());
        this->custom_button_command_c5 = params[1].simplified();
        if (params[2] != "") {
            this->custom_button_color_c5 = params[2].simplified();
        }
        connect(ui_->pushButton_c5, &QPushButton::clicked, this, &MainWindow::customButtonPressed5);
    }

    if (!this->c6ButtonForce) {
        ui_->pushButton_c6->hide();
    } else {
        // read button config 6
        QFile paramFile(this->custom_button_file_c6);
        paramFile.open(QIODevice::ReadOnly);
        QTextStream data(&paramFile);
        QStringList params = data.readAll().split("#");
        paramFile.close();
        ui_->pushButton_c6->setText(params[0].simplified());
        this->custom_button_command_c6 = params[1].simplified();
        if (params[2] != "") {
            this->custom_button_color_c6 = params[2].simplified();
        }
        connect(ui_->pushButton_c6, &QPushButton::clicked, this, &MainWindow::customButtonPressed6);
    }

    // as default hide camera controls
    ui_->cameraWidget->hide();
    ui_->pushButtonRecordActive->hide();

    // fill gui with dummys if needed
    if (this->c1ButtonForce || this->c2ButtonForce || this->c3ButtonForce || this->c4ButtonForce || this->c5ButtonForce || this->c6ButtonForce) {
        if (!this->c1ButtonForce && !this->c2ButtonForce) {
            ui_->pushButtonDummy1->show();
        } else {
            ui_->pushButtonDummy1->hide();
        }
        if (!this->c3ButtonForce && !this->c4ButtonForce) {
            ui_->pushButtonDummy2->show();
        } else {
            ui_->pushButtonDummy2->hide();
        }
        if (!this->c5ButtonForce && !this->c6ButtonForce) {
            ui_->pushButtonDummy3->show();
        } else {
            ui_->pushButtonDummy3->hide();
        }
    } else {
        ui_->pushButtonDummy1->hide();
        ui_->pushButtonDummy2->hide();
        ui_->pushButtonDummy3->hide();
    }

    // show dev labels if dev mode activated
    if (!this->devModeEnabled) {
        ui_->devlabel_left->hide();
        ui_->devlabel_right->hide();
    }

    // Load configured brightness values  
    system("/usr/local/bin/autoapp_helper getbrightnessvalues");
    QFile paramFile(QString("/tmp/br_values"));
    paramFile.open(QIODevice::ReadOnly);
    QTextStream data_param(&paramFile);
    QStringList brigthnessvalues = data_param.readAll().split("#");
    paramFile.close();

    // set brightness slider attribs
    ui_->horizontalSliderBrightness->setMinimum(brigthnessvalues[0].toInt());
    ui_->horizontalSliderBrightness->setMaximum(brigthnessvalues[1].toInt());
    ui_->horizontalSliderBrightness->setSingleStep(brigthnessvalues[2].toInt());
    ui_->horizontalSliderBrightness->setTickInterval(brigthnessvalues[2].toInt());

    // run monitor for custom brightness command if enabled in crankshaft_env.sh
    if (brigthnessvalues[3] == "1") {
        ui_->pushButtonBrightness->show();
        ui_->pushButtonBrightness2->show();
        this->customBrightnessControl = true;
        system("/usr/local/bin/autoapp_helper startcustombrightness &");
    }

    // read param file
    if (std::ifstream("/boot/crankshaft/volume")) {
        QFile audioparamFile(QString("/boot/crankshaft/volume"));
        audioparamFile.open(QIODevice::ReadOnly);
        QTextStream data_param(&audioparamFile);
        QString getparams = data_param.readAll();
        audioparamFile.close();
        // init volume
        QString vol=QString::number(getparams.toInt());
        ui_->volumeValueLabel->setText(vol+"%");
        ui_->horizontalSliderVolume->setValue(vol.toInt());
    }

    // switch to old menu if set in settings
    if (!configuration->oldGUI()) {
        this->oldGUIStyle = false;
        ui_->menuWidget->show();
        ui_->oldmenuWidget->hide();
    } else {
        this->oldGUIStyle = true;
        ui_->oldmenuWidget->show();
        ui_->menuWidget->hide();
    }

    // set bg's on startup
    MainWindow::updateBG();
    if (!this->nightModeEnabled) {
        ui_->pushButtonDay->hide();
        ui_->pushButtonDay2->hide();
        ui_->pushButtonNight->show();
        ui_->pushButtonNight2->show();
    } else {
        ui_->pushButtonNight->hide();
        ui_->pushButtonNight2->hide();
        ui_->pushButtonDay->show();
        ui_->pushButtonDay2->show();
    }

    // use big clock in classic gui?
    if (configuration->showBigClock()) {
        this->UseBigClock = true;
    } else {
        this->UseBigClock = false;
    }

    // clock viibility by settings
    if (!configuration->showClock()) {
        ui_->Digital_clock->hide();
        ui_->bigClock->hide();
        this->NoClock = true;
    } else {
        this->NoClock = false;
        if (this->UseBigClock) {
            ui_->oldmenuDummy->hide();
            ui_->bigClock->show();
            if (oldGUIStyle) {
                ui_->Digital_clock->hide();
            }
        } else {
            ui_->oldmenuDummy->show();
            ui_->Digital_clock->show();
            ui_->bigClock->hide();
        }
    }

    // hide gui toggle if enabled in settings
    if (configuration->hideMenuToggle()) {
        ui_->pushButtonToggleGUI->hide();
        ui_->pushButtonToggleGUI2->hide();
    }

    // init alpha values
    MainWindow::updateAlpha();

    // Hide auto day/night if needed
    if (this->lightsensor) {
        ui_->pushButtonDay->hide();
        ui_->pushButtonNight->hide();
        ui_->pushButtonDay2->hide();
        ui_->pushButtonNight2->hide();
        ui_->pushButtonBrightness->hide();
        ui_->pushButtonBrightness2->hide();
    }

    player = new QMediaPlayer(this);
    playlist = new QMediaPlaylist(this);
    connect(player, &QMediaPlayer::positionChanged, this, &MainWindow::on_positionChanged);
    connect(player, &QMediaPlayer::durationChanged, this, &MainWindow::on_durationChanged);
    connect(player, &QMediaPlayer::metaDataAvailableChanged, this, &MainWindow::metaDataChanged);
    connect(player, &QMediaPlayer::stateChanged, this, &MainWindow::on_StateChanged);

    ui_->pushButtonList->hide();
    ui_->pushButtonBackToPlayer->hide();
    ui_->PlayerPlayingWidget->hide();
    ui_->pushButtonPlayerStop->hide();
    ui_->pushButtonPlayerPause->hide();

    this->musicfolder = QString::fromStdString(configuration->getMp3MasterPath());
    this->albumfolder = QString::fromStdString(configuration->getMp3SubFolder());

    ui_->labelFolderpath->setText(this->musicfolder);
    ui_->labelAlbumpath->setText(this->albumfolder);

    ui_->labelFolderpath->hide();
    ui_->labelAlbumpath->hide();

    MainWindow::scanFolders();
    ui_->comboBoxAlbum->setCurrentText(QString::fromStdString(configuration->getMp3SubFolder()));
    MainWindow::scanFiles();
    player->setPlaylist(this->playlist);
    ui_->mp3List->setCurrentRow(configuration->getMp3Track());
    this->currentPlaylistIndex = configuration->getMp3Track();

    if (configuration->mp3AutoPlay()) {
        MainWindow::playerShow();
        MainWindow::playerHide();
        MainWindow::on_pushButtonPlayerPlayList_clicked();
        MainWindow::playerShow();
    }

    watcher = new QFileSystemWatcher(this);
    watcher->addPath("/media/USBDRIVES");
    connect(watcher, &QFileSystemWatcher::directoryChanged, this, &MainWindow::setTrigger);

    watcher_tmp = new QFileSystemWatcher(this);
    watcher_tmp->addPath("/tmp");
    connect(watcher_tmp, &QFileSystemWatcher::directoryChanged, this, &MainWindow::tmpChanged);
}

MainWindow::~MainWindow()
{
    delete ui_;
}

}
}
}
}

void f1x::openauto::autoapp::ui::MainWindow::customButtonPressed1()
{
    system(qPrintable(this->custom_button_command_c1 + " &"));
}

void f1x::openauto::autoapp::ui::MainWindow::customButtonPressed2()
{
    system(qPrintable(this->custom_button_command_c2 + " &"));
}

void f1x::openauto::autoapp::ui::MainWindow::customButtonPressed3()
{
    system(qPrintable(this->custom_button_command_c3 + " &"));
}

void f1x::openauto::autoapp::ui::MainWindow::customButtonPressed4()
{
    system(qPrintable(this->custom_button_command_c4 + " &"));
}

void f1x::openauto::autoapp::ui::MainWindow::customButtonPressed5()
{
    system(qPrintable(this->custom_button_command_c5 + " &"));
}

void f1x::openauto::autoapp::ui::MainWindow::customButtonPressed6()
{
    system(qPrintable(this->custom_button_command_c6 + " &"));
}


void f1x::openauto::autoapp::ui::MainWindow::on_pushButtonBrightness_clicked()
{
    this->brightnessFile = new QFile(this->brightnessFilename);
    this->brightnessFileAlt = new QFile(this->brightnessFilenameAlt);

    // Get the current brightness value
    if (!this->customBrightnessControl) {
        if (this->brightnessFile->open(QIODevice::ReadOnly)) {
            QByteArray data = this->brightnessFile->readAll();
            std::string::size_type sz;
            int brightness_val = std::stoi(data.toStdString(), &sz);
            ui_->horizontalSliderBrightness->setValue(brightness_val);
            QString bri=QString::number(brightness_val);
            ui_->brightnessValueLabel->setText(bri);
            this->brightnessFile->close();
        }
    } else {
        if (this->brightnessFileAlt->open(QIODevice::ReadOnly)) {
            QByteArray data = this->brightnessFileAlt->readAll();
            std::string::size_type sz;
            int brightness_val = std::stoi(data.toStdString(), &sz);
            ui_->horizontalSliderBrightness->setValue(brightness_val);
            QString bri=QString::number(brightness_val);
            ui_->brightnessValueLabel->setText(bri);
            this->brightnessFileAlt->close();
        }
    }
    ui_->BrightnessSliderControl->show();
    ui_->VolumeSliderControl->hide();
}

void f1x::openauto::autoapp::ui::MainWindow::on_pushButtonBrightness2_clicked()
{
    this->brightnessFile = new QFile(this->brightnessFilename);
    this->brightnessFileAlt = new QFile(this->brightnessFilenameAlt);

    // Get the current brightness value
    if (!this->customBrightnessControl) {
        if (this->brightnessFile->open(QIODevice::ReadOnly)) {
            QByteArray data = this->brightnessFile->readAll();
            std::string::size_type sz;
            int brightness_val = std::stoi(data.toStdString(), &sz);
            ui_->horizontalSliderBrightness->setValue(brightness_val);
            QString bri=QString::number(brightness_val);
            ui_->brightnessValueLabel->setText(bri);
            this->brightnessFile->close();
        }
    } else {
        if (this->brightnessFileAlt->open(QIODevice::ReadOnly)) {
            QByteArray data = this->brightnessFileAlt->readAll();
            std::string::size_type sz;
            int brightness_val = std::stoi(data.toStdString(), &sz);
            ui_->horizontalSliderBrightness->setValue(brightness_val);
            QString bri=QString::number(brightness_val);
            ui_->brightnessValueLabel->setText(bri);
            this->brightnessFileAlt->close();
        }
    }
    ui_->BrightnessSliderControl->show();
    ui_->VolumeSliderControl->hide();
}

void f1x::openauto::autoapp::ui::MainWindow::on_pushButtonVolume_clicked()
{
    ui_->horizontalSliderVolume->show();
    ui_->volumeValueLabel->show();
    if (this->toggleMute) {
        ui_->pushButtonUnMute->show();
    } else {
        ui_->pushButtonMute->show();
    }
    ui_->VolumeSliderControl->show();
    ui_->BrightnessSliderControl->hide();
}

void f1x::openauto::autoapp::ui::MainWindow::on_pushButtonVolume2_clicked()
{
    ui_->horizontalSliderVolume->show();
    ui_->volumeValueLabel->show();
    if (this->toggleMute) {
        ui_->pushButtonUnMute->show();
    } else {
        ui_->pushButtonMute->show();
    }
    ui_->VolumeSliderControl->show();
    ui_->BrightnessSliderControl->hide();
}

void f1x::openauto::autoapp::ui::MainWindow::on_horizontalSliderBrightness_valueChanged(int value)
{
    int n = snprintf(this->brightness_str, 5, "%d", value);
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
        }
    }
    QString bri=QString::number(value);
    ui_->brightnessValueLabel->setText(bri);
}

void f1x::openauto::autoapp::ui::MainWindow::on_horizontalSliderVolume_valueChanged(int value)
{
    int n = snprintf(this->volume_str, 5, "%d", value);
    QString vol=QString::number(value);
    ui_->volumeValueLabel->setText(vol+"%");
    system(("/usr/local/bin/autoapp_helper setvolume " + std::to_string(value) + "&").c_str());
}

void f1x::openauto::autoapp::ui::MainWindow::updateAlpha()
{
    int value = configuration_->getAlphaTrans();
    //int n = snprintf(this->alpha_str, 5, "%d", value);

    if (value != this->alpha_current_str) {
        this->alpha_current_str = value;
        double alpha = value/100.0;
        QString alp=QString::number(alpha);
        ui_->pushButtonExit->setStyleSheet( "background-color: rgba(164, 0, 0, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
        ui_->pushButtonShutdown->setStyleSheet( "background-color: rgba(239, 41, 41, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
        ui_->pushButtonReboot->setStyleSheet( "background-color: rgba(252, 175, 62, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
        ui_->pushButtonCancel->setStyleSheet( "background-color: rgba(32, 74, 135, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
        ui_->pushButtonBrightness->setStyleSheet( "background-color: rgba(245, 121, 0, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
        ui_->pushButtonVolume->setStyleSheet( "background-color: rgba(64, 191, 191, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
        ui_->pushButtonLock->setStyleSheet( "background-color: rgba(15, 54, 5, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
        ui_->pushButtonSettings->setStyleSheet( "background-color: rgba(138, 226, 52, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
        ui_->pushButtonDay->setStyleSheet( "background: rgba(252, 233, 79, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
        ui_->pushButtonNight->setStyleSheet( "background-color: rgba(114, 159, 207, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
        ui_->pushButtonCameraShow->setStyleSheet( "background-color: rgba(100, 62, 4, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
        ui_->pushButtonWifi->setStyleSheet( "background-color: rgba(252, 175, 62, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
        ui_->pushButtonToggleGUI->setStyleSheet( "background-color: rgba(237, 164, 255, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
        ui_->pushButton_c1->setStyleSheet( "background-color: rgba(" + this->custom_button_color_c1 + ", " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); color: rgb(255,255,255);");
        ui_->pushButton_c2->setStyleSheet( "background-color: rgba(" + this->custom_button_color_c2 + ", " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); color: rgb(255,255,255);");
        ui_->pushButton_c3->setStyleSheet( "background-color: rgba(" + this->custom_button_color_c3 + ", " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); color: rgb(255,255,255);");
        ui_->pushButton_c4->setStyleSheet( "background-color: rgba(" + this->custom_button_color_c4 + ", " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); color: rgb(255,255,255);");
        ui_->pushButton_c5->setStyleSheet( "background-color: rgba(" + this->custom_button_color_c5 + ", " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); color: rgb(255,255,255);");
        ui_->pushButton_c6->setStyleSheet( "background-color: rgba(" + this->custom_button_color_c6 + ", " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); color: rgb(255,255,255);");
        ui_->pushButtonDummy1->setStyleSheet( "background-color: rgba(186, 189, 182, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
        ui_->pushButtonDummy2->setStyleSheet( "background-color: rgba(186, 189, 182, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
        ui_->pushButtonDummy3->setStyleSheet( "background-color: rgba(186, 189, 182, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
        ui_->pushButtonDebug->setStyleSheet( "background-color: rgba(85, 87, 83, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
        ui_->pushButtonMusic->setStyleSheet( "background-color: rgba(78, 154, 6, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); color: rgb(255,255,255);");
        ui_->pushButtonAndroidAuto->setStyleSheet( "background-color: rgba(48, 140, 198, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); color: rgb(255,255,255);");
        ui_->pushButtonNoDevice->setStyleSheet( "background-color: rgba(48, 140, 198, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); color: rgb(255,255,255);");
        ui_->pushButtonNoWiFiDevice->setStyleSheet( "background-color: rgba(252, 175, 62, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); color: rgb(255,255,255);");
    }
}

void f1x::openauto::autoapp::ui::MainWindow::switchGuiToNight()
{
    MainWindow::on_pushButtonVolume_clicked();
    f1x::openauto::autoapp::ui::MainWindow::updateBG();
    ui_->pushButtonDay->show();
    ui_->pushButtonDay2->show();
    ui_->pushButtonNight->hide();
    ui_->pushButtonNight2->hide();
}

void f1x::openauto::autoapp::ui::MainWindow::switchGuiToDay()
{
    MainWindow::on_pushButtonVolume_clicked();
    f1x::openauto::autoapp::ui::MainWindow::updateBG();
    ui_->pushButtonNight->show();
    ui_->pushButtonNight2->show();
    ui_->pushButtonDay->hide();
    ui_->pushButtonDay2->hide();
}

void f1x::openauto::autoapp::ui::MainWindow::cameraControlHide()
{
    if (this->cameraButtonForce) {
        ui_->cameraWidget->hide();
        if (!this->oldGUIStyle) {
            ui_->menuWidget->show();
        } else {
            ui_->oldmenuWidget->show();
        }
    }
}

void f1x::openauto::autoapp::ui::MainWindow::cameraControlShow()
{
    if (this->cameraButtonForce) {
        if (!this->oldGUIStyle) {
            ui_->menuWidget->hide();
        } else {
            ui_->oldmenuWidget->hide();
        }
        ui_->cameraWidget->show();
    }
}

void f1x::openauto::autoapp::ui::MainWindow::playerShow()
{
    if (this->wallpaperEQFileExists) {
        this->setStyleSheet("QMainWindow { background: url(wallpaper-eq.png); background-repeat: no-repeat; background-position: center; }");
    } else {
        this->setStyleSheet("QMainWindow { background: url(:/black.png); background-repeat: no-repeat; background-position: center; }");
    }

    if (!this->oldGUIStyle) {
        ui_->menuWidget->hide();
    } else {
        ui_->oldmenuWidget->hide();
    }
    ui_->mediaWidget->show();
    ui_->VolumeSliderControlPlayer->show();
    ui_->VolumeSliderControl->hide();
    ui_->BrightnessSliderControl->hide();
}

void f1x::openauto::autoapp::ui::MainWindow::playerHide()
{
    if (!this->oldGUIStyle) {
        ui_->menuWidget->show();
    } else {
        ui_->oldmenuWidget->show();
    }
    ui_->mediaWidget->hide();
    ui_->VolumeSliderControl->show();
    ui_->VolumeSliderControlPlayer->hide();
    ui_->BrightnessSliderControl->hide();
    f1x::openauto::autoapp::ui::MainWindow::updateBG();
    f1x::openauto::autoapp::ui::MainWindow::tmpChanged();
}

void f1x::openauto::autoapp::ui::MainWindow::toggleExit()
{
    if (!this->exitMenuVisible) {
        ui_->exitWidget->show();
        ui_->buttonWidget->hide();
        ui_->horizontalWidgetButtons->hide();
        ui_->horizontalWidgetPower->show();
        this->exitMenuVisible = true;
    } else {
        ui_->buttonWidget->show();
        ui_->exitWidget->hide();
        ui_->horizontalWidgetButtons->show();
        ui_->horizontalWidgetPower->hide();
        this->exitMenuVisible = false;
    }
}

void f1x::openauto::autoapp::ui::MainWindow::toggleMuteButton()
{
    if (!this->toggleMute) {
        ui_->pushButtonMute->hide();
        ui_->pushButtonUnMute->show();
        this->toggleMute = true;
    } else {
        ui_->pushButtonUnMute->hide();
        ui_->pushButtonMute->show();
        this->toggleMute = false;
    }
}

void f1x::openauto::autoapp::ui::MainWindow::toggleGUI()
{
    if (!this->oldGUIStyle) {
        ui_->oldmenuWidget->show();
        ui_->menuWidget->hide();
        this->oldGUIStyle = true;
        if (!this->NoClock) {
            if (UseBigClock) {
                ui_->Digital_clock->hide();
                ui_->bigClock->show();
            } else {
                ui_->Digital_clock->show();
                ui_->bigClock->hide();
            }
        }
        MainWindow::on_pushButtonVolume_clicked();
    } else {
        ui_->menuWidget->show();
        ui_->oldmenuWidget->hide();
        this->oldGUIStyle = false;
        if (!this->NoClock) {
            ui_->Digital_clock->show();
        }
    }
    f1x::openauto::autoapp::ui::MainWindow::updateBG();
    f1x::openauto::autoapp::ui::MainWindow::tmpChanged();
}

void f1x::openauto::autoapp::ui::MainWindow::updateBG()
{
    if (this->date_text == "12/24") {
        this->setStyleSheet("QMainWindow { background: url(:/christmas.png); background-repeat: no-repeat; background-position: center; }");
        this->holidaybg = true;
    }
    else if (this->date_text == "12/31") {
        this->setStyleSheet("QMainWindow { background: url(:/firework.png); background-repeat: no-repeat; background-position: center; }");
        this->holidaybg = true;
    }
    else {
        if (!this->nightModeEnabled) {
            if (this->oldGUIStyle) {
                if (this->wallpaperClassicDayFileExists) {
                    //this->setStyleSheet( this->styleSheet().append("QMainWindow { background: url(wallpaper-classic.png); background-repeat: no-repeat; background-position: center; }") );
                    this->setStyleSheet("QMainWindow { background: url(wallpaper-classic.png); background-repeat: no-repeat; background-position: center; }");
                } else {
                    this->setStyleSheet("QMainWindow { background: url(:/black.png); background-repeat: no-repeat; background-position: center; }");
                }
            } else {
                if (this->wallpaperDayFileExists) {
                    //this->setStyleSheet( this->styleSheet().append("QMainWindow { background: url(wallpaper.png); background-repeat: no-repeat; background-position: center; }") );
                    this->setStyleSheet("QMainWindow { background: url(wallpaper.png); background-repeat: no-repeat; background-position: center; }");
                } else {
                    this->setStyleSheet("QMainWindow { background: url(:/black.png); background-repeat: no-repeat; background-position: center; }");
                }
            }
        } else {
            if (this->oldGUIStyle) {
                if (this->wallpaperClassicNightFileExists) {
                    //this->setStyleSheet( this->styleSheet().append("QMainWindow { background: url(wallpaper-classic-night.png); background-repeat: no-repeat; background-position: center; }") );
                    this->setStyleSheet( "QMainWindow { background: url(wallpaper-classic-night.png); background-repeat: no-repeat; background-position: center; }");
                } else {
                    this->setStyleSheet("QMainWindow { background: url(:/black.png); background-repeat: no-repeat; background-position: center; }");
                }
            } else {
                if (this->wallpaperNightFileExists) {
                    //this->setStyleSheet( this->styleSheet().append("QMainWindow { background: url(wallpaper-night.png); background-repeat: no-repeat; background-position: center; }") );
                    this->setStyleSheet("QMainWindow { background: url(wallpaper-night.png); background-repeat: no-repeat; background-position: center; }");
                } else {
                    this->setStyleSheet("QMainWindow { background: url(:/black.png); background-repeat: no-repeat; background-position: center; }");
                }
            }
        }
    }
}

void f1x::openauto::autoapp::ui::MainWindow::createDebuglog()
{
    system("/usr/local/bin/crankshaft debuglog &");
}

void f1x::openauto::autoapp::ui::MainWindow::setPairable()
{
    system("/usr/local/bin/autoapp_helper enablepairing &");
}

void f1x::openauto::autoapp::ui::MainWindow::setMute()
{
    system("/usr/local/bin/autoapp_helper setmute &");
}

void f1x::openauto::autoapp::ui::MainWindow::setUnMute()
{
    system("/usr/local/bin/autoapp_helper setunmute &");
}

void f1x::openauto::autoapp::ui::MainWindow::showTime()
{
    QTime time=QTime::currentTime();
    QDate date=QDate::currentDate();
    QString time_text=time.toString("hh : mm : ss");
    this->date_text=date.toString("MM/dd");

    if ((time.second() % 2) == 0) {
        time_text[3] = ' ';
        time_text[8] = ' ';
    }

    ui_->Digital_clock->setText(time_text);
    ui_->bigClock->setText(time_text);

    if (!this->holidaybg) {
        if (this->date_text == "12/24") {
            MainWindow::updateBG();
        }
        else if (this->date_text == "12/31") {
            MainWindow::updateBG();
        }
    }
}


void f1x::openauto::autoapp::ui::MainWindow::on_horizontalSliderProgressPlayer_sliderMoved(int position)
{
    player->setPosition(position);
}

void f1x::openauto::autoapp::ui::MainWindow::on_horizontalSliderVolumePlayer_sliderMoved(int position)
{
    player->setVolume(position);
    ui_->volumeValueLabelPlayer->setText(QString::number(position) + "%");
}

void f1x::openauto::autoapp::ui::MainWindow::on_pushButtonList_clicked()
{
    ui_->mp3selectWidget->show();
    ui_->PlayerPlayingWidget->hide();
    ui_->pushButtonList->hide();
    ui_->pushButtonPlayerPlayList->show();
    ui_->pushButtonBackToPlayer->show();
}

void f1x::openauto::autoapp::ui::MainWindow::on_pushButtonPlayerStop_clicked()
{
    ui_->mp3List->setCurrentRow(playlist->currentIndex());
    player->stop();
    ui_->pushButtonBack->setIcon(QPixmap("://coverlogo.png"));
    ui_->pushButtonPlayerPause->setStyleSheet( "background-color: rgb(233, 185, 110); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); color: rgb(0,0,0);");
    ui_->mp3selectWidget->show();
    ui_->PlayerPlayingWidget->hide();
    ui_->pushButtonBackToPlayer->hide();
    ui_->pushButtonPlayerPlayList->show();
    ui_->pushButtonPlayerStop->hide();
    ui_->pushButtonList->hide();
    ui_->pushButtonPlayerPause->hide();
    ui_->playerPositionTime->setText("00:00 / 00:00");
    ui_->labelCurrentPlaying->setText("");
    ui_->labelTrack->setText("");
}

void f1x::openauto::autoapp::ui::MainWindow::on_pushButtonPlayerPause_clicked()
{
    {
        if(player->state() == QMediaPlayer::PlayingState){
            player->pause();
            ui_->pushButtonPlayerPause->setStyleSheet( "background-color: rgb(218, 143, 143); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); color: rgb(0,0,0);");
        }else{
            ui_->pushButtonPlayerPause->setStyleSheet( "background-color: rgb(233, 185, 110); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); color: rgb(0,0,0);");
            player->play();
            player->setPosition(player->position());
        }

    }
}

void f1x::openauto::autoapp::ui::MainWindow::on_positionChanged(qint64 position)
{
    ui_->horizontalSliderProgressPlayer->setValue(position);

    //Setting the time
    QString time_elapsed, time_total;

    int total_seconds, total_minutes;

    total_seconds = (player->duration()/1000) % 60;
    total_minutes = (player->duration()/1000) / 60;

    if(total_minutes >= 60){
        int total_hours = (total_minutes/60);
        total_minutes = total_minutes - (total_hours*60);
        time_total = QString("%1").arg(total_hours, 2,10,QChar('0')) + ':' + QString("%1").arg(total_minutes, 2,10,QChar('0')) + ':' + QString("%1").arg(total_seconds, 2,10,QChar('0'));

    }else{
        time_total = QString("%1").arg(total_minutes, 2,10,QChar('0')) + ':' + QString("%1").arg(total_seconds, 2,10,QChar('0'));

    }

    //calculate time elapsed
    int seconds, minutes;

    seconds = (position/1000) % 60;
    minutes = (position/1000) / 60;

    //if minutes is over 60 then we should really display hours
    if(minutes >= 60){
        int hours = (minutes/60);
        minutes = minutes - (hours*60);
        time_elapsed = QString("%1").arg(hours, 2,10,QChar('0')) + ':' + QString("%1").arg(minutes, 2,10,QChar('0')) + ':' + QString("%1").arg(seconds, 2,10,QChar('0'));
    }else{
        time_elapsed = QString("%1").arg(minutes, 2,10,QChar('0')) + ':' + QString("%1").arg(seconds, 2,10,QChar('0'));
    }
    ui_->playerPositionTime->setText(time_elapsed + " / " + time_total);
}

void f1x::openauto::autoapp::ui::MainWindow::on_durationChanged(qint64 position)
{
    ui_->horizontalSliderProgressPlayer->setMaximum(position);
}

void f1x::openauto::autoapp::ui::MainWindow::on_mp3List_itemClicked(QListWidgetItem *item)
{
    this->selectedMp3file = item->text();
}

void f1x::openauto::autoapp::ui::MainWindow::metaDataChanged()
{
    QImage img = player->metaData(QMediaMetaData::CoverArtImage).value<QImage>();
    QImage imgscaled = img.scaled(270,270,Qt::IgnoreAspectRatio);
    if (!imgscaled.isNull()) {
    ui_->pushButtonBack->setIcon(QPixmap::fromImage(imgscaled));
    } else {
     ui_->pushButtonBack->setIcon(QPixmap("://coverlogo.png"));
    }

    QString fullpathplaying = player->currentMedia().canonicalUrl().toString();
    QString filename = QFileInfo(fullpathplaying).fileName();

    try {
        // use metadata from mp3list widget (prescanned id3 by taglib)
        if (playlist->currentIndex() != -1 && fullpathplaying != "") {
            QString currentsong = ui_->mp3List->item(playlist->currentIndex())->text();
            ui_->labelCurrentPlaying->setText(currentsong);
        }
    } catch (...) {
        // use metadata from player
        QString AlbumInterpret = player->metaData(QMediaMetaData::AlbumArtist).toString();
        QString Title = player->metaData(QMediaMetaData::Title).toString();

        if (AlbumInterpret == "" && ui_->comboBoxAlbum->currentText() != ".") {
            AlbumInterpret = ui_->comboBoxAlbum->currentText();
        }
        QString currentPlaying;

        if (AlbumInterpret != "") {
            currentPlaying.append(AlbumInterpret);
        }
        if (Title != "" && AlbumInterpret != "") {
            currentPlaying.append(" - ");
        }
        if (Title != "") {
            currentPlaying.append(Title);
        }
        ui_->labelCurrentPlaying->setText(currentPlaying);
    }
    ui_->labelTrack->setText(QString::number(playlist->currentIndex()+1));
    ui_->labelTrackCount->setText(QString::number(playlist->mediaCount()));

    // Write current playing album and track to config
    this->configuration_->setMp3Track(playlist->currentIndex());
    this->configuration_->setMp3SubFolder(ui_->comboBoxAlbum->currentText().toStdString());
    this->configuration_->save();
}

void f1x::openauto::autoapp::ui::MainWindow::on_pushButtonPlayerPlayList_clicked()
{
    player->setPlaylist(this->playlist);
    playlist->setCurrentIndex(this->currentPlaylistIndex);
    player->play();
    ui_->mp3selectWidget->hide();
    ui_->PlayerPlayingWidget->show();
    ui_->pushButtonPlayerPlayList->hide();
    ui_->pushButtonList->show();
    ui_->pushButtonBackToPlayer->hide();
    ui_->pushButtonPlayerStop->show();
    ui_->pushButtonPlayerPause->setStyleSheet( "background-color: rgb(233, 185, 110); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); color: rgb(0,0,0);");
    ui_->pushButtonPlayerPause->show();
    int currentalbum = ui_->comboBoxAlbum->currentIndex();
    ui_->labelCurrentAlbumIndex->setText(QString::number(currentalbum+1));
}

void f1x::openauto::autoapp::ui::MainWindow::on_comboBoxAlbum_currentIndexChanged(const QString &arg1)
{
    this->albumfolder = arg1;
    MainWindow::scanFiles();
}

void f1x::openauto::autoapp::ui::MainWindow::setTrigger()
{
    this->mediacontentchanged = true;

    ui_->SysinfoTopLeft->setText("Media changed - Scanning ...");
    ui_->SysinfoTopLeft->show();

    QTimer::singleShot(10000, this, SLOT(scanFolders()));
}

void f1x::openauto::autoapp::ui::MainWindow::setRetryUSBConnect()
{
    ui_->SysinfoTopLeft->setText("Trying USB connect ...");
    ui_->SysinfoTopLeft->show();

    QTimer::singleShot(10000, this, SLOT(resetRetryUSBMessage()));
}

void f1x::openauto::autoapp::ui::MainWindow::resetRetryUSBMessage()
{
    ui_->SysinfoTopLeft->setText("");
    ui_->SysinfoTopLeft->hide();
}

void f1x::openauto::autoapp::ui::MainWindow::scanFolders()
{
    try {
        if (this->mediacontentchanged == true) {
            this->mediacontentchanged = false;
            int cleaner = ui_->comboBoxAlbum->count();
            while (cleaner > -1) {
                ui_->comboBoxAlbum->removeItem(cleaner);
                cleaner--;
            }
            QDir directory(this->musicfolder);
            QStringList folders = directory.entryList(QStringList() << "*", QDir::AllDirs, QDir::Name);
            foreach (QString foldername, folders) {
                if (foldername != "..") {
                    ui_->comboBoxAlbum->addItem(foldername);
                    ui_->labelAlbumCount->setText(QString::number(ui_->comboBoxAlbum->count()));
                }
            }
            this->currentPlaylistIndex = 0;
            ui_->SysinfoTopLeft->hide();
        }
    }
    catch(...) {
        ui_->SysinfoTopLeft->hide();
    }
}

void f1x::openauto::autoapp::ui::MainWindow::scanFiles()
{
    if (this->mediacontentchanged == false) {
        int cleaner = ui_->mp3List->count();
        while (cleaner > -1) {
            ui_->mp3List->takeItem(cleaner);
            cleaner--;
        }
        this->playlist->clear();

        QList<QMediaContent> content;
        QDir directory(this->musicfolder + "/" + this->albumfolder);
        QStringList mp3s = directory.entryList(QStringList() << "*.mp3" << "*.flac" << "*.aac" << "*.ogg" << "*.mp4" << "*.mp4a" << "*.wma",QDir::Files, QDir::Name);
        foreach (QString filename, mp3s) {
            // add to mediacontent
            content.push_back(QMediaContent(QUrl::fromLocalFile(this->musicfolder + "/" + this->albumfolder + "/" + filename)));
            // add items to gui
            // read metadata using taglib
            try {
                TagLib::FileRef file((this->musicfolder + "/" + this->albumfolder + "/" + filename).toUtf8(),true);
                TagLib::String artist_string = file.tag()->artist();
                TagLib::String title_string = file.tag()->title();
                TagLib::uint track_string = file.tag()->track();
                QString artistid3 = QString::fromStdWString(artist_string.toCWString());
                QString titleid3 = QString::fromStdWString(title_string.toCWString());
                QString trackid3 = QString::number(track_string);
                int tracklength = trackid3.length();
                if (tracklength < 2) {
                    trackid3 = "0" + trackid3;
                }
                QString ID3Entry = trackid3 + ": " + artistid3 + " - " + titleid3;
                ui_->mp3List->addItem(ID3Entry);
            } catch (...) {
                // old way only adding filename to list
                ui_->mp3List->addItem(filename);
            }
        }
        // set playlist
        this->playlist->addMedia(content);
    }
}

void f1x::openauto::autoapp::ui::MainWindow::on_mp3List_currentRowChanged(int currentRow)
{
    ui_->labelFolderpath->setText(QString::number(currentRow));
    this->currentPlaylistIndex = currentRow;
}

void f1x::openauto::autoapp::ui::MainWindow::on_pushButtonPlayerNextBig_clicked()
{
    playlist->next();
}

void f1x::openauto::autoapp::ui::MainWindow::on_pushButtonPlayerPrevBig_clicked()
{
    playlist->previous();
}

void f1x::openauto::autoapp::ui::MainWindow::on_pushButtonPlayerPrevAlbum_clicked()
{
    int albumcount = ui_->comboBoxAlbum->count();
    int currentalbum = ui_->comboBoxAlbum->currentIndex();
    if (currentalbum >= 1) {
        currentalbum = currentalbum-1;
        ui_->comboBoxAlbum->setCurrentIndex(currentalbum);
        ui_->labelCurrentAlbumIndex->setText(QString::number(currentalbum+1));
        player->play();
    } else {
        currentalbum = albumcount-1;
        ui_->comboBoxAlbum->setCurrentIndex(currentalbum);
        ui_->labelCurrentAlbumIndex->setText(QString::number(currentalbum+1));
        player->play();
    }
}

void f1x::openauto::autoapp::ui::MainWindow::on_pushButtonPlayerNextAlbum_clicked()
{
    int albumcount = ui_->comboBoxAlbum->count();
    int currentalbum = ui_->comboBoxAlbum->currentIndex();
    if (currentalbum < albumcount-1) {
        currentalbum = currentalbum + 1;
        ui_->comboBoxAlbum->setCurrentIndex(currentalbum);
        ui_->labelCurrentAlbumIndex->setText(QString::number(currentalbum+1));
        player->play();
    } else {
        currentalbum = 0;
        ui_->comboBoxAlbum->setCurrentIndex(currentalbum);
        ui_->labelCurrentAlbumIndex->setText(QString::number(currentalbum+1));
        player->play();
    }
}

void f1x::openauto::autoapp::ui::MainWindow::on_pushButtonBackToPlayer_clicked()
{
    ui_->PlayerPlayingWidget->show();
    ui_->mp3selectWidget->hide();
    ui_->pushButtonBackToPlayer->hide();
    ui_->pushButtonPlayerPlayList->hide();
    ui_->pushButtonList->show();
}

void f1x::openauto::autoapp::ui::MainWindow::on_StateChanged(QMediaPlayer::State state)
{
    if (state == QMediaPlayer::StoppedState || state == QMediaPlayer::PausedState) {
        std::remove("/tmp/media_playing");
    } else {
        std::ofstream("/tmp/media_playing");
    }
}


bool f1x::openauto::autoapp::ui::MainWindow::check_file_exist(const char *fileName)
{
    std::ifstream ifile(fileName, std::ios::in);
    // file not ok - checking if symlink
    if (!ifile.good()) {
        QFileInfo linkFile = QString(fileName);
        if (linkFile.isSymLink()) {
            QFileInfo linkTarget(linkFile.symLinkTarget());
            return linkTarget.exists();
        } else {
            return ifile.good();
        }
    } else {
        return ifile.good();
    }
}

void f1x::openauto::autoapp::ui::MainWindow::tmpChanged()
{
    try {
        if (std::ifstream("/tmp/entityexit")) {
            MainWindow::TriggerAppStop();
            usleep(1000000);
            std::remove("/tmp/entityexit");
        }
    } catch (...) {
        OPENAUTO_LOG(error) << "[OpenAuto] Error in entityexit";
    }

    // check if system is in display off mode (tap2wake)
    if (std::ifstream("/tmp/blankscreen")) {
        if (ui_->centralWidget->isVisible() == true) {
            ui_->centralWidget->hide();
        }
    } else {
        if (ui_->centralWidget->isVisible() == false) {
            ui_->centralWidget->show();
        }
    }

    // check if custom command needs black background
    if (std::ifstream("/tmp/blackscreen")) {
        if (ui_->centralWidget->isVisible() == true) {
            ui_->centralWidget->hide();
            this->setStyleSheet("QMainWindow {background-color: rgb(0,0,0);}");
            this->background_set = false;
        }
    } else {
        if (this->background_set == false) {
            f1x::openauto::autoapp::ui::MainWindow::updateBG();
            this->background_set = true;
        }
    }

    // check if phone is conencted to usb
    if (std::ifstream("/tmp/android_device")) {
        if (ui_->pushButtonAndroidAuto->isVisible() == false) {
            ui_->pushButtonAndroidAuto->show();
            ui_->pushButtonNoDevice->hide();
        }
        if (ui_->pushButtonAndroidAuto2->isVisible() == false) {
            ui_->pushButtonAndroidAuto2->show();
            ui_->pushButtonNoDevice2->hide();
        }
    } else {
        if (ui_->pushButtonAndroidAuto->isVisible() == true) {
            ui_->pushButtonNoDevice->show();
            ui_->pushButtonAndroidAuto->hide();
        }
        if (ui_->pushButtonAndroidAuto2->isVisible() == true) {
            ui_->pushButtonNoDevice2->show();
            ui_->pushButtonAndroidAuto2->hide();
        }
    }

    // check if bluetooth available
    this->bluetoothEnabled = check_file_exist("/tmp/button_bluetooth_visible");

    if (this->bluetoothEnabled) {
        if (ui_->pushButtonBluetooth->isVisible() == false) {
            ui_->pushButtonBluetooth->show();
        }
    } else {
        if (ui_->pushButtonBluetooth->isVisible() == true) {
            ui_->pushButtonBluetooth->hide();
        }
    }

    // check if a device is connected via bluetooth
    if (std::ifstream("/tmp/btdevice")) {
        if (ui_->btDevice->isVisible() == false) {
            QFile phoneBTData(QString("/tmp/btdevice"));
            phoneBTData.open(QIODevice::ReadOnly);
            QTextStream data_date(&phoneBTData);
            QString linedate = data_date.readAll();
            phoneBTData.close();
            ui_->btDevice->setText(linedate.simplified());
            ui_->btDevice->show();
        }
    } else {
        if (ui_->btDevice->isVisible() == true) {
            ui_->btDevice->hide();
        }
    }

    if (std::ifstream("/tmp/config_in_progress") || std::ifstream("/tmp/debug_in_progress") || std::ifstream("/tmp/enable_pairing")) {
        if (ui_->SysinfoTopLeft2->isVisible() == false) {
            if (std::ifstream("/tmp/config_in_progress")) {
                ui_->pushButtonSettings->hide();
                ui_->pushButtonSettings2->hide();
                ui_->pushButtonLock->show();
                ui_->pushButtonLock2->show();
                ui_->SysinfoTopLeft2->setText("Config in progress ...");
                ui_->SysinfoTopLeft2->show();
            }
            if (std::ifstream("/tmp/debug_in_progress")) {
                ui_->pushButtonSettings->hide();
                ui_->pushButtonSettings2->hide();
                ui_->pushButtonDebug->hide();
                ui_->pushButtonDebug2->hide();
                ui_->pushButtonLock->show();
                ui_->pushButtonLock2->show();
                ui_->SysinfoTopLeft2->setText("Creating debug.zip ...");
                ui_->SysinfoTopLeft2->show();
            }
            if (std::ifstream("/tmp/enable_pairing")) {
                ui_->pushButtonDebug->hide();
                ui_->pushButtonDebug2->hide();
                ui_->SysinfoTopLeft2->setText("Pairing enabled for 120 seconds!");
                ui_->SysinfoTopLeft2->show();
            }
        }
    } else {
        if (ui_->SysinfoTopLeft2->isVisible() == true) {
            ui_->SysinfoTopLeft2->setText("");
            ui_->SysinfoTopLeft2->hide();
            ui_->pushButtonSettings->show();
            ui_->pushButtonSettings2->show();
            ui_->pushButtonLock->hide();
            ui_->pushButtonLock2->hide();
            if (this->systemDebugmode) {
                ui_->pushButtonDebug->show();
                ui_->pushButtonDebug2->show();
            }
        }
    }

    // update day/night state
    this->nightModeEnabled = check_file_exist("/tmp/night_mode_enabled");

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

    // camera stuff
    if (this->cameraButtonForce) {

        // check if dashcam is recording
        this->dashCamRecording = check_file_exist("/tmp/dashcam_is_recording");
        // show recording state if dashcam is visible
        if (ui_->cameraWidget->isVisible() == true) {
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

        // check if rearcam is eanbled
        this->rearCamEnabled = check_file_exist("/tmp/rearcam_enabled");
        if (this->rearCamEnabled) {
            if (!this->rearCamVisible) {
                this->rearCamVisible = true;
                f1x::openauto::autoapp::ui::MainWindow::MainWindow::showRearCam();
            }
        } else {
            if (this->rearCamVisible) {
                this->rearCamVisible = false;
                f1x::openauto::autoapp::ui::MainWindow::MainWindow::hideRearCam();
            }
        }
    }

    // check if shutdown is external triggered and init clean app exit
    if (std::ifstream("/tmp/external_exit")) {
        f1x::openauto::autoapp::ui::MainWindow::MainWindow::exit();
    }

    this->hotspotActive = check_file_exist("/tmp/hotspot_active");

    // hide wifi if hotspot disabled
    if (!this->hotspotActive) {
        if ((ui_->AAWIFIWidget->isVisible() == true) || (ui_->AAWIFIWidget2->isVisible() == true)){
            ui_->AAWIFIWidget->hide();
            ui_->AAWIFIWidget2->hide();
            ui_->AAUSBWidget->show();
            ui_->AAUSBWidget2->show();
        }
    } else {
        if ((ui_->AAWIFIWidget->isVisible() == false) || (ui_->AAWIFIWidget2->isVisible() == false)) {
            ui_->AAWIFIWidget->show();
            ui_->AAWIFIWidget2->show();
            ui_->AAUSBWidget->hide();
            ui_->AAUSBWidget2->hide();
        }
    }

    if (std::ifstream("/tmp/temp_recent_list")) {
        if (ui_->pushButtonWifi->isVisible() == false) {
            ui_->pushButtonWifi->show();
        }
        if (ui_->pushButtonNoWiFiDevice->isVisible() == true) {
            ui_->pushButtonNoWiFiDevice->hide();
        }
        if (ui_->pushButtonWifi2->isVisible() == false) {
            ui_->pushButtonWifi2->show();
        }
        if (ui_->pushButtonNoWiFiDevice2->isVisible() == true) {
            ui_->pushButtonNoWiFiDevice2->hide();
        }
    } else {
        if (ui_->pushButtonWifi->isVisible() == true) {
            ui_->pushButtonWifi->hide();
        }
        if (ui_->pushButtonNoWiFiDevice->isVisible() == false) {
            ui_->pushButtonNoWiFiDevice->show();
        }
        if (ui_->pushButtonWifi2->isVisible() == true) {
            ui_->pushButtonWifi2->hide();
        }
        if (ui_->pushButtonNoWiFiDevice2->isVisible() == false) {
            ui_->pushButtonNoWiFiDevice2->show();
        }
    }

    // handle dummys in classic menu
    int button_count = 0;
    if (ui_->pushButtonCameraShow2->isVisible() == true) {
        button_count = button_count + 1;
    }
    if (ui_->AAWIFIWidget2->isVisible() == true) {
        button_count = button_count + 1;
    }
    if (ui_->pushButtonDebug2->isVisible() == true) {
        button_count = button_count + 1;
    }
    if (ui_->pushButtonVolume2->isVisible() == true) {
        button_count = button_count + 1;
    }

    // Hide auto day/night if needed
    if (this->lightsensor) {
        ui_->pushButtonDay->hide();
        ui_->pushButtonNight->hide();
        ui_->pushButtonDay2->hide();
        ui_->pushButtonNight2->hide();
        ui_->pushButtonBrightness->hide();
        ui_->pushButtonBrightness2->hide();
    }

    // use big clock in classic gui?
    if (this->configuration_->showBigClock()) {
        this->UseBigClock = true;
    } else {
        this->UseBigClock = false;
    }

    // clock viibility by settings
    if (!this->configuration_->showClock()) {
        ui_->Digital_clock->hide();
        ui_->oldmenuDummy->show();
        ui_->bigClock->hide();
        this->NoClock = true;
    } else {
        this->NoClock = false;
        if (this->UseBigClock && ui_->oldmenuWidget->isVisible() == true) {
            ui_->oldmenuDummy->hide();
            ui_->bigClock->show();
            if (oldGUIStyle) {
                ui_->Digital_clock->hide();
            }
        } else {
            ui_->oldmenuDummy->show();
            ui_->Digital_clock->show();
            ui_->bigClock->hide();
        }
    }

    // hide gui toggle if enabled in settings
    if (this->configuration_->hideMenuToggle()) {
        ui_->pushButtonToggleGUI->hide();
        ui_->pushButtonToggleGUI2->hide();
    } else {
        ui_->pushButtonToggleGUI->show();
        ui_->pushButtonToggleGUI2->show();
    }

    // read value from tsl2561
    if (std::ifstream("/tmp/tsl2561") && this->configuration_->showLux()) {
        QFile paramFile("/tmp/tsl2561");
        paramFile.open(QIODevice::ReadOnly);
        QTextStream data(&paramFile);
        QStringList value = data.readAll().split("\n");
        paramFile.close();
        if (ui_->label_left->isVisible() == false) {
            ui_->label_left->show();
            ui_->label_right->show();
        }
        ui_->label_left->setText("Lux: " + value[0]);
    } else {
        if (ui_->label_left->isVisible() == true) {
            ui_->label_left->hide();
            ui_->label_right->hide();
            ui_->label_left->setText("");
            ui_->label_right->setText("");
        }
    }
    MainWindow::updateAlpha();
}
