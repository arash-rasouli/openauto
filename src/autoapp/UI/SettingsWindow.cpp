/*subfolder
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

#include <QMessageBox>
#include <f1x/openauto/autoapp/UI/SettingsWindow.hpp>
#include "ui_settingswindow.h"
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <string>
#include <QTimer>
#include <QDateTime>
#include <QNetworkInterface>
#include <fstream>
#include <QStorageInfo>
#include <QProcess>

namespace f1x
{
namespace openauto
{
namespace autoapp
{
namespace ui
{

SettingsWindow::SettingsWindow(configuration::IConfiguration::Pointer configuration, QWidget *parent)
    : QWidget(parent)
    , ui_(new Ui::SettingsWindow)
    , configuration_(std::move(configuration))
{
    ui_->setupUi(this);
    connect(ui_->pushButtonCancel, &QPushButton::clicked, this, &SettingsWindow::close);
    connect(ui_->pushButtonSave, &QPushButton::clicked, this, &SettingsWindow::onSave);
    connect(ui_->pushButtonUnpair , &QPushButton::clicked, this, &SettingsWindow::unpairAll);
    connect(ui_->pushButtonUnpair , &QPushButton::clicked, this, &SettingsWindow::close);
    connect(ui_->horizontalSliderScreenDPI, &QSlider::valueChanged, this, &SettingsWindow::onUpdateScreenDPI);
    connect(ui_->horizontalSliderAlphaTrans, &QSlider::valueChanged, this, &SettingsWindow::onUpdateAlphaTrans);
    connect(ui_->horizontalSliderDay, &QSlider::valueChanged, this, &SettingsWindow::onUpdateBrightnessDay);
    connect(ui_->horizontalSliderNight, &QSlider::valueChanged, this, &SettingsWindow::onUpdateBrightnessNight);
    connect(ui_->horizontalSliderBrightness1, &QSlider::valueChanged, this, &SettingsWindow::onUpdateBrightness1);
    connect(ui_->horizontalSliderBrightness2, &QSlider::valueChanged, this, &SettingsWindow::onUpdateBrightness2);
    connect(ui_->horizontalSliderBrightness3, &QSlider::valueChanged, this, &SettingsWindow::onUpdateBrightness3);
    connect(ui_->horizontalSliderBrightness4, &QSlider::valueChanged, this, &SettingsWindow::onUpdateBrightness4);
    connect(ui_->horizontalSliderBrightness5, &QSlider::valueChanged, this, &SettingsWindow::onUpdateBrightness5);
    connect(ui_->horizontalSliderLux1, &QSlider::valueChanged, this, &SettingsWindow::onUpdateLux1);
    connect(ui_->horizontalSliderLux2, &QSlider::valueChanged, this, &SettingsWindow::onUpdateLux2);
    connect(ui_->horizontalSliderLux3, &QSlider::valueChanged, this, &SettingsWindow::onUpdateLux3);
    connect(ui_->horizontalSliderLux4, &QSlider::valueChanged, this, &SettingsWindow::onUpdateLux4);
    connect(ui_->horizontalSliderLux5, &QSlider::valueChanged, this, &SettingsWindow::onUpdateLux5);

    connect(ui_->pushButtonClearSelection, &QPushButton::clicked, std::bind(&SettingsWindow::setButtonCheckBoxes, this, false));
    connect(ui_->pushButtonSelectAll, &QPushButton::clicked, std::bind(&SettingsWindow::setButtonCheckBoxes, this, true));
    connect(ui_->pushButtonResetToDefaults, &QPushButton::clicked, this, &SettingsWindow::onResetToDefaults);
    connect(ui_->horizontalSliderSystemVolume, &QSlider::valueChanged, this, &SettingsWindow::onUpdateSystemVolume);
    connect(ui_->horizontalSliderSystemCapture, &QSlider::valueChanged, this, &SettingsWindow::onUpdateSystemCapture);
    connect(ui_->radioButtonHotspot, &QPushButton::clicked, this, &SettingsWindow::onStartHotspot);
    connect(ui_->radioButtonClient, &QPushButton::clicked, this, &SettingsWindow::onStopHotspot);
    connect(ui_->pushButtonSetTime, &QPushButton::clicked, this, &SettingsWindow::setTime);
    connect(ui_->pushButtonSetTime, &QPushButton::clicked, this, &SettingsWindow::close);
    connect(ui_->pushButtonNTP, &QPushButton::clicked, [&]() { system("/usr/local/bin/crankshaft rtc sync &"); });
    connect(ui_->pushButtonNTP, &QPushButton::clicked, this, &SettingsWindow::close);
    connect(ui_->pushButtonCheckNow, &QPushButton::clicked, [&]() { system("/usr/local/bin/crankshaft update check &"); });
    connect(ui_->pushButtonDebuglog, &QPushButton::clicked, this, &SettingsWindow::close);
    connect(ui_->pushButtonDebuglog, &QPushButton::clicked, [&]() { system("/usr/local/bin/crankshaft debuglog &");});
    connect(ui_->pushButtonNetworkAuto, &QPushButton::clicked, [&]() { system("/usr/local/bin/crankshaft network auto &");});
    connect(ui_->pushButtonNetwork0, &QPushButton::clicked, this, &SettingsWindow::on_pushButtonNetwork0_clicked);
    connect(ui_->pushButtonNetwork1, &QPushButton::clicked, this, &SettingsWindow::on_pushButtonNetwork1_clicked);
    connect(ui_->pushButtonSambaStart, &QPushButton::clicked, [&]() { system("/usr/local/bin/crankshaft samba start &");});
    connect(ui_->pushButtonSambaStop, &QPushButton::clicked, [&]() { system("/usr/local/bin/crankshaft samba stop &");});

    // menu
    ui_->tab1->show();
    ui_->tab2->hide();
    ui_->tab3->hide();
    ui_->tab4->hide();
    ui_->tab5->hide();
    ui_->tab6->hide();
    ui_->tab7->hide();
    ui_->tab8->hide();
    ui_->tab9->hide();

    ui_->horizontalGroupBox->hide();
    ui_->labelTestInProgress->hide();

    connect(ui_->pushButtonTab1, &QPushButton::clicked, this, &SettingsWindow::show_tab1);
    connect(ui_->pushButtonTab2, &QPushButton::clicked, this, &SettingsWindow::show_tab2);
    connect(ui_->pushButtonTab3, &QPushButton::clicked, this, &SettingsWindow::show_tab3);
    connect(ui_->pushButtonTab4, &QPushButton::clicked, this, &SettingsWindow::show_tab4);
    connect(ui_->pushButtonTab5, &QPushButton::clicked, this, &SettingsWindow::show_tab5);
    connect(ui_->pushButtonTab5, &QPushButton::clicked, this, &SettingsWindow::updateNetworkInfo);
    connect(ui_->pushButtonTab6, &QPushButton::clicked, this, &SettingsWindow::show_tab6);
    connect(ui_->pushButtonTab6, &QPushButton::clicked, this, &SettingsWindow::updateSystemInfo);
    connect(ui_->pushButtonTab7, &QPushButton::clicked, this, &SettingsWindow::show_tab7);
    connect(ui_->pushButtonTab8, &QPushButton::clicked, this, &SettingsWindow::show_tab8);
    connect(ui_->pushButtonTab9, &QPushButton::clicked, this, &SettingsWindow::show_tab9);

    QTime time=QTime::currentTime();
    QString time_text_hour=time.toString("hh");
    QString time_text_minute=time.toString("mm");
    ui_->spinBoxHour->setValue((time_text_hour).toInt());
    ui_->spinBoxMinute->setValue((time_text_minute).toInt());
    ui_->label_modeswitchprogress->setText("Ok");
    ui_->label_notavailable->hide();

    QString wifi_ssid = configuration_->getCSValue("WIFI_SSID");
    QString wifi2_ssid = configuration_->getCSValue("WIFI2_SSID");

    ui_->pushButtonNetwork0->setText(wifi_ssid);
    ui_->pushButtonNetwork1->setText(wifi2_ssid);

    if (!std::ifstream("/boot/crankshaft/network1.conf")) {
        ui_->pushButtonNetwork1->hide();
        ui_->pushButtonNetwork0->show();
    }
    if (!std::ifstream("/boot/crankshaft/network0.conf")) {
        ui_->pushButtonNetwork1->hide();
        ui_->pushButtonNetwork0->setText(configuration_->getCSValue("WIFI2_SSID"));
    }
    if (!std::ifstream("/boot/crankshaft/network0.conf") && !std::ifstream("/boot/crankshaft/network1.conf")) {
        ui_->pushButtonNetwork0->hide();
        ui_->pushButtonNetwork1->hide();
        ui_->pushButtonNetworkAuto->hide();
        ui_->label_notavailable->show();
    }

    if (std::ifstream("/tmp/hotspot_active")) {
        ui_->radioButtonClient->setChecked(0);
        ui_->radioButtonHotspot->setChecked(1);
        ui_->lineEditWifiSSID->setText(configuration_->getParamFromFile("/etc/hostapd/hostapd.conf","ssid"));
        ui_->lineEditPassword->show();
        ui_->label_password->show();
        ui_->lineEditPassword->setText("1234567890");
        ui_->clientNetworkSelect->hide();
        ui_->label_notavailable->show();
    } else {
        ui_->radioButtonClient->setChecked(1);
        ui_->radioButtonHotspot->setChecked(0);
        ui_->lineEditWifiSSID->setText(configuration_->readFileContent("/tmp/wifi_ssid"));
        ui_->lineEditPassword->hide();
        ui_->label_password->hide();
        ui_->lineEditPassword->setText("");
        ui_->clientNetworkSelect->hide();
        ui_->label_notavailable->show();
    }

    if (std::ifstream("/tmp/samba_running")) {
        ui_->labelSambaStatus->setText("running");
        ui_->pushButtonSambaStart->hide();
        ui_->pushButtonSambaStop->show();
    } else {
        ui_->labelSambaStatus->setText("stopped");
        ui_->pushButtonSambaStop->hide();
        ui_->pushButtonSambaStart->show();
    }

    QTimer *refresh=new QTimer(this);
    connect(refresh, SIGNAL(timeout()),this,SLOT(updateInfo()));
    refresh->start(5000);
}

SettingsWindow::~SettingsWindow()
{
    delete ui_;
}

void SettingsWindow::updateInfo()
{
    if (ui_->tab6->isVisible() == true) {
        updateSystemInfo();
    }
    if (ui_->tab5->isVisible() == true) {
        updateNetworkInfo();
    }
}

void SettingsWindow::onSave()
{
    configuration_->setHandednessOfTrafficType(ui_->radioButtonLeftHandDrive->isChecked() ? configuration::HandednessOfTrafficType::LEFT_HAND_DRIVE : configuration::HandednessOfTrafficType::RIGHT_HAND_DRIVE);

    configuration_->showClock(ui_->checkBoxShowClock->isChecked());
    configuration_->showBigClock(ui_->checkBoxShowBigClock->isChecked());
    configuration_->oldGUI(ui_->checkBoxOldGUI->isChecked());
    configuration_->setAlphaTrans(static_cast<size_t>(ui_->horizontalSliderAlphaTrans->value()));
    configuration_->hideMenuToggle(ui_->checkBoxHideMenuToggle->isChecked());
    configuration_->showLux(ui_->checkBoxShowLux->isChecked());
    configuration_->showCursor(ui_->checkBoxShowCursor->isChecked());
    configuration_->hideBrightnessControl(ui_->checkBoxHideBrightnessControl->isChecked());
    configuration_->showNetworkinfo(ui_->checkBoxNetworkinfo->isChecked());
    configuration_->mp3AutoPlay(ui_->checkBoxAutoPlay->isChecked());
    configuration_->showAutoPlay(ui_->checkBoxShowPlayer->isChecked());
    configuration_->instantPlay(ui_->checkBoxInstantPlay->isChecked());
    configuration_->hideWarning(ui_->checkBoxDontShowAgain->isChecked());

    configuration_->setVideoFPS(ui_->radioButton30FPS->isChecked() ? aasdk::proto::enums::VideoFPS::_30 : aasdk::proto::enums::VideoFPS::_60);

    if(ui_->radioButton480p->isChecked())
    {
        configuration_->setVideoResolution(aasdk::proto::enums::VideoResolution::_480p);
    }
    // else if(ui_->radioButton720p->isChecked())
    // {
    //     // configuration_->setVideoResolution(aasdk::proto::enums::VideoResolution::_720p);
    // }
    // // else if(ui_->radioButton1080p->isChecked())
    // {
    //     // configuration_->setVideoResolution(aasdk::proto::enums::VideoResolution::_1080p);
    // }

    configuration_->setScreenDPI(static_cast<size_t>(ui_->horizontalSliderScreenDPI->value()));
    configuration_->setOMXLayerIndex(ui_->spinBoxOmxLayerIndex->value());

    QRect videoMargins(0, 0, ui_->spinBoxVideoMarginWidth->value(), ui_->spinBoxVideoMarginHeight->value());
    configuration_->setVideoMargins(std::move(videoMargins));

    configuration_->setTouchscreenEnabled(ui_->checkBoxEnableTouchscreen->isChecked());
    this->saveButtonCheckBoxes();

    configuration_->playerButtonControl(ui_->checkBoxPlayerControl->isChecked());



    configuration_->setMusicAudioChannelEnabled(ui_->checkBoxMusicAudioChannel->isChecked());
    configuration_->setSpeechAudioChannelEnabled(ui_->checkBoxSpeechAudioChannel->isChecked());
    configuration_->setAudioOutputBackendType(ui_->radioButtonRtAudio->isChecked() ? configuration::AudioOutputBackendType::RTAUDIO : configuration::AudioOutputBackendType::QT);

    configuration_->save();

    // generate param string for autoapp_helper
    std::string params;
    params.append( std::to_string(ui_->horizontalSliderSystemVolume->value()) );
    params.append("#");
    params.append( std::to_string(ui_->horizontalSliderSystemCapture->value()) );
    params.append("#");
    params.append( std::to_string(ui_->spinBoxDisconnect->value()) );
    params.append("#");
    params.append( std::to_string(ui_->spinBoxShutdown->value()) );
    params.append("#");
    params.append( std::to_string(ui_->spinBoxDay->value()) );
    params.append("#");
    params.append( std::to_string(ui_->spinBoxNight->value()) );
    params.append("#");
    if (ui_->checkBoxGPIO->isChecked()) {
        params.append("1");
    } else {
        params.append("0");
    }
    params.append("#");
    params.append( std::string(ui_->comboBoxDevMode->currentText().toStdString()) );
    params.append("#");
    params.append( std::string(ui_->comboBoxInvert->currentText().toStdString()) );
    params.append("#");
    params.append( std::string(ui_->comboBoxX11->currentText().toStdString()) );
    params.append("#");
    params.append( std::string(ui_->comboBoxRearcam->currentText().toStdString()) );
    params.append("#");
    params.append( std::string(ui_->comboBoxAndroid->currentText().toStdString()) );
    params.append("#");
    if (ui_->radioButtonX11->isChecked()) {
        params.append("1");
    } else {
        params.append("0");
    }
    params.append("#");
    if (ui_->radioButtonScreenRotated->isChecked()) {
        params.append("1");
    } else {
        params.append("0");
    }
    params.append("#");
    params.append( std::string("'") + std::string(ui_->comboBoxPulseOutput->currentText().toStdString()) + std::string("'") );
    params.append("#");
    params.append( std::string("'") + std::string(ui_->comboBoxPulseInput->currentText().toStdString()) + std::string("'") );
    params.append("#");
    params.append( std::string(ui_->comboBoxHardwareRTC->currentText().toStdString()) );
    params.append("#");
    params.append( std::string(ui_->comboBoxTZ->currentText().toStdString()) );
    params.append("#");
    params.append( std::string(ui_->comboBoxHardwareDAC->currentText().toStdString()) );
    params.append("#");
    if (ui_->checkBoxDisableShutdown->isChecked()) {
        params.append("1");
    } else {
        params.append("0");
    }
    params.append("#");
    if (ui_->checkBoxDisableScreenOff->isChecked()) {
        params.append("1");
    } else {
        params.append("0");
    }
    params.append("#");
    if (ui_->radioButtonDebugmodeEnabled->isChecked()) {
        params.append("1");
    } else {
        params.append("0");
    }
    params.append("#");
    params.append( std::string(ui_->comboBoxGPIOShutdown->currentText().toStdString()) );
    params.append("#");
    params.append( std::to_string(ui_->spinBoxGPIOShutdownDelay->value()) );
    params.append("#");
    if (ui_->checkBoxHotspot->isChecked()) {
        params.append("1");
    } else {
        params.append("0");
    }
    params.append("#");
    params.append( std::string(ui_->comboBoxCam->currentText().toStdString()) );
    params.append("#");
    params.append("#");
    params.append("#");
    if (ui_->checkBoxHardwareSave->isChecked()) {
        params.append("1");
    } else {
        params.append("0");
    }
    params.append("#");
    params.append( std::string(ui_->comboBoxUSBCam->currentText().toStdString()) );
    params.append("#");
    params.append( std::string(ui_->comboBoxLS->currentText().split(" ")[0].toStdString()) );
    params.append("#");
    params.append( std::string(ui_->comboBoxDayNight->currentText().toStdString()) );
    params.append("#");
    params.append( std::to_string(ui_->horizontalSliderDay->value()) );
    params.append("#");
    params.append( std::to_string(ui_->horizontalSliderNight->value()) );
    params.append("#");
    params.append( std::to_string(ui_->horizontalSliderLux1->value()) );
    params.append("#");
    params.append( std::to_string(ui_->horizontalSliderBrightness1->value()) );
    params.append("#");
    params.append( std::to_string(ui_->horizontalSliderLux2->value()) );
    params.append("#");
    params.append( std::to_string(ui_->horizontalSliderBrightness2->value()) );
    params.append("#");
    params.append( std::to_string(ui_->horizontalSliderLux3->value()) );
    params.append("#");
    params.append( std::to_string(ui_->horizontalSliderBrightness3->value()) );
    params.append("#");
    params.append( std::to_string(ui_->horizontalSliderLux4->value()) );
    params.append("#");
    params.append( std::to_string(ui_->horizontalSliderBrightness4->value()) );
    params.append("#");
    params.append( std::to_string(ui_->horizontalSliderLux5->value()) );
    params.append("#");
    params.append( std::to_string(ui_->horizontalSliderBrightness5->value()) );
    params.append("#");
    params.append( std::string(ui_->comboBoxCheckInterval->currentText().toStdString()) );
    params.append("#");
    params.append( std::string(ui_->comboBoxNightmodeStep->currentText().toStdString()) );
    params.append("#");
    if (ui_->checkBoxDisableDayNightRTC->isChecked()) {
        params.append("0");
    } else {
        params.append("1");
    }
    params.append("#");
    if(ui_->radioButtonAnimatedCSNG->isChecked())
    {
        params.append("0");
    }
    else if(ui_->radioButtonCSNG->isChecked())
    {
        params.append("1");
    }
    else if(ui_->radioButtonCustom->isChecked())
    {
        params.append("2");
    }
    params.append("#");
    params.append( std::string(ui_->comboBoxCountryCode->currentText().split("|")[0].replace(" ","").toStdString()) );
    params.append("#");
    if (ui_->checkBoxBlankOnly ->isChecked()) {
        params.append("1");
    } else {
        params.append("0");
    }
    params.append("#");
    if (ui_->checkBoxFlipX ->isChecked()) {
        params.append("1");
    } else {
        params.append("0");
    }
    params.append("#");
    if (ui_->checkBoxFlipY ->isChecked()) {
        params.append("1");
    } else {
        params.append("0");
    }
    params.append("#");
    params.append( std::string(ui_->comboBoxRotation->currentText().toStdString()) );
    params.append("#");
    params.append( std::string(ui_->comboBoxResolution->currentText().toStdString()) );
    params.append("#");
    params.append( std::string((ui_->comboBoxFPS->currentText()).replace(" (not @1080)","").toStdString()) );
    params.append("#");
    params.append( std::string(ui_->comboBoxAWB->currentText().toStdString()) );
    params.append("#");
    params.append( std::string(ui_->comboBoxEXP->currentText().toStdString()) );
    params.append("#");
    params.append( std::string(ui_->comboBoxLoopTime->currentText().toStdString()) );
    params.append("#");
    params.append( std::string(ui_->comboBoxLoopCount->currentText().toStdString()) );
    params.append("#");
    if (ui_->checkBoxAutoRecording ->isChecked()) {
        params.append("1");
    } else {
        params.append("0");
    }
    params.append("#");
    if (ui_->checkBoxFlipXUSB ->isChecked()) {
        params.append("1");
    } else {
        params.append("0");
    }
    params.append("#");
    if (ui_->checkBoxFlipYUSB ->isChecked()) {
        params.append("1");
    } else {
        params.append("0");
    }
    params.append("#");
    params.append( std::string(ui_->comboBoxUSBRotation->currentText().replace("180","1").toStdString()) );
    params.append("#");
    system((std::string("/usr/local/bin/autoapp_helper setparams#") + std::string(params) + std::string(" &") ).c_str());

    this->close();
}

void SettingsWindow::onResetToDefaults()
{
    QMessageBox confirmationMessage(QMessageBox::Question, "Confirmation", "Are you sure you want to reset settings?", QMessageBox::Yes | QMessageBox::Cancel);
    //confirmationMessage.setWindowFlags(Qt::WindowStaysOnTopHint);
    if(confirmationMessage.exec() == QMessageBox::Yes)
    {
        configuration_->reset();
        this->load();
    }
}

void SettingsWindow::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    this->load();
}

void SettingsWindow::load()
{
    ui_->radioButtonLeftHandDrive->setChecked(configuration_->getHandednessOfTrafficType() == configuration::HandednessOfTrafficType::LEFT_HAND_DRIVE);
    ui_->radioButtonRightHandDrive->setChecked(configuration_->getHandednessOfTrafficType() == configuration::HandednessOfTrafficType::RIGHT_HAND_DRIVE);
    ui_->checkBoxShowClock->setChecked(configuration_->showClock());
    ui_->horizontalSliderAlphaTrans->setValue(static_cast<int>(configuration_->getAlphaTrans()));

    ui_->checkBoxShowBigClock->setChecked(configuration_->showBigClock());
    ui_->checkBoxOldGUI->setChecked(configuration_->oldGUI());
    ui_->checkBoxHideMenuToggle->setChecked(configuration_->hideMenuToggle());
    ui_->checkBoxShowLux->setChecked(configuration_->showLux());
    ui_->checkBoxShowCursor->setChecked(configuration_->showCursor());
    ui_->checkBoxHideBrightnessControl->setChecked(configuration_->hideBrightnessControl());
    ui_->checkBoxNetworkinfo->setChecked(configuration_->showNetworkinfo());
    ui_->checkBoxAutoPlay->setChecked(configuration_->mp3AutoPlay());
    ui_->checkBoxShowPlayer->setChecked(configuration_->showAutoPlay());
    ui_->checkBoxInstantPlay->setChecked(configuration_->instantPlay());
    ui_->checkBoxDontShowAgain->setChecked(configuration_->hideWarning());

    ui_->radioButton30FPS->setChecked(configuration_->getVideoFPS() == aasdk::proto::enums::VideoFPS::_30);
    ui_->radioButton60FPS->setChecked(configuration_->getVideoFPS() == aasdk::proto::enums::VideoFPS::_60);

    ui_->radioButton480p->setChecked(configuration_->getVideoResolution() == aasdk::proto::enums::VideoResolution::_480p);
    // ui_->radioButton720p->setChecked(configuration_->getVideoResolution() == aasdk::proto::enums::VideoResolution::_720p);
    // ui_->radioButton1080p->setChecked(configuration_->getVideoResolution() == aasdk::proto::enums::VideoResolution::_1080p);
    ui_->horizontalSliderScreenDPI->setValue(static_cast<int>(configuration_->getScreenDPI()));
    ui_->spinBoxOmxLayerIndex->setValue(configuration_->getOMXLayerIndex());

    const auto& videoMargins = configuration_->getVideoMargins();
    ui_->spinBoxVideoMarginWidth->setValue(videoMargins.width());
    ui_->spinBoxVideoMarginHeight->setValue(videoMargins.height());

    ui_->checkBoxEnableTouchscreen->setChecked(configuration_->getTouchscreenEnabled());
    this->loadButtonCheckBoxes();
    ui_->checkBoxPlayerControl->setChecked(configuration_->playerButtonControl());

    


    ui_->checkBoxMusicAudioChannel->setChecked(configuration_->musicAudioChannelEnabled());
    ui_->checkBoxSpeechAudioChannel->setChecked(configuration_->speechAudioChannelEnabled());

    const auto& audioOutputBackendType = configuration_->getAudioOutputBackendType();
    ui_->radioButtonRtAudio->setChecked(audioOutputBackendType == configuration::AudioOutputBackendType::RTAUDIO);
    ui_->radioButtonQtAudio->setChecked(audioOutputBackendType == configuration::AudioOutputBackendType::QT);

    ui_->checkBoxHardwareSave->setChecked(false);
    QStorageInfo storage("/media/USBDRIVES/CSSTORAGE");
    storage.refresh();
    if (storage.isValid() && storage.isReady()) {
        if (storage.isReadOnly()) {
            ui_->labelStorage->setText("Storage is read only!  (" + storage.device() + ") - This can be caused by demaged filesystem on CSSTORAGE. Try a reboot.");
        } else {
            ui_->labelStorage->setText("Device: " + storage.device() + " Label: " + storage.displayName() + " Total: " + QString::number(storage.bytesTotal()/1024/1024/1024) + "GB Free: " + QString::number(storage.bytesFree()/1024/1024/1024) + "GB (" + storage.fileSystemType() + ")");
        }
    } else {
        ui_->labelStorage->setText("Storage is not ready or missing!");
    }
}

void SettingsWindow::loadButtonCheckBoxes()
{
    const auto& buttonCodes = configuration_->getButtonCodes();
    ui_->checkBoxPlayButton->setChecked(std::find(buttonCodes.begin(), buttonCodes.end(), aasdk::proto::enums::ButtonCode::PLAY) != buttonCodes.end());
    ui_->checkBoxPauseButton->setChecked(std::find(buttonCodes.begin(), buttonCodes.end(), aasdk::proto::enums::ButtonCode::PAUSE) != buttonCodes.end());
    ui_->checkBoxTogglePlayButton->setChecked(std::find(buttonCodes.begin(), buttonCodes.end(), aasdk::proto::enums::ButtonCode::TOGGLE_PLAY) != buttonCodes.end());
    ui_->checkBoxNextTrackButton->setChecked(std::find(buttonCodes.begin(), buttonCodes.end(), aasdk::proto::enums::ButtonCode::NEXT) != buttonCodes.end());
    ui_->checkBoxPreviousTrackButton->setChecked(std::find(buttonCodes.begin(), buttonCodes.end(), aasdk::proto::enums::ButtonCode::PREV) != buttonCodes.end());
    ui_->checkBoxHomeButton->setChecked(std::find(buttonCodes.begin(), buttonCodes.end(), aasdk::proto::enums::ButtonCode::HOME) != buttonCodes.end());
    ui_->checkBoxPhoneButton->setChecked(std::find(buttonCodes.begin(), buttonCodes.end(), aasdk::proto::enums::ButtonCode::PHONE) != buttonCodes.end());
    ui_->checkBoxCallEndButton->setChecked(std::find(buttonCodes.begin(), buttonCodes.end(), aasdk::proto::enums::ButtonCode::CALL_END) != buttonCodes.end());
    ui_->checkBoxVoiceCommandButton->setChecked(std::find(buttonCodes.begin(), buttonCodes.end(), aasdk::proto::enums::ButtonCode::MICROPHONE_1) != buttonCodes.end());
    ui_->checkBoxLeftButton->setChecked(std::find(buttonCodes.begin(), buttonCodes.end(), aasdk::proto::enums::ButtonCode::LEFT) != buttonCodes.end());
    ui_->checkBoxRightButton->setChecked(std::find(buttonCodes.begin(), buttonCodes.end(), aasdk::proto::enums::ButtonCode::RIGHT) != buttonCodes.end());
    ui_->checkBoxUpButton->setChecked(std::find(buttonCodes.begin(), buttonCodes.end(), aasdk::proto::enums::ButtonCode::UP) != buttonCodes.end());
    ui_->checkBoxDownButton->setChecked(std::find(buttonCodes.begin(), buttonCodes.end(), aasdk::proto::enums::ButtonCode::DOWN) != buttonCodes.end());
    ui_->checkBoxScrollWheelButton->setChecked(std::find(buttonCodes.begin(), buttonCodes.end(), aasdk::proto::enums::ButtonCode::SCROLL_WHEEL) != buttonCodes.end());
    ui_->checkBoxBackButton->setChecked(std::find(buttonCodes.begin(), buttonCodes.end(), aasdk::proto::enums::ButtonCode::BACK) != buttonCodes.end());
    ui_->checkBoxEnterButton->setChecked(std::find(buttonCodes.begin(), buttonCodes.end(), aasdk::proto::enums::ButtonCode::ENTER) != buttonCodes.end());
    ui_->checkBoxNavButton->setChecked(std::find(buttonCodes.begin(), buttonCodes.end(), aasdk::proto::enums::ButtonCode::NAVIGATION) != buttonCodes.end());
}

void SettingsWindow::setButtonCheckBoxes(bool value)
{
    ui_->checkBoxPlayButton->setChecked(value);
    ui_->checkBoxPauseButton->setChecked(value);
    ui_->checkBoxTogglePlayButton->setChecked(value);
    ui_->checkBoxNextTrackButton->setChecked(value);
    ui_->checkBoxPreviousTrackButton->setChecked(value);
    ui_->checkBoxHomeButton->setChecked(value);
    ui_->checkBoxPhoneButton->setChecked(value);
    ui_->checkBoxCallEndButton->setChecked(value);
    ui_->checkBoxVoiceCommandButton->setChecked(value);
    ui_->checkBoxLeftButton->setChecked(value);
    ui_->checkBoxRightButton->setChecked(value);
    ui_->checkBoxUpButton->setChecked(value);
    ui_->checkBoxDownButton->setChecked(value);
    ui_->checkBoxScrollWheelButton->setChecked(value);
    ui_->checkBoxBackButton->setChecked(value);
    ui_->checkBoxEnterButton->setChecked(value);
    ui_->checkBoxNavButton->setChecked(value);
}

void SettingsWindow::saveButtonCheckBoxes()
{
    configuration::IConfiguration::ButtonCodes buttonCodes;
    this->saveButtonCheckBox(ui_->checkBoxPlayButton, buttonCodes, aasdk::proto::enums::ButtonCode::PLAY);
    this->saveButtonCheckBox(ui_->checkBoxPauseButton, buttonCodes, aasdk::proto::enums::ButtonCode::PAUSE);
    this->saveButtonCheckBox(ui_->checkBoxTogglePlayButton, buttonCodes, aasdk::proto::enums::ButtonCode::TOGGLE_PLAY);
    this->saveButtonCheckBox(ui_->checkBoxNextTrackButton, buttonCodes, aasdk::proto::enums::ButtonCode::NEXT);
    this->saveButtonCheckBox(ui_->checkBoxPreviousTrackButton, buttonCodes, aasdk::proto::enums::ButtonCode::PREV);
    this->saveButtonCheckBox(ui_->checkBoxHomeButton, buttonCodes, aasdk::proto::enums::ButtonCode::HOME);
    this->saveButtonCheckBox(ui_->checkBoxPhoneButton, buttonCodes, aasdk::proto::enums::ButtonCode::PHONE);
    this->saveButtonCheckBox(ui_->checkBoxCallEndButton, buttonCodes, aasdk::proto::enums::ButtonCode::CALL_END);
    this->saveButtonCheckBox(ui_->checkBoxVoiceCommandButton, buttonCodes, aasdk::proto::enums::ButtonCode::MICROPHONE_1);
    this->saveButtonCheckBox(ui_->checkBoxLeftButton, buttonCodes, aasdk::proto::enums::ButtonCode::LEFT);
    this->saveButtonCheckBox(ui_->checkBoxRightButton, buttonCodes, aasdk::proto::enums::ButtonCode::RIGHT);
    this->saveButtonCheckBox(ui_->checkBoxUpButton, buttonCodes, aasdk::proto::enums::ButtonCode::UP);
    this->saveButtonCheckBox(ui_->checkBoxDownButton, buttonCodes, aasdk::proto::enums::ButtonCode::DOWN);
    this->saveButtonCheckBox(ui_->checkBoxScrollWheelButton, buttonCodes, aasdk::proto::enums::ButtonCode::SCROLL_WHEEL);
    this->saveButtonCheckBox(ui_->checkBoxBackButton, buttonCodes, aasdk::proto::enums::ButtonCode::BACK);
    this->saveButtonCheckBox(ui_->checkBoxEnterButton, buttonCodes, aasdk::proto::enums::ButtonCode::ENTER);
    this->saveButtonCheckBox(ui_->checkBoxNavButton, buttonCodes, aasdk::proto::enums::ButtonCode::NAVIGATION);
    configuration_->setButtonCodes(buttonCodes);
}

void SettingsWindow::saveButtonCheckBox(const QCheckBox* checkBox, configuration::IConfiguration::ButtonCodes& buttonCodes, aasdk::proto::enums::ButtonCode::Enum buttonCode)
{
    if(checkBox->isChecked())
    {
        buttonCodes.push_back(buttonCode);
    }
}

void SettingsWindow::onUpdateScreenDPI(int value)
{
    ui_->labelScreenDPIValue->setText(QString::number(value));
}

void SettingsWindow::onUpdateAlphaTrans(int value)
{
    double alpha = value/100.0;
    ui_->labelAlphaTransValue->setText(QString::number(alpha));
}

void SettingsWindow::onUpdateBrightnessDay(int value)
{
    ui_->labelBrightnessDay->setText(QString::number(value));
}

void SettingsWindow::onUpdateBrightnessNight(int value)
{
    ui_->labelBrightnessNight->setText(QString::number(value));
}

void SettingsWindow::onUpdateSystemVolume(int value)
{
    ui_->labelSystemVolumeValue->setText(QString::number(value));
}

void SettingsWindow::onUpdateSystemCapture(int value)
{
    ui_->labelSystemCaptureValue->setText(QString::number(value));
}

void SettingsWindow::onUpdateLux1(int value)
{
    ui_->valueLux1->setText(QString::number(value));
}

void SettingsWindow::onUpdateLux2(int value)
{
    ui_->valueLux2->setText(QString::number(value));
}

void SettingsWindow::onUpdateLux3(int value)
{
    ui_->valueLux3->setText(QString::number(value));
}

void SettingsWindow::onUpdateLux4(int value)
{
    ui_->valueLux4->setText(QString::number(value));
}

void SettingsWindow::onUpdateLux5(int value)
{
    ui_->valueLux5->setText(QString::number(value));
}

void SettingsWindow::onUpdateBrightness1(int value)
{
    ui_->valueBrightness1->setText(QString::number(value));
}

void SettingsWindow::onUpdateBrightness2(int value)
{
    ui_->valueBrightness2->setText(QString::number(value));
}

void SettingsWindow::onUpdateBrightness3(int value)
{
    ui_->valueBrightness3->setText(QString::number(value));
}

void SettingsWindow::onUpdateBrightness4(int value)
{
    ui_->valueBrightness4->setText(QString::number(value));
}

void SettingsWindow::onUpdateBrightness5(int value)
{
    ui_->valueBrightness5->setText(QString::number(value));
}

void SettingsWindow::unpairAll()
{
    // system("/usr/local/bin/crankshaft bluetooth unpair &");
}

void SettingsWindow::setTime()
{ 
    // generate param string for autoapp_helper
    std::string params;
    params.append( std::to_string(ui_->spinBoxHour->value()) );
    params.append("#");
    params.append( std::to_string(ui_->spinBoxMinute->value()) );
    params.append("#");
    system((std::string("/usr/local/bin/autoapp_helper settime#") + std::string(params) + std::string(" &") ).c_str());
}

void SettingsWindow::syncNTPTime()
{
    system("/usr/local/bin/crankshaft rtc sync &");
}

void SettingsWindow::loadSystemValues()
{
    // set brightness slider attribs
    ui_->horizontalSliderDay->setMinimum(configuration_->getCSValue("BR_MIN").toInt());
    ui_->horizontalSliderDay->setMaximum(configuration_->getCSValue("BR_MAX").toInt());
    ui_->horizontalSliderDay->setSingleStep(configuration_->getCSValue("BR_STEP").toInt());
    ui_->horizontalSliderDay->setTickInterval(configuration_->getCSValue("BR_STEP").toInt());
    ui_->horizontalSliderDay->setValue(configuration_->getCSValue("BR_DAY").toInt());

    ui_->horizontalSliderNight->setMinimum(configuration_->getCSValue("BR_MIN").toInt());
    ui_->horizontalSliderNight->setMaximum(configuration_->getCSValue("BR_MAX").toInt());
    ui_->horizontalSliderNight->setSingleStep(configuration_->getCSValue("BR_STEP").toInt());
    ui_->horizontalSliderNight->setTickInterval(configuration_->getCSValue("BR_STEP").toInt());
    ui_->horizontalSliderNight->setValue(configuration_->getCSValue("BR_NIGHT").toInt());

    ui_->horizontalSliderBrightness1->setMinimum(configuration_->getCSValue("BR_MIN").toInt());
    ui_->horizontalSliderBrightness1->setMaximum(configuration_->getCSValue("BR_MAX").toInt());
    ui_->horizontalSliderBrightness1->setSingleStep(configuration_->getCSValue("BR_STEP").toInt());
    ui_->horizontalSliderBrightness1->setTickInterval(configuration_->getCSValue("BR_STEP").toInt());

    ui_->horizontalSliderBrightness2->setMinimum(configuration_->getCSValue("BR_MIN").toInt());
    ui_->horizontalSliderBrightness2->setMaximum(configuration_->getCSValue("BR_MAX").toInt());
    ui_->horizontalSliderBrightness2->setSingleStep(configuration_->getCSValue("BR_STEP").toInt());
    ui_->horizontalSliderBrightness2->setTickInterval(configuration_->getCSValue("BR_STEP").toInt());

    ui_->horizontalSliderBrightness3->setMinimum(configuration_->getCSValue("BR_MIN").toInt());
    ui_->horizontalSliderBrightness3->setMaximum(configuration_->getCSValue("BR_MAX").toInt());
    ui_->horizontalSliderBrightness3->setSingleStep(configuration_->getCSValue("BR_STEP").toInt());
    ui_->horizontalSliderBrightness3->setTickInterval(configuration_->getCSValue("BR_STEP").toInt());

    ui_->horizontalSliderBrightness4->setMinimum(configuration_->getCSValue("BR_MIN").toInt());
    ui_->horizontalSliderBrightness4->setMaximum(configuration_->getCSValue("BR_MAX").toInt());
    ui_->horizontalSliderBrightness4->setSingleStep(configuration_->getCSValue("BR_STEP").toInt());
    ui_->horizontalSliderBrightness4->setTickInterval(configuration_->getCSValue("BR_STEP").toInt());

    ui_->horizontalSliderBrightness5->setMinimum(configuration_->getCSValue("BR_MIN").toInt());
    ui_->horizontalSliderBrightness5->setMaximum(configuration_->getCSValue("BR_MAX").toInt());
    ui_->horizontalSliderBrightness5->setSingleStep(configuration_->getCSValue("BR_STEP").toInt());
    ui_->horizontalSliderBrightness5->setTickInterval(configuration_->getCSValue("BR_STEP").toInt());

    // set tsl2561 slider attribs
    ui_->horizontalSliderLux1->setValue(configuration_->getCSValue("LUX_LEVEL_1").toInt());
    ui_->horizontalSliderBrightness1->setValue(configuration_->getCSValue("DISP_BRIGHTNESS_1").toInt());
    ui_->horizontalSliderLux2->setValue(configuration_->getCSValue("LUX_LEVEL_2").toInt());
    ui_->horizontalSliderBrightness2->setValue(configuration_->getCSValue("DISP_BRIGHTNESS_2").toInt());
    ui_->horizontalSliderLux3->setValue(configuration_->getCSValue("LUX_LEVEL_3").toInt());
    ui_->horizontalSliderBrightness3->setValue(configuration_->getCSValue("DISP_BRIGHTNESS_3").toInt());
    ui_->horizontalSliderLux4->setValue(configuration_->getCSValue("LUX_LEVEL_4").toInt());
    ui_->horizontalSliderBrightness4->setValue(configuration_->getCSValue("DISP_BRIGHTNESS_4").toInt());
    ui_->horizontalSliderLux5->setValue(configuration_->getCSValue("LUX_LEVEL_5").toInt());
    ui_->horizontalSliderBrightness5->setValue(configuration_->getCSValue("DISP_BRIGHTNESS_5").toInt());
    ui_->comboBoxCheckInterval->setCurrentText(configuration_->getCSValue("TSL2561_CHECK_INTERVAL"));
    ui_->comboBoxNightmodeStep->setCurrentText(configuration_->getCSValue("TSL2561_DAYNIGHT_ON_STEP"));

    if (std::ifstream("/tmp/return_value")) {
        QString return_values = configuration_->readFileContent("/tmp/return_value");
        QStringList getparams = return_values.split("#");

        // version string
        ui_->valueSystemVersion->setText(configuration_->readFileContent("/etc/crankshaft.build"));
        // date string
        ui_->valueSystemBuildDate->setText(configuration_->readFileContent("/etc/crankshaft.date"));
        // set volume
        ui_->labelSystemVolumeValue->setText(configuration_->readFileContent("/boot/crankshaft/volume"));
        ui_->horizontalSliderSystemVolume->setValue(configuration_->readFileContent("/boot/crankshaft/volume").toInt());
        // set cap volume
        ui_->labelSystemCaptureValue->setText(configuration_->readFileContent("/boot/crankshaft/capvolume"));
        ui_->horizontalSliderSystemCapture->setValue(configuration_->readFileContent("/boot/crankshaft/capvolume").toInt());
        // set shutdown
        ui_->valueShutdownTimer->setText("- - -");
        ui_->spinBoxShutdown->setValue(configuration_->getCSValue("DISCONNECTION_POWEROFF_MINS").toInt());
        // set disconnect
        ui_->valueDisconnectTimer->setText("- - -");
        ui_->spinBoxDisconnect->setValue(configuration_->getCSValue("DISCONNECTION_SCREEN_POWEROFF_SECS").toInt());
        // set day/night
        ui_->spinBoxDay->setValue(configuration_->getCSValue("RTC_DAY_START").toInt());
        ui_->spinBoxNight->setValue(configuration_->getCSValue("RTC_NIGHT_START").toInt());
        // set gpios
        if (configuration_->getCSValue("ENABLE_GPIO") == "1") {
           ui_->checkBoxGPIO->setChecked(true);
        } else {
            ui_->checkBoxGPIO->setChecked(false);
        }
        ui_->comboBoxDevMode->setCurrentText(configuration_->getCSValue("DEV_PIN"));
        ui_->comboBoxInvert->setCurrentText(configuration_->getCSValue("INVERT_PIN"));
        ui_->comboBoxX11->setCurrentText(configuration_->getCSValue("X11_PIN"));
        ui_->comboBoxRearcam->setCurrentText(configuration_->getCSValue("REARCAM_PIN"));
        ui_->comboBoxAndroid->setCurrentText(configuration_->getCSValue("ANDROID_PIN"));
        // set mode
        if (configuration_->getCSValue("START_X11") == "0") {
            ui_->radioButtonEGL->setChecked(true);
        } else {
            ui_->radioButtonX11->setChecked(true);
        }
        // set rotation
        if (configuration_->getCSValue("FLIP_SCREEN") == "0") {
            ui_->radioButtonScreenNormal->setChecked(true);
        } else {
            ui_->radioButtonScreenRotated->setChecked(true);
        }

        if (std::ifstream("/tmp/get_inputs")) {
            QFile inputsFile(QString("/tmp/get_inputs"));
            inputsFile.open(QIODevice::ReadOnly);
            QTextStream data_return(&inputsFile);
            QStringList inputs = data_return.readAll().split("\n");
            inputsFile.close();
            int cleaner = ui_->comboBoxPulseInput->count();
            while (cleaner > -1) {
                ui_->comboBoxPulseInput->removeItem(cleaner);
                cleaner--;
            }
            int indexin = inputs.count();
            int countin = 0;
            while (countin < indexin-1) {
                ui_->comboBoxPulseInput->addItem(inputs[countin]);
                countin++;
            }
        }

        if (std::ifstream("/tmp/get_outputs")) {
            QFile outputsFile(QString("/tmp/get_outputs"));
            outputsFile.open(QIODevice::ReadOnly);
            QTextStream data_return(&outputsFile);
            QStringList outputs = data_return.readAll().split("\n");
            outputsFile.close();
            int cleaner = ui_->comboBoxPulseOutput->count();
            while (cleaner > -1) {
                ui_->comboBoxPulseOutput->removeItem(cleaner);
                cleaner--;
            }
            int indexout = outputs.count();
            int countout = 0;
            while (countout < indexout-1) {
                ui_->comboBoxPulseOutput->addItem(outputs[countout]);
                countout++;
            }
        }

        ui_->comboBoxPulseOutput->setCurrentText(configuration_->readFileContent("/tmp/get_default_output"));
        ui_->comboBoxPulseInput->setCurrentText(configuration_->readFileContent("/tmp/get_default_input"));

        if (std::ifstream("/tmp/timezone_listing")) {
            QFile zoneFile(QString("/tmp/timezone_listing"));
            zoneFile.open(QIODevice::ReadOnly);
            QTextStream data_return(&zoneFile);
            QStringList zones = data_return.readAll().split("\n");
            zoneFile.close();
            int cleaner = ui_->comboBoxTZ->count();
            while (cleaner > 0) {
                ui_->comboBoxTZ->removeItem(cleaner);
                cleaner--;
            }
            int indexout = zones.count();
            int countzone = 0;
            while (countzone < indexout-1) {
                ui_->comboBoxTZ->addItem(zones[countzone]);
                countzone++;
            }
        }

        // set rtc
        QString rtcstring = configuration_->getParamFromFile("/boot/config.txt","dtoverlay=i2c-rtc");
        if (rtcstring != "") {
            QStringList rtc = rtcstring.split(",");
            ui_->comboBoxHardwareRTC->setCurrentText(rtc[1].trimmed());
            // set timezone
            ui_->comboBoxTZ->setCurrentText(configuration_->readFileContent("/etc/timezone"));
        } else {
            ui_->comboBoxHardwareRTC->setCurrentText("none");
            ui_->comboBoxTZ->setCurrentText(configuration_->readFileContent("/etc/timezone"));
        }

        // set dac
        QString dac = "Custom";
        if (getparams[4] == "allo-boss-dac-pcm512x-audio") {
            dac = "Allo - Boss";
        }
        if (getparams[4] == "allo-piano-dac-pcm512x-audio") {
            dac = "Allo - Piano";
        }
        if (getparams[4] == "iqaudio-dacplus") {
            dac = "IQaudIO - Pi-DAC Plus/Pro/Zero";
        }
        if (getparams[4] == "iqaudio-dacplus,unmute_amp") {
            dac = "IQaudIO - Pi-Digi Amp Plus";
        }
        if (getparams[4] == "iqaudio-dacplus,auto_mute_amp") {
            dac = "IQaudIO - Pi-Digi Amp Plus - Automute";
        }
        if (getparams[4] == "iqaudio-digi-wm8804-audio") {
            dac = "IQaudIO - Pi-Digi Plus";
        }
        if (getparams[4] == "audioinjector-wm8731-audio") {
            dac = "Audioinjector - Zero/Stereo";
        }
        if (getparams[4] == "hifiberry-dac") {
            dac = "Hifiberry - DAC";
        }
        if (getparams[4] == "hifiberry-dacplus") {
            dac = "Hifiberry - DAC Plus";
        }
        if (getparams[4] == "hifiberry-digi") {
            dac = "Hifiberry - Digi";
        }
        if (getparams[4] == "hifiberry-digi-pro") {
            dac = "Hifiberry - Digi Pro";
        }
        if (getparams[4] == "hifiberry-amp") {
            dac = "Hifiberry - DAC Amp";
        }
        if (getparams[4] == "audio") {
            dac = "Raspberry Pi - Onboard";
        }
        ui_->comboBoxHardwareDAC->setCurrentText(dac);

        // set shutdown disable
        if (configuration_->getCSValue("DISCONNECTION_POWEROFF_DISABLE") == "1") {
            ui_->checkBoxDisableShutdown->setChecked(true);
        } else {
            ui_->checkBoxDisableShutdown->setChecked(false);
        }

        // set screen off disable
        if (configuration_->getCSValue("DISCONNECTION_SCREEN_POWEROFF_DISABLE") == "1") {
            ui_->checkBoxDisableScreenOff->setChecked(true);
        } else {
            ui_->checkBoxDisableScreenOff->setChecked(false);
        }

        // set custom brightness command
        if (configuration_->getCSValue("CUSTOM_BRIGHTNESS_COMMAND") != "") {
            ui_->labelCustomBrightnessCommand->setText(configuration_->getCSValue("CUSTOM_BRIGHTNESS_COMMAND") + " brvalue");
        } else {
            ui_->labelCustomBrightnessCommand->setText("Disabled");
        }

        // set debug mode
        if (configuration_->getCSValue("DEBUG_MODE") == "1") {
            ui_->radioButtonDebugmodeEnabled->setChecked(true);
        } else {
            ui_->radioButtonDebugmodeDisabled->setChecked(true);
        }

        // GPIO based shutdown
        ui_->comboBoxGPIOShutdown->setCurrentText(configuration_->getCSValue("IGNITION_PIN"));
        ui_->spinBoxGPIOShutdownDelay->setValue(configuration_->getCSValue("IGNITION_DELAY").toInt());

        // Wifi Hotspot
        if (configuration_->getCSValue("ENABLE_HOTSPOT") == "1") {
            ui_->checkBoxHotspot->setChecked(true);
        } else {
            ui_->checkBoxHotspot->setChecked(false);
        }

        // set cam
        if (configuration_->getParamFromFile("/boot/config.txt","start_x") == "1") {
            ui_->comboBoxCam->setCurrentText("enabled");
        } else {
            ui_->comboBoxCam->setCurrentText("disabled");
        }
        if (configuration_->getCSValue("RPICAM_HFLIP") == "1") {
            ui_->checkBoxFlipX->setChecked(true);
        } else {
            ui_->checkBoxFlipX->setChecked(false);
        }
        if (configuration_->getCSValue("RPICAM_VFLIP") == "1") {
            ui_->checkBoxFlipY->setChecked(true);
        } else {
            ui_->checkBoxFlipY->setChecked(false);
        }
        ui_->comboBoxRotation->setCurrentText(configuration_->getCSValue("RPICAM_ROTATION"));
        ui_->comboBoxResolution->setCurrentText(configuration_->getCSValue("RPICAM_RESOLUTION"));
        ui_->comboBoxFPS->setCurrentText(configuration_->getCSValue("RPICAM_FPS"));
        ui_->comboBoxAWB->setCurrentText(configuration_->getCSValue("RPICAM_AWB"));
        ui_->comboBoxEXP->setCurrentText(configuration_->getCSValue("RPICAM_EXP"));
        ui_->comboBoxLoopTime->setCurrentText(configuration_->getCSValue("RPICAM_LOOPTIME"));
        ui_->comboBoxLoopCount->setCurrentText(configuration_->getCSValue("RPICAM_LOOPCOUNT"));

        if (configuration_->getCSValue("RPICAM_AUTORECORDING") == "1") {
            ui_->checkBoxAutoRecording->setChecked(true);
        } else {
            ui_->checkBoxAutoRecording->setChecked(false);
        }

        if (configuration_->getCSValue("USBCAM_USE") == "1") {
            ui_->comboBoxUSBCam->setCurrentText("enabled");
        } else {
            ui_->comboBoxUSBCam->setCurrentText("none");
        }
        if (configuration_->getCSValue("USBCAM_ROTATION") == "1") {
            ui_->comboBoxUSBRotation->setCurrentText("180");
        } else {
            ui_->comboBoxUSBRotation->setCurrentText("0");
        }
        if (configuration_->getCSValue("USBCAM_HFLIP") == "1") {
            ui_->checkBoxFlipXUSB->setChecked(true);
        } else {
            ui_->checkBoxFlipXUSB->setChecked(false);
        }
        if (configuration_->getCSValue("USBCAM_VFLIP") == "1") {
            ui_->checkBoxFlipYUSB->setChecked(true);
        } else {
            ui_->checkBoxFlipYUSB->setChecked(false);
        }

        
        // set lightsensor
        if (std::ifstream("/etc/cs_lightsensor")) {
            ui_->comboBoxLS->setCurrentIndex(1);
            ui_->groupBoxSliderDay->hide();
            ui_->groupBoxSliderNight->hide();
        } else {
            ui_->comboBoxLS->setCurrentIndex(0);
            ui_->pushButtonTab9->hide();
            ui_->groupBoxSliderDay->show();
            ui_->groupBoxSliderNight->show();
        }
        ui_->comboBoxDayNight->setCurrentText(configuration_->getCSValue("DAYNIGHT_PIN"));
        if (configuration_->getCSValue("RTC_DAYNIGHT") == "1") {
            ui_->checkBoxDisableDayNightRTC->setChecked(false);
        } else {
            ui_->checkBoxDisableDayNightRTC->setChecked(true);
        }
        QString theme = configuration_->getParamFromFile("/etc/plymouth/plymouthd.conf","Theme");
        if (theme == "csnganimation") {
            ui_->radioButtonAnimatedCSNG->setChecked(true);
        }
        else if (theme == "crankshaft") {
            ui_->radioButtonCSNG->setChecked(true);
        }
        else if (theme == "custom") {
            ui_->radioButtonCustom->setChecked(true);
        }
        // wifi country code
        ui_->comboBoxCountryCode->setCurrentIndex(ui_->comboBoxCountryCode->findText(configuration_->getCSValue("WIFI_COUNTRY"), Qt::MatchFlag::MatchStartsWith));
        // set screen blank instead off
        if (configuration_->getCSValue("SCREEN_POWEROFF_OVERRIDE") == "1") {
            ui_->checkBoxBlankOnly->setChecked(true);
        } else {
            ui_->checkBoxBlankOnly->setChecked(false);
        }
    }
    // update network info
    updateNetworkInfo();
}

void SettingsWindow::onStartHotspot()
{
    ui_->label_modeswitchprogress->setText("Wait ...");
    ui_->clientNetworkSelect->hide();
    ui_->label_notavailable->show();
    ui_->radioButtonClient->setEnabled(0);
    ui_->radioButtonHotspot->setEnabled(0);
    ui_->lineEdit_wlan0->setText("");
    ui_->lineEditWifiSSID->setText("");
    ui_->pushButtonNetworkAuto->hide();
    qApp->processEvents();
    std::remove("/tmp/manual_hotspot_control");
    std::ofstream("/tmp/manual_hotspot_control");
    system("/opt/crankshaft/service_hotspot.sh start &");
}

void SettingsWindow::onStopHotspot()
{
    ui_->label_modeswitchprogress->setText("Wait ...");
    ui_->clientNetworkSelect->hide();
    ui_->label_notavailable->show();
    ui_->radioButtonClient->setEnabled(0);
    ui_->radioButtonHotspot->setEnabled(0);
    ui_->lineEdit_wlan0->setText("");
    ui_->lineEditWifiSSID->setText("");
    ui_->lineEditPassword->setText("");
    ui_->pushButtonNetworkAuto->hide();
    qApp->processEvents();
    system("/opt/crankshaft/service_hotspot.sh stop &");
}

void SettingsWindow::updateSystemInfo()
{
    // free ram
    struct sysinfo info;
    sysinfo(&info);
    ui_->valueSystemFreeMem->setText(QString::number(info.freeram/1024/1024) + " MB");
    // current cpu speed
    QString freq = configuration_->readFileContent("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_cur_freq");
    int currentfreq = freq.toInt()/1000;
    ui_->valueSystemCPUFreq->setText(QString::number(currentfreq) + "MHz");
    // current cpu temp
    QString temp = configuration_->readFileContent("/sys/class/thermal/thermal_zone0/temp");
    int currenttemp = temp.toInt()/1000;
    ui_->valueSystemCPUTemp->setText(QString::number(currenttemp) + "°C");
    // get remaining times
    QProcess process;
    process.start("/bin/bash", QStringList() << "-c" << "systemctl list-timers -all | grep disconnect | awk {'print $1'}");
    process.waitForFinished(-1);
    QString stdout = process.readAllStandardOutput();
    if (stdout.simplified() != "n/a") {
        process.start("/bin/bash", QStringList() << "-c" << "systemctl list-timers -all | grep disconnect | awk {'print $5\" \"$6'}");
        process.waitForFinished(-1);
        QString stdout = process.readAllStandardOutput();
        if (stdout.simplified() != "") {
            ui_->valueDisconnectTimer->setText(stdout.simplified());
        } else {
            ui_->valueDisconnectTimer->setText("Stopped");
        }
    } else {
        ui_->valueDisconnectTimer->setText("Stopped");
    }
    process.start("/bin/bash", QStringList() << "-c" << "systemctl list-timers -all | grep shutdown | awk {'print $1'}");
    process.waitForFinished(-1);
    stdout = process.readAllStandardOutput();
    if (stdout.simplified() != "n/a") {
        process.start("/bin/bash", QStringList() << "-c" << "systemctl list-timers -all | grep shutdown | awk {'print $5\" \"$6'}");
        process.waitForFinished(-1);
        QString stdout = process.readAllStandardOutput();
        if (stdout.simplified() != "") {
            ui_->valueShutdownTimer->setText(stdout.simplified());
        } else {
            ui_->valueShutdownTimer->setText("Stopped");
        }
    } else {
        ui_->valueShutdownTimer->setText("Stopped");
    }
}

void SettingsWindow::show_tab1()
{
    ui_->tab2->hide();
    ui_->tab3->hide();
    ui_->tab4->hide();
    ui_->tab5->hide();
    ui_->tab6->hide();
    ui_->tab7->hide();
    ui_->tab8->hide();
    ui_->tab9->hide();
    ui_->tab1->show();
}

void SettingsWindow::show_tab2()
{
    ui_->tab1->hide();
    ui_->tab3->hide();
    ui_->tab4->hide();
    ui_->tab5->hide();
    ui_->tab6->hide();
    ui_->tab7->hide();
    ui_->tab8->hide();
    ui_->tab9->hide();
    ui_->tab2->show();
}

void SettingsWindow::show_tab3()
{
    ui_->tab1->hide();
    ui_->tab2->hide();
    ui_->tab4->hide();
    ui_->tab5->hide();
    ui_->tab6->hide();
    ui_->tab7->hide();
    ui_->tab8->hide();
    ui_->tab9->hide();
    ui_->tab3->show();
}

void SettingsWindow::show_tab4()
{
    ui_->tab1->hide();
    ui_->tab2->hide();
    ui_->tab3->hide();
    ui_->tab5->hide();
    ui_->tab6->hide();
    ui_->tab7->hide();
    ui_->tab8->hide();
    ui_->tab9->hide();
    ui_->tab4->show();
}

void SettingsWindow::show_tab5()
{
    ui_->tab1->hide();
    ui_->tab2->hide();
    ui_->tab3->hide();
    ui_->tab4->hide();
    ui_->tab6->hide();
    ui_->tab7->hide();
    ui_->tab8->hide();
    ui_->tab9->hide();
    ui_->tab5->show();
}

void SettingsWindow::show_tab6()
{
    ui_->tab1->hide();
    ui_->tab2->hide();
    ui_->tab3->hide();
    ui_->tab4->hide();
    ui_->tab5->hide();
    ui_->tab7->hide();
    ui_->tab8->hide();
    ui_->tab9->hide();
    ui_->tab6->show();
}

void SettingsWindow::show_tab7()
{
    ui_->tab1->hide();
    ui_->tab2->hide();
    ui_->tab3->hide();
    ui_->tab4->hide();
    ui_->tab5->hide();
    ui_->tab6->hide();
    ui_->tab8->hide();
    ui_->tab9->hide();
    ui_->tab7->show();
}

void SettingsWindow::show_tab8()
{
    ui_->tab1->hide();
    ui_->tab2->hide();
    ui_->tab3->hide();
    ui_->tab4->hide();
    ui_->tab5->hide();
    ui_->tab6->hide();
    ui_->tab7->hide();
    ui_->tab9->hide();
    ui_->tab8->show();
}

void SettingsWindow::show_tab9()
{
    ui_->tab1->hide();
    ui_->tab2->hide();
    ui_->tab3->hide();
    ui_->tab4->hide();
    ui_->tab5->hide();
    ui_->tab6->hide();
    ui_->tab7->hide();
    ui_->tab8->hide();
    ui_->tab9->show();
}

}
}
}
}

void f1x::openauto::autoapp::ui::SettingsWindow::on_pushButtonAudioTest_clicked()
{
    ui_->labelTestInProgress->show();
    ui_->pushButtonAudioTest->hide();
    qApp->processEvents();
    system("/usr/local/bin/crankshaft audio test");
    ui_->pushButtonAudioTest->show();
    ui_->labelTestInProgress->hide();
}

void f1x::openauto::autoapp::ui::SettingsWindow::updateNetworkInfo()
{
    if (std::ifstream("/tmp/samba_running")) {
        ui_->labelSambaStatus->setText("running");
        if (ui_->pushButtonSambaStart->isVisible() == true) {
            ui_->pushButtonSambaStart->hide();
            ui_->pushButtonSambaStop->show();
        }
    } else {
        ui_->labelSambaStatus->setText("stopped");
        if (ui_->pushButtonSambaStop->isVisible() == true) {
            ui_->pushButtonSambaStop->hide();
            ui_->pushButtonSambaStart->show();
        }
    }

    if (!std::ifstream("/tmp/mode_change_progress")) {
        QNetworkInterface eth0if = QNetworkInterface::interfaceFromName("eth0");
        if (eth0if.flags().testFlag(QNetworkInterface::IsUp)) {
            QList<QNetworkAddressEntry> entrieseth0 = eth0if.addressEntries();
            if (!entrieseth0.isEmpty()) {
                QNetworkAddressEntry eth0 = entrieseth0.first();
                //qDebug() << "eth0: " << eth0.ip();
                ui_->lineEdit_eth0->setText(eth0.ip().toString());
            }
        } else {
            //qDebug() << "eth0: down";
            ui_->lineEdit_eth0->setText("interface down");
        }

        QNetworkInterface wlan0if = QNetworkInterface::interfaceFromName("wlan0");
        if (wlan0if.flags().testFlag(QNetworkInterface::IsUp)) {
            QList<QNetworkAddressEntry> entrieswlan0 = wlan0if.addressEntries();
            if (!entrieswlan0.isEmpty()) {
                QNetworkAddressEntry wlan0 = entrieswlan0.first();
                //qDebug() << "wlan0: " << wlan0.ip();
                ui_->lineEdit_wlan0->setText(wlan0.ip().toString());
            }
        } else {
            //qDebug() << "wlan0: down";
            ui_->lineEdit_wlan0->setText("interface down");
        }

        if (std::ifstream("/tmp/hotspot_active")) {
            ui_->radioButtonClient->setEnabled(1);
            ui_->radioButtonHotspot->setEnabled(1);
            ui_->radioButtonHotspot->setChecked(1);
            ui_->radioButtonClient->setChecked(0);
            ui_->label_modeswitchprogress->setText("Ok");
            ui_->lineEditWifiSSID->setText(configuration_->getParamFromFile("/etc/hostapd/hostapd.conf","ssid"));
            ui_->lineEditPassword->show();
            ui_->label_password->show();
            ui_->lineEditPassword->setText(configuration_->getParamFromFile("/etc/hostapd/hostapd.conf","wpa_passphrase"));
            ui_->clientNetworkSelect->hide();
            ui_->pushButtonNetworkAuto->hide();
            ui_->label_notavailable->show();
        } else {
            ui_->radioButtonClient->setEnabled(1);
            ui_->radioButtonHotspot->setEnabled(1);
            ui_->radioButtonHotspot->setChecked(0);
            ui_->radioButtonClient->setChecked(1);
            ui_->label_modeswitchprogress->setText("Ok");
            ui_->lineEditWifiSSID->setText(configuration_->readFileContent("/tmp/wifi_ssid"));
            ui_->lineEditPassword->hide();
            ui_->label_password->hide();
            ui_->lineEditPassword->setText("");
            ui_->clientNetworkSelect->show();
            ui_->label_notavailable->hide();
            ui_->pushButtonNetworkAuto->show();

            if (!std::ifstream("/boot/crankshaft/network1.conf")) {
                ui_->pushButtonNetwork1->hide();
                ui_->pushButtonNetwork0->show();
            }
            if (!std::ifstream("/boot/crankshaft/network0.conf")) {
                ui_->pushButtonNetwork1->hide();
                ui_->pushButtonNetwork0->setText(configuration_->getCSValue("WIFI2_SSID"));
            }
            if (!std::ifstream("/boot/crankshaft/network0.conf") && !std::ifstream("/boot/crankshaft/network1.conf")) {
                ui_->pushButtonNetwork0->hide();
                ui_->pushButtonNetwork1->hide();
                ui_->pushButtonNetworkAuto->hide();
                ui_->label_notavailable->show();
            }
        }
    }
}

void f1x::openauto::autoapp::ui::SettingsWindow::on_pushButtonNetwork0_clicked()
{
    ui_->lineEdit_wlan0->setText("");
    ui_->lineEditWifiSSID->setText("");
    ui_->lineEditPassword->setText("");
    qApp->processEvents();
    system("/usr/local/bin/crankshaft network 0 >/dev/null 2>&1 &");

}

void f1x::openauto::autoapp::ui::SettingsWindow::on_pushButtonNetwork1_clicked()
{
    ui_->lineEdit_wlan0->setText("");
    ui_->lineEditWifiSSID->setText("");
    ui_->lineEditPassword->setText("");
    qApp->processEvents();
    system("/usr/local/bin/crankshaft network 1 >/dev/null 2>&1 &");
}

void f1x::openauto::autoapp::ui::SettingsWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape)
    {
        f1x::openauto::autoapp::ui::SettingsWindow::close();
    }
    if (event->key() == Qt::Key_Return) {
        QApplication::postEvent (QApplication::focusWidget(), new QKeyEvent ( QEvent::KeyPress, Qt::Key_Space, Qt::NoModifier));
        QApplication::postEvent (QApplication::focusWidget(), new QKeyEvent ( QEvent::KeyRelease, Qt::Key_Space, Qt::NoModifier));
    }    
    if (event->key() == Qt::Key_1) {
        QApplication::postEvent (QApplication::focusWidget(), new QKeyEvent ( QEvent::KeyPress, Qt::Key_Tab, Qt::ShiftModifier));
    }
    if (event->key() == Qt::Key_2) {
        QApplication::postEvent (QApplication::focusWidget(), new QKeyEvent ( QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier));
    }
}
