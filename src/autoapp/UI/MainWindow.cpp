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
#include <QProcess>

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

    // trigger files
    QFileInfo nightModeFile("/tmp/night_mode_enabled");
    this->nightModeEnabled = nightModeFile.exists();

    QFileInfo devModeFile("/tmp/dev_mode_enabled");
    this->devModeEnabled = devModeFile.exists();

    QFileInfo wifiButtonFile("/etc/button_wifi_visible");
    this->wifiButtonForce = wifiButtonFile.exists();

    QFileInfo cameraButtonFile("/etc/button_camera_visible");
    this->cameraButtonForce = cameraButtonFile.exists();

    QFileInfo brightnessButtonFile("/etc/button_brightness_visible");
    this->brightnessButtonForce = brightnessButtonFile.exists();

    QFileInfo DebugmodeFile("/tmp/usb_debug_mode");
    this->systemDebugmode = DebugmodeFile.exists();

    QFileInfo c1ButtonFile(this->custom_button_file_c1);
    this->c1ButtonForce = c1ButtonFile.exists();

    QFileInfo c2ButtonFile(this->custom_button_file_c2);
    this->c2ButtonForce = c2ButtonFile.exists();

    QFileInfo c3ButtonFile(this->custom_button_file_c3);
    this->c3ButtonForce = c3ButtonFile.exists();

    QFileInfo c4ButtonFile(this->custom_button_file_c4);
    this->c4ButtonForce = c4ButtonFile.exists();

    QFileInfo c5ButtonFile(this->custom_button_file_c5);
    this->c5ButtonForce = c5ButtonFile.exists();

    QFileInfo c6ButtonFile(this->custom_button_file_c6);
    this->c6ButtonForce = c6ButtonFile.exists();

    QFileInfo c7ButtonFile(this->custom_button_file_c7);
    this->c7ButtonForce = c7ButtonFile.exists();

    // wallpaper stuff
    QFileInfo wallpaperDayFile("wallpaper.png");
    this->wallpaperDayFileExists = wallpaperDayFile.exists();

    QFileInfo wallpaperNightFile("wallpaper-night.png");
    this->wallpaperNightFileExists = wallpaperNightFile.exists();

    QFileInfo wallpaperClassicDayFile("wallpaper-classic.png");
    this->wallpaperDayFileExists = wallpaperDayFile.exists();

    QFileInfo wallpaperClassicNightFile("wallpaper-classic-night.png");
    this->wallpaperNightFileExists = wallpaperNightFile.exists();

    if (wallpaperDayFile.isSymLink()) {
        QFileInfo linkTarget(wallpaperDayFile.symLinkTarget());
        this->wallpaperDayFileExists = linkTarget.exists();
    }

    if (wallpaperNightFile.isSymLink()) {
        QFileInfo linkTarget(wallpaperNightFile.symLinkTarget());
        this->wallpaperNightFileExists = linkTarget.exists();
    }

    if (wallpaperClassicDayFile.isSymLink()) {
        QFileInfo linkTarget(wallpaperClassicDayFile.symLinkTarget());
        this->wallpaperClassicDayFileExists = linkTarget.exists();
    }

    if (wallpaperClassicNightFile.isSymLink()) {
        QFileInfo linkTarget(wallpaperClassicNightFile.symLinkTarget());
        this->wallpaperClassicNightFileExists = linkTarget.exists();
    }

    ui_->setupUi(this);
    connect(ui_->pushButtonSettings, &QPushButton::clicked, this, &MainWindow::openSettings);
    connect(ui_->pushButtonSettings2, &QPushButton::clicked, this, &MainWindow::openSettings);
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
    connect(ui_->pushButtonAlpha, &QPushButton::clicked, this, &MainWindow::showAlphaSlider);
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
    connect(ui_->pushButtonUSB, &QPushButton::clicked, this, &MainWindow::openUSBDialog);
    connect(ui_->pushButtonRescan, &QPushButton::clicked, this, &MainWindow::scanFolders);

    // by default hide bluetooth button on init
    ui_->pushButtonBluetooth->hide();

    // by default hide media player
    ui_->mediaWidget->hide();
    
    QTimer *timer=new QTimer(this);
    connect(timer, SIGNAL(timeout()),this,SLOT(showTime()));
    timer->start(1000);

    // Build Version string for mainscreen
    // Get git version string
    QFileInfo vFile("/etc/crankshaft.build");
    if (vFile.exists()) {
        QFile versionFile(QString("/etc/crankshaft.build"));
        versionFile.open(QIODevice::ReadOnly);
        QTextStream data_version(&versionFile);
        QString lineversion = data_version.readAll();
        versionFile.close();
        this->bversion=lineversion.simplified();
    } else {
        this->bversion="unknown";
    }

    // Get date string
    QFileInfo dFile("/etc/crankshaft.build");
    if (dFile.exists()) {
        QFile dateFile(QString("/etc/crankshaft.date"));
        dateFile.open(QIODevice::ReadOnly);
        QTextStream data_date(&dateFile);
        QString linedate = data_date.readAll();
        dateFile.close();
        this->bdate=linedate.simplified();
    } else {
        this->bdate="- - -";
    }

    QString buildid = "Build: " + this->bversion + " (" + this->bdate + ")";
    ui_->BuildID->setText(buildid);

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
        ui_->pushButtonDummyCamWifi->hide();
    } else {
        ui_->pushButtonCameraShow->hide();
        ui_->pushButtonCameraShow2->hide();
    }

    // show debug button if enabled
    if (!this->systemDebugmode) {
        ui_->pushButtonDebug->hide();
        ui_->pushButtonDebug2->hide();
    }

    ui_->systemConfigInProgress->hide();
    ui_->pushButtonLock->hide();
    ui_->pushButtonLock2->hide();
    ui_->btDevice->hide();

    // hide brightness slider of control file is not existing
    QFileInfo brightnessFile(brightnessFilename);
    if (!brightnessFile.exists() && !this->brightnessButtonForce) {
        ui_->pushButtonBrightness->hide();
    }

    // as default hide brightness slider
    ui_->BrightnessSliderControl->hide();

    // as default hide alpha slider and button
    ui_->pushButtonAlpha->hide();
    ui_->AlphaSliderControl->hide();

    // as default hide volume slider player
    ui_->VolumeSliderControlPlayer->hide();

    // as default hide power buttons
    ui_->pushButtonShutdown->hide();
    ui_->pushButtonReboot->hide();
    ui_->pushButtonCancel->hide();
    ui_->exitWidget->hide();

    // as default hide phone connected label
    ui_->phoneConnected->hide();

    // as default hide muted button
    ui_->pushButtonUnMute->hide();

    // hide wifi if not forced
    if (!this->wifiButtonForce) {
        ui_->pushButtonWifi->hide();
        ui_->pushButtonWifi2->hide();
    } else {
        ui_->pushButtonDummyCamWifi->hide();
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

    if (!this->c7ButtonForce) {
        ui_->pushButton_c7->hide();
    } else {
        // read button config 7
        QFile paramFile(this->custom_button_file_c7);
        paramFile.open(QIODevice::ReadOnly);
        QTextStream data(&paramFile);
        QStringList params = data.readAll().split("#");
        paramFile.close();
        ui_->pushButton_c7->setText(params[0].simplified());
        this->custom_button_command_c7 = params[1].simplified();
        if (params[2] != "") {
            this->custom_button_color_c7 = params[2].simplified();
        }
        connect(ui_->pushButton_c7, &QPushButton::clicked, this, &MainWindow::customButtonPressed7);
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
    QProcess processbrigthness;
    processbrigthness.start("/usr/local/bin/autoapp_helper getbrightnessvalues");
    processbrigthness.waitForFinished();
    QString brigthness = processbrigthness.readAllStandardOutput();
    brigthness.resize(brigthness.size()-1); // remove last line break
    QStringList brigthnessvalues = brigthness.split("#");

    // set brightness slider attribs
    ui_->horizontalSliderBrightness->setMinimum(brigthnessvalues[0].toInt());
    ui_->horizontalSliderBrightness->setMaximum(brigthnessvalues[1].toInt());
    ui_->horizontalSliderBrightness->setSingleStep(brigthnessvalues[2].toInt());
    ui_->horizontalSliderBrightness->setTickInterval(brigthnessvalues[2].toInt());

    // run monitor for custom brightness command if enabled in crankshaft_env.sh
    if (brigthnessvalues[3] == "1") {
        ui_->pushButtonBrightness->show();
        this->customBrightnessControl = true;
        system("/usr/local/bin/autoapp_helper startcustombrightness &");
    }

    // read param file
    QFileInfo audioparamFile("/boot/crankshaft/volume");
    if (audioparamFile.exists()) {
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
    if (!this->nightModeEnabled) {
        if (this->oldGUIStyle) {
            if (this->wallpaperClassicDayFileExists) {
                this->setStyleSheet("QMainWindow { background: url(wallpaper-classic.png); background-repeat: no-repeat; background-position: center; }");
            } else {
                this->setStyleSheet("QMainWindow { background: url(:/black.png); background-repeat: no-repeat; background-position: center; }");
            }
        } else {
            if (this->wallpaperDayFileExists) {
                this->setStyleSheet("QMainWindow { background: url(wallpaper.png); background-repeat: no-repeat; background-position: center; }");
            } else {
                this->setStyleSheet("QMainWindow { background: url(:/black.png); background-repeat: no-repeat; background-position: center; }");
            }
        }
        ui_->pushButtonDay->hide();
        ui_->pushButtonDay2->hide();
        ui_->pushButtonNight->show();
        ui_->pushButtonNight2->show();
    } else {
        if (this->oldGUIStyle) {
            if (this->wallpaperClassicNightFileExists) {
                this->setStyleSheet("QMainWindow { background: url(wallpaper-classic-night.png); background-repeat: no-repeat; background-position: center; }");
            } else {
                this->setStyleSheet("QMainWindow { background: url(:/black.png); background-repeat: no-repeat; background-position: center; }");
            }
        } else {
            if (this->wallpaperNightFileExists) {
                this->setStyleSheet("QMainWindow { background: url(wallpaper-night.png); background-repeat: no-repeat; background-position: center; }");
            } else {
                this->setStyleSheet("QMainWindow { background: url(:/black.png); background-repeat: no-repeat; background-position: center; }");
            }
        }
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
            ui_->Digital_clock->show();
            ui_->bigClock->hide();
        }
    }

    // hide gui toggle if enabled in settings
    if (configuration->hideMenuToggle()) {
        ui_->pushButtonToggleGUI->hide();
        ui_->pushButtonToggleGUI2->hide();
    }

    // hide alpha controls if enabled in settings
    if (!configuration->hideAlpha()) {
        ui_->pushButtonAlpha->show();
    }

    // init alpha values
    ui_->horizontalSliderAlpha->setValue(int(configuration->getAlphaTrans()));
    MainWindow::on_horizontalSliderAlpha_valueChanged(int(configuration->getAlphaTrans()));

    player = new QMediaPlayer(this);
    playlist = new QMediaPlaylist(this);
    connect(player, &QMediaPlayer::positionChanged, this, &MainWindow::on_positionChanged);
    connect(player, &QMediaPlayer::durationChanged, this, &MainWindow::on_durationChanged);
    connect(player, &QMediaPlayer::metaDataAvailableChanged, this, &MainWindow::metaDataChanged);

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

    // link possible existing media
    system(qPrintable("ln -s /media/CSSTORAGE/Music/* /media/MYMEDIA"));
    system(qPrintable("/usr/local/bin/autoapp_helper cleansymlinks"));

    MainWindow::scanFolders();
    ui_->comboBoxAlbum->setCurrentText(QString::fromStdString(configuration->getMp3SubFolder()));
    MainWindow::scanFiles();
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

void f1x::openauto::autoapp::ui::MainWindow::customButtonPressed7()
{
    system(qPrintable(this->custom_button_command_c7 + " &"));
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
    ui_->AlphaSliderControl->hide();
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
    ui_->AlphaSliderControl->hide();
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
    ui_->AlphaSliderControl->hide();

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
    ui_->AlphaSliderControl->hide();
}

void f1x::openauto::autoapp::ui::MainWindow::on_pushButtonAlpha_clicked()
{
    ui_->AlphaSliderControl->show();
    ui_->VolumeSliderControl->hide();
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

void f1x::openauto::autoapp::ui::MainWindow::on_horizontalSliderAlpha_valueChanged(int value)
{
    int n = snprintf(this->alpha_str, 5, "%d", value);
    double alpha = value/100.0;
    QString alp=QString::number(alpha);
    ui_->alphaValueLabel->setText(alp);
    ui_->pushButtonAlpha->setStyleSheet( "background-color: rgba(243, 243, 243, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
    ui_->pushButtonExit->setStyleSheet( "background-color: rgba(164, 0, 0, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
    ui_->pushButtonShutdown->setStyleSheet( "background-color: rgba(239, 41, 41, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
    ui_->pushButtonReboot->setStyleSheet( "background-color: rgba(252, 175, 62, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
    ui_->pushButtonCancel->setStyleSheet( "background-color: rgba(32, 74, 135, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
    ui_->pushButtonBrightness->setStyleSheet( "background-color: rgba(245, 121, 0, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
    ui_->pushButtonVolume->setStyleSheet( "background-color: rgba(64, 191, 191, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
    ui_->pushButtonAlpha->setStyleSheet( "background-color: rgba(173, 127, 168, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
    ui_->pushButtonLock->setStyleSheet( "background-color: rgba(15, 54, 5, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
    ui_->pushButtonSettings->setStyleSheet( "background-color: rgba(138, 226, 52, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
    ui_->pushButtonDay->setStyleSheet( "background: rgba(252, 233, 79, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
    ui_->pushButtonNight->setStyleSheet( "background-color: rgba(114, 159, 207, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
    ui_->pushButtonCameraShow->setStyleSheet( "background-color: rgba(100, 62, 4, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
    ui_->pushButtonDummyCamWifi->setStyleSheet( "background-color: rgba(117, 80, 123, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
    ui_->pushButtonWifi->setStyleSheet( "background-color: rgba(252, 175, 62, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
    ui_->pushButtonToggleGUI->setStyleSheet( "background-color: rgba(237, 164, 255, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
    ui_->pushButton_c1->setStyleSheet( "background-color: rgba(" + this->custom_button_color_c1 + ", " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); color: rgb(255,255,255);");
    ui_->pushButton_c2->setStyleSheet( "background-color: rgba(" + this->custom_button_color_c2 + ", " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); color: rgb(255,255,255);");
    ui_->pushButton_c3->setStyleSheet( "background-color: rgba(" + this->custom_button_color_c3 + ", " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); color: rgb(255,255,255);");
    ui_->pushButton_c4->setStyleSheet( "background-color: rgba(" + this->custom_button_color_c4 + ", " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); color: rgb(255,255,255);");
    ui_->pushButton_c5->setStyleSheet( "background-color: rgba(" + this->custom_button_color_c5 + ", " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); color: rgb(255,255,255);");
    ui_->pushButton_c6->setStyleSheet( "background-color: rgba(" + this->custom_button_color_c6 + ", " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); color: rgb(255,255,255);");
    ui_->pushButton_c7->setStyleSheet( "background-color: rgba(" + this->custom_button_color_c7 + ", " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); color: rgb(255,255,255);");
    ui_->pushButtonDummy1->setStyleSheet( "background-color: rgba(186, 189, 182, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
    ui_->pushButtonDummy2->setStyleSheet( "background-color: rgba(186, 189, 182, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
    ui_->pushButtonDummy3->setStyleSheet( "background-color: rgba(186, 189, 182, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
    ui_->pushButtonDebug->setStyleSheet( "background-color: rgba(85, 87, 83, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5);");
    ui_->pushButtonMusic->setStyleSheet( "background-color: rgba(78, 154, 6, " + alp + " ); border-radius: 4px; border: 2px solid rgba(255,255,255,0.5); color: rgb(255,255,255);");
}

void f1x::openauto::autoapp::ui::MainWindow::switchGuiToNight()
{
    MainWindow::on_pushButtonVolume_clicked();
    if (this->oldGUIStyle) {
        if (this->wallpaperClassicNightFileExists) {
            this->setStyleSheet("QMainWindow { background: url(wallpaper-classic-night.png); background-repeat: no-repeat; background-position: center; }");
        } else {
            this->setStyleSheet("QMainWindow { background: url(:/black.png); background-repeat: no-repeat; background-position: center; }");
        }
    } else {
        if (this->wallpaperNightFileExists) {
            this->setStyleSheet("QMainWindow { background: url(wallpaper-night.png); background-repeat: no-repeat; background-position: center; }");
        } else {
            this->setStyleSheet("QMainWindow { background: url(:/black.png); background-repeat: no-repeat; background-position: center; }");
        }
    }
    ui_->pushButtonDay->show();
    ui_->pushButtonDay2->show();
    ui_->pushButtonNight->hide();
    ui_->pushButtonNight2->hide();
}

void f1x::openauto::autoapp::ui::MainWindow::switchGuiToDay()
{
    MainWindow::on_pushButtonVolume_clicked();
    if (this->oldGUIStyle) {
        if (this->wallpaperClassicDayFileExists) {
            this->setStyleSheet("QMainWindow { background: url(wallpaper-classic.png); background-repeat: no-repeat; background-position: center; }");
        } else {
            this->setStyleSheet("QMainWindow { background: url(:/black.png); background-repeat: no-repeat; background-position: center; }");
        }
    } else {
        if (this->wallpaperDayFileExists) {
            this->setStyleSheet("QMainWindow { background: url(wallpaper.png); background-repeat: no-repeat; background-position: center; }");
        } else {
            this->setStyleSheet("QMainWindow { background: url(:/black.png); background-repeat: no-repeat; background-position: center; }");
        }
    }
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
    if (!this->oldGUIStyle) {
        ui_->menuWidget->hide();
    } else {
        ui_->oldmenuWidget->hide();
    }
    ui_->mediaWidget->show();
    ui_->VolumeSliderControlPlayer->show();
    ui_->VolumeSliderControl->hide();
    ui_->BrightnessSliderControl->hide();
    ui_->AlphaSliderControl->hide();
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
    ui_->AlphaSliderControl->hide();
}

void f1x::openauto::autoapp::ui::MainWindow::toggleExit()
{
    if (!this->exitMenuVisible) {
        ui_->pushButtonExit->hide();
        ui_->pushButtonShutdown->show();
        ui_->pushButtonReboot->show();
        ui_->pushButtonCancel->show();
        ui_->exitWidget->show();
        ui_->buttonWidget->hide();
        this->exitMenuVisible = true;
    } else {
        ui_->pushButtonShutdown->hide();
        ui_->pushButtonReboot->hide();
        ui_->pushButtonCancel->hide();
        ui_->pushButtonExit->show();
        ui_->buttonWidget->show();
        ui_->exitWidget->hide();
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
    if (!this->nightModeEnabled) {
        if (this->oldGUIStyle) {
            if (this->wallpaperClassicDayFileExists) {
                this->setStyleSheet( this->styleSheet().append("QMainWindow { background: url(wallpaper-classic.png); background-repeat: no-repeat; background-position: center; }") );
            } else {
                this->setStyleSheet("QMainWindow { background: url(:/black.png); background-repeat: no-repeat; background-position: center; }");
            }
        } else {
            if (this->wallpaperDayFileExists) {
                this->setStyleSheet( this->styleSheet().append("QMainWindow { background: url(wallpaper.png); background-repeat: no-repeat; background-position: center; }") );
            } else {
                this->setStyleSheet("QMainWindow { background: url(:/black.png); background-repeat: no-repeat; background-position: center; }");
            }
        }
    } else {
        if (this->oldGUIStyle) {
            if (this->wallpaperClassicNightFileExists) {
                this->setStyleSheet( this->styleSheet().append("QMainWindow { background: url(wallpaper-classic-night.png); background-repeat: no-repeat; background-position: center; }") );
            } else {
                this->setStyleSheet("QMainWindow { background: url(:/black.png); background-repeat: no-repeat; background-position: center; }");
            }
        } else {
            if (this->wallpaperNightFileExists) {
                this->setStyleSheet( this->styleSheet().append("QMainWindow { background: url(wallpaper-night.png); background-repeat: no-repeat; background-position: center; }") );
            } else {
                this->setStyleSheet("QMainWindow { background: url(:/black.png); background-repeat: no-repeat; background-position: center; }");
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
    QString time_text=time.toString("hh : mm : ss");
    if ((time.second() % 2) == 0) {
        time_text[3] = ' ';
        time_text[8] = ' ';

        // check if system is in display off mode (tap2wake)
        QFileInfo blankFile("/tmp/blankscreen");
        if (blankFile.exists()) {
            if (ui_->centralWidget->isVisible() == true) {
                ui_->centralWidget->hide();
            }
        } else {
            if (ui_->centralWidget->isVisible() == false) {
                ui_->centralWidget->show();
            }
        }

        // check if custom command needs black background
        QFileInfo blackFile("/tmp/blackscreen");
        if (blackFile.exists()) {
            if (ui_->centralWidget->isVisible() == true) {
                ui_->centralWidget->hide();
                this->setStyleSheet("QMainWindow {background-color: rgb(0,0,0);}");
            }
        } else {
            if (!this->nightModeEnabled) {
                if (this->oldGUIStyle) {
                    if (this->wallpaperClassicDayFileExists) {
                        this->setStyleSheet("QMainWindow { background: url(wallpaper-classic.png); background-repeat: no-repeat; background-position: center; }");
                    } else {
                        this->setStyleSheet("QMainWindow { background: url(:/black.png); background-repeat: no-repeat; background-position: center; }");
                    }
                } else {
                    if (this->wallpaperDayFileExists) {
                        this->setStyleSheet("QMainWindow { background: url(wallpaper.png); background-repeat: no-repeat; background-position: center; }");
                    } else {
                        this->setStyleSheet("QMainWindow { background: url(:/black.png); background-repeat: no-repeat; background-position: center; }");
                    }
                }
            } else {
                if (this->oldGUIStyle) {
                    if (this->wallpaperClassicNightFileExists) {
                        this->setStyleSheet("QMainWindow { background: url(wallpaper-classic-night.png); background-repeat: no-repeat; background-position: center; }");
                    } else {
                        this->setStyleSheet("QMainWindow { background: url(:/black.png); background-repeat: no-repeat; background-position: center; }");
                    }
                } else {
                    if (this->wallpaperNightFileExists) {
                        this->setStyleSheet("QMainWindow { background: url(wallpaper-night.png); background-repeat: no-repeat; background-position: center; }");
                    } else {
                        this->setStyleSheet("QMainWindow { background: url(:/black.png); background-repeat: no-repeat; background-position: center; }");
                    }
                }
            }
        }

        // check if phone is conencted to usb
        QFileInfo phoneConnectedFile("/tmp/android_device");
        if (phoneConnectedFile.exists()) {
            if (ui_->phoneConnected->isVisible() == false) {
                ui_->phoneConnected->setText("USB connected");
                ui_->phoneConnected->show();
            }
        } else {
            if (ui_->phoneConnected->isVisible() == true) {
                ui_->phoneConnected->hide();
            }
        }

        // check if bluetooth available
        QFileInfo bluetoothButtonFile("/tmp/button_bluetooth_visible");
        this->bluetoothEnabled = bluetoothButtonFile.exists();

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
        QFileInfo phoneBTConnectedFile("/tmp/btdevice");
        if (phoneBTConnectedFile.exists()) {
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

        // check the need for system messages
        QFileInfo configInProgressFile("/tmp/config_in_progress");
        QFileInfo debugInProgressFile("/tmp/debug_in_progress");
        QFileInfo enablePairingFile("/tmp/enable_pairing");

        if (configInProgressFile.exists() || debugInProgressFile.exists() || enablePairingFile.exists()) {
            if (ui_->systemConfigInProgress->isVisible() == false) {
                if (configInProgressFile.exists()) {
                    ui_->systemConfigInProgress->setText("System config in progress - please wait ...");
                    ui_->pushButtonSettings->hide();
                    ui_->pushButtonSettings2->hide();
                    ui_->pushButtonLock->show();
                    ui_->pushButtonLock2->show();
                    ui_->systemConfigInProgress->show();
                }
                if (debugInProgressFile.exists()) {
                    ui_->systemConfigInProgress->setText("Creating debug.zip on /boot - please wait ...");
                    ui_->pushButtonSettings->hide();
                    ui_->pushButtonSettings2->hide();
                    ui_->pushButtonDebug->hide();
                    ui_->pushButtonDebug2->hide();
                    ui_->pushButtonLock->show();
                    ui_->pushButtonLock2->show();
                    ui_->systemConfigInProgress->show();
                }
                if (enablePairingFile.exists()) {
                    ui_->systemConfigInProgress->setText("Auto Bluetooth Pairing enabled for 120 seconds!");
                    ui_->pushButtonDebug->hide();
                    ui_->pushButtonDebug2->hide();
                    ui_->systemConfigInProgress->show();
                }
            }
        } else {
            if (ui_->systemConfigInProgress->isVisible() == true) {
                ui_->systemConfigInProgress->hide();
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

        // camera stuff
        if (this->cameraButtonForce) {

            // check if dashcam is recording
            QFileInfo dashCamRecordingFile("/tmp/dashcam_is_recording");
            this->dashCamRecording = dashCamRecordingFile.exists();

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
            QFileInfo rearCamFile("/tmp/rearcam_enabled");
            this->rearCamEnabled = rearCamFile.exists();

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

        // check if sutdown is external triggered and init clean app exit
        QFileInfo externalExitFile("/tmp/external_exit");
        if (externalExitFile.exists()) {
            f1x::openauto::autoapp::ui::MainWindow::MainWindow::exit();
        }

        QFileInfo hotspotFile("/tmp/hotspot_active");
        this->hotspotActive = hotspotFile.exists();

        // hide wifi if not forced
        if (!this->hotspotActive) {
            if ((ui_->pushButtonWifi->isVisible() == true) || (ui_->pushButtonWifi2->isVisible() == true)){
                ui_->pushButtonWifi->hide();
                ui_->pushButtonWifi2->hide();
                if (!this->cameraButtonForce) {
                    ui_->pushButtonDummyCamWifi->show();
                }
            }
        } else {
            if ((ui_->pushButtonWifi->isVisible() == false) || (ui_->pushButtonWifi2->isVisible() == false)) {
                ui_->pushButtonWifi->show();
                ui_->pushButtonWifi2->show();
                ui_->pushButtonDummyCamWifi->hide();
            }
        }
    }

    // handle dummys in classic menu
    int button_count = 0;
    if (ui_->pushButtonCameraShow2->isVisible() == true) {
        button_count = button_count + 1;
    }
    if (ui_->pushButtonToggleGUI2->isVisible() == true) {
        button_count = button_count + 1;
    }
    if (ui_->pushButtonWifi2->isVisible() == true) {
        button_count = button_count + 1;
    }
    if (ui_->pushButtonDebug2->isVisible() == true) {
        button_count = button_count + 1;
    }
    if (button_count >= 3) {
        ui_->pushButtonDummyClassic1->hide();
        ui_->pushButtonDummyClassic2->hide();
    }
    if (button_count == 2) {
        ui_->pushButtonDummyClassic1->hide();
        ui_->pushButtonDummyClassic2->hide();
    }
    if (button_count == 1) {
        ui_->pushButtonDummyClassic1->show();
        ui_->pushButtonDummyClassic2->hide();
    }
    if (button_count == 0) {
        ui_->pushButtonDummyClassic1->show();
        ui_->pushButtonDummyClassic2->show();
    }

    ui_->Digital_clock->setText(time_text);
    ui_->bigClock->setText(time_text);
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
    ui_->pushButtonUSB->show();
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
    ui_->pushButtonUSB->show();
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
    ui_->pushButtonUSB->hide();
}

void f1x::openauto::autoapp::ui::MainWindow::on_comboBoxAlbum_currentIndexChanged(const QString &arg1)
{
    this->albumfolder = arg1;
    f1x::openauto::autoapp::ui::MainWindow::scanFiles();
}

void f1x::openauto::autoapp::ui::MainWindow::scanFolders()
{
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
}

void f1x::openauto::autoapp::ui::MainWindow::scanFiles()
{
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
    ui_->pushButtonUSB->hide();
}
