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

#include <QMessageBox>
#include <f1x/openauto/autoapp/UI/SettingsWindow.hpp>
#include "ui_settingswindow.h"
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <string>

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
    connect(ui_->horizontalSliderScreenDPI, &QSlider::valueChanged, this, &SettingsWindow::onUpdateScreenDPI);
    connect(ui_->radioButtonUseExternalBluetoothAdapter, &QRadioButton::clicked, [&](bool checked) { ui_->lineEditExternalBluetoothAdapterAddress->setEnabled(checked); });
    connect(ui_->radioButtonDisableBluetooth, &QRadioButton::clicked, [&]() { ui_->lineEditExternalBluetoothAdapterAddress->setEnabled(false); });
    connect(ui_->radioButtonUseLocalBluetoothAdapter, &QRadioButton::clicked, [&]() { ui_->lineEditExternalBluetoothAdapterAddress->setEnabled(false); });
    connect(ui_->pushButtonClearSelection, &QPushButton::clicked, std::bind(&SettingsWindow::setButtonCheckBoxes, this, false));
    connect(ui_->pushButtonSelectAll, &QPushButton::clicked, std::bind(&SettingsWindow::setButtonCheckBoxes, this, true));
    connect(ui_->pushButtonResetToDefaults, &QPushButton::clicked, this, &SettingsWindow::onResetToDefaults);
    connect(ui_->pushButtonShowBindings, &QPushButton::clicked, this, &SettingsWindow::onShowBindings);
    connect(ui_->horizontalSliderSystemVolume, &QSlider::valueChanged, this, &SettingsWindow::onUpdateSystemVolume);
    connect(ui_->horizontalSliderSystemCapture, &QSlider::valueChanged, this, &SettingsWindow::onUpdateSystemCapture);

}

SettingsWindow::~SettingsWindow()
{
    delete ui_;
}

void SettingsWindow::onSave()
{
    configuration_->setHandednessOfTrafficType(ui_->radioButtonLeftHandDrive->isChecked() ? configuration::HandednessOfTrafficType::LEFT_HAND_DRIVE : configuration::HandednessOfTrafficType::RIGHT_HAND_DRIVE);
    configuration_->showClock(ui_->checkBoxShowClock->isChecked());
    configuration_->setVideoFPS(ui_->radioButton30FPS->isChecked() ? aasdk::proto::enums::VideoFPS::_30 : aasdk::proto::enums::VideoFPS::_60);

    if(ui_->radioButton480p->isChecked())
    {
        configuration_->setVideoResolution(aasdk::proto::enums::VideoResolution::_480p);
    }
    else if(ui_->radioButton720p->isChecked())
    {
        configuration_->setVideoResolution(aasdk::proto::enums::VideoResolution::_720p);
    }
    else if(ui_->radioButton1080p->isChecked())
    {
        configuration_->setVideoResolution(aasdk::proto::enums::VideoResolution::_1080p);
    }

    configuration_->setScreenDPI(static_cast<size_t>(ui_->horizontalSliderScreenDPI->value()));
    configuration_->setOMXLayerIndex(ui_->spinBoxOmxLayerIndex->value());

    QRect videoMargins(0, 0, ui_->spinBoxVideoMarginWidth->value(), ui_->spinBoxVideoMarginHeight->value());
    configuration_->setVideoMargins(std::move(videoMargins));

    configuration_->setTouchscreenEnabled(ui_->checkBoxEnableTouchscreen->isChecked());
    this->saveButtonCheckBoxes();

    if(ui_->radioButtonDisableBluetooth->isChecked())
    {
        configuration_->setBluetoothAdapterType(configuration::BluetoothAdapterType::NONE);
    }
    else if(ui_->radioButtonUseLocalBluetoothAdapter->isChecked())
    {
        configuration_->setBluetoothAdapterType(configuration::BluetoothAdapterType::LOCAL);
    }
    else if(ui_->radioButtonUseExternalBluetoothAdapter->isChecked())
    {
        configuration_->setBluetoothAdapterType(configuration::BluetoothAdapterType::REMOTE);
    }

    configuration_->setBluetoothRemoteAdapterAddress(ui_->lineEditExternalBluetoothAdapterAddress->text().toStdString());

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
    if (ui_->checkBoxBluetoothAutoPair->isChecked()) {
        params.append("1");
    } else {
        params.append("0");
    }
    params.append("#");
    params.append( std::string(ui_->comboBoxBluetooth->currentText().toStdString()) );
    params.append("#");
    if (ui_->checkBoxHardwareSave->isChecked()) {
        params.append("1");
    } else {
        params.append("0");
    }
    params.append("#");

    system((std::string("/usr/local/bin/autoapp_helper setparams#") + std::string(params) + std::string(" &") ).c_str());

    this->close();
}

void SettingsWindow::onResetToDefaults()
{
    QMessageBox confirmationMessage(QMessageBox::Question, "Confirmation", "Are you sure you want to reset settings?", QMessageBox::Yes | QMessageBox::Cancel);
    confirmationMessage.setWindowFlags(Qt::WindowStaysOnTopHint);
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

    ui_->radioButton30FPS->setChecked(configuration_->getVideoFPS() == aasdk::proto::enums::VideoFPS::_30);
    ui_->radioButton60FPS->setChecked(configuration_->getVideoFPS() == aasdk::proto::enums::VideoFPS::_60);

    ui_->radioButton480p->setChecked(configuration_->getVideoResolution() == aasdk::proto::enums::VideoResolution::_480p);
    ui_->radioButton720p->setChecked(configuration_->getVideoResolution() == aasdk::proto::enums::VideoResolution::_720p);
    ui_->radioButton1080p->setChecked(configuration_->getVideoResolution() == aasdk::proto::enums::VideoResolution::_1080p);
    ui_->horizontalSliderScreenDPI->setValue(static_cast<int>(configuration_->getScreenDPI()));
    ui_->spinBoxOmxLayerIndex->setValue(configuration_->getOMXLayerIndex());

    const auto& videoMargins = configuration_->getVideoMargins();
    ui_->spinBoxVideoMarginWidth->setValue(videoMargins.width());
    ui_->spinBoxVideoMarginHeight->setValue(videoMargins.height());

    ui_->checkBoxEnableTouchscreen->setChecked(configuration_->getTouchscreenEnabled());
    this->loadButtonCheckBoxes();

    ui_->radioButtonDisableBluetooth->setChecked(configuration_->getBluetoothAdapterType() == configuration::BluetoothAdapterType::NONE);
    ui_->radioButtonUseLocalBluetoothAdapter->setChecked(configuration_->getBluetoothAdapterType() == configuration::BluetoothAdapterType::LOCAL);
    ui_->radioButtonUseExternalBluetoothAdapter->setChecked(configuration_->getBluetoothAdapterType() == configuration::BluetoothAdapterType::REMOTE);
    ui_->lineEditExternalBluetoothAdapterAddress->setEnabled(configuration_->getBluetoothAdapterType() == configuration::BluetoothAdapterType::REMOTE);
    ui_->lineEditExternalBluetoothAdapterAddress->setText(QString::fromStdString(configuration_->getBluetoothRemoteAdapterAddress()));

    ui_->checkBoxMusicAudioChannel->setChecked(configuration_->musicAudioChannelEnabled());
    ui_->checkBoxSpeechAudioChannel->setChecked(configuration_->speechAudioChannelEnabled());

    const auto& audioOutputBackendType = configuration_->getAudioOutputBackendType();
    ui_->radioButtonRtAudio->setChecked(audioOutputBackendType == configuration::AudioOutputBackendType::RTAUDIO);
    ui_->radioButtonQtAudio->setChecked(audioOutputBackendType == configuration::AudioOutputBackendType::QT);

    ui_->checkBoxHardwareSave->setChecked(false);
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

void SettingsWindow::onUpdateSystemVolume(int value)
{
    ui_->labelSystemVolumeValue->setText(QString::number(value));
}

void SettingsWindow::onUpdateSystemCapture(int value)
{
    ui_->labelSystemCaptureValue->setText(QString::number(value));
}

void SettingsWindow::unpairAll()
{
    system("/usr/local/bin/crankshaft bluetooth unpair &");
}

void SettingsWindow::loadSystemValues()
{
    // Generate param file
    system("/usr/local/bin/autoapp_helper getparams");

    // read param file
    QFileInfo paramFile("/tmp/return_value");
    if (paramFile.exists()) {
        QFile paramFile(QString("/tmp/return_value"));
        paramFile.open(QIODevice::ReadOnly);
        QTextStream data_param(&paramFile);
        QStringList getparams = data_param.readAll().split("#");
        paramFile.close();
        // version string
        ui_->valueSystemVersion->setText(getparams[0]);
        // date string
        ui_->valueSystemBuildDate->setText(getparams[1]);
        // set volume
        ui_->labelSystemVolumeValue->setText(getparams[2]);
        ui_->horizontalSliderSystemVolume->setValue(getparams[2].toInt());
        // set cap volume
        ui_->labelSystemCaptureValue->setText(getparams[3]);
        ui_->horizontalSliderSystemCapture->setValue(getparams[3].toInt());
        // set shutdown
        ui_->valueShutdownTimer->setText(getparams[4]);
        ui_->spinBoxShutdown->setValue(getparams[5].toInt());
        // set disconnect
        ui_->valueDisconnectTimer->setText(getparams[6]);
        ui_->spinBoxDisconnect->setValue(getparams[7].toInt());
        // set day/night
        ui_->spinBoxDay->setValue(getparams[8].toInt());
        ui_->spinBoxNight->setValue(getparams[9].toInt());
        // set gpios
        if (getparams[10] == "1") {
            ui_->checkBoxGPIO->setChecked(true);
        } else {
            ui_->checkBoxGPIO->setChecked(false);
        }
        ui_->comboBoxDevMode->setCurrentText(getparams[11]);
        ui_->comboBoxInvert->setCurrentText(getparams[12]);
        ui_->comboBoxX11->setCurrentText(getparams[13]);
        ui_->comboBoxRearcam->setCurrentText(getparams[14]);
        ui_->comboBoxAndroid->setCurrentText(getparams[15]);
        // set mode
        if (getparams[16] == "0") {
            ui_->radioButtonEGL->setChecked(true);
        } else {
            ui_->radioButtonX11->setChecked(true);
        }
        // set rotation
        if (getparams[17] == "0") {
            ui_->radioButtonScreenNormal->setChecked(true);
        } else {
            ui_->radioButtonScreenRotated->setChecked(true);
        }
        // set free mem
        ui_->valueSystemFreeMem->setText(getparams[18]);
        // set cpu freq
        ui_->valueSystemCPUFreq->setText(getparams[19] + "MHz");
        // set cpu temp
        ui_->valueSystemCPUTemp->setText(getparams[20]);

        QFileInfo inputsFile("/tmp/get_inputs");
        if (inputsFile.exists()) {
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

        QFileInfo outputsFile("/tmp/get_outputs");
        if (outputsFile.exists()) {
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

        QFileInfo defaultoutputFile("/tmp/get_default_output");
        if (defaultoutputFile.exists()) {
            QFile defaultoutputFile(QString("/tmp/get_default_output"));
            defaultoutputFile.open(QIODevice::ReadOnly);
            QTextStream data_return(&defaultoutputFile);
            QStringList defoutput = data_return.readAll().split("\n");
            defaultoutputFile.close();
            ui_->comboBoxPulseOutput->setCurrentText(defoutput[0]);
        }

        QFileInfo defaultinputFile("/tmp/get_default_input");
        if (defaultinputFile.exists()) {
            QFile defaultinputFile(QString("/tmp/get_default_input"));
            defaultinputFile.open(QIODevice::ReadOnly);
            QTextStream data_return(&defaultinputFile);
            QStringList definput = data_return.readAll().split("\n");
            defaultinputFile.close();
            ui_->comboBoxPulseInput->setCurrentText(definput[0]);
        }

        QFileInfo zoneFile("/tmp/timezone_listing");
        if (zoneFile.exists()) {
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
        ui_->comboBoxHardwareRTC->setCurrentText(getparams[21]);
        // set timezone
        ui_->comboBoxTZ->setCurrentText(getparams[22]);

        // set dac
        QString dac = "Custom";
        if (getparams[23] == "allo-boss-dac-pcm512x-audio") {
            dac = "Allo - Boss";
        }
        if (getparams[23] == "allo-piano-dac-pcm512x-audio") {
            dac = "Allo - Piano";
        }
        if (getparams[23] == "iqaudio-dacplus") {
            dac = "IQaudIO - Pi-DAC Plus/Pro/Zero";
        }
        if (getparams[23] == "iqaudio-dacplus,unmute_amp") {
            dac = "IQaudIO - Pi-Digi Amp Plus";
        }
        if (getparams[23] == "iqaudio-dacplus,auto_mute_amp") {
            dac = "IQaudIO - Pi-Digi Amp Plus - Automute";
        }
        if (getparams[23] == "iqaudio-digi-wm8804-audio") {
            dac = "IQaudIO - Pi-Digi Plus";
        }
        if (getparams[23] == "audioinjector-wm8731-audio") {
            dac = "Audioinjector - Zero/Stereo";
        }
        if (getparams[23] == "hifiberry-dac") {
            dac = "Hifiberry - DAC";
        }
        if (getparams[23] == "hifiberry-dacplus") {
            dac = "Hifiberry - DAC Plus";
        }
        if (getparams[23] == "hifiberry-digi") {
            dac = "Hifiberry - DAC Digi";
        }
        if (getparams[23] == "hifiberry-amp") {
            dac = "Hifiberry - DAC Amp";
        }
        if (getparams[23] == "audio") {
            dac = "Raspberry Pi - Onboard";
        }
        ui_->comboBoxHardwareDAC->setCurrentText(dac);

        // set shutdown disable
        if (getparams[24] == "1") {
            ui_->checkBoxDisableShutdown->setChecked(true);
        } else {
            ui_->checkBoxDisableShutdown->setChecked(false);
        }

        // set screen off disable
        if (getparams[25] == "1") {
            ui_->checkBoxDisableScreenOff->setChecked(true);
        } else {
            ui_->checkBoxDisableScreenOff->setChecked(false);
        }

        // set custom brightness command
        if (getparams[26] != "0") {
            ui_->labelCustomBrightnessCommand->setText(getparams[26] + " brvalue");
        } else {
            ui_->labelCustomBrightnessCommand->setText("Disabled");
        }

        // set debug mode
        if (getparams[27] == "1") {
            ui_->radioButtonDebugmodeEnabled->setChecked(true);
        } else {
            ui_->radioButtonDebugmodeDisabled->setChecked(true);
        }

        // GPIO based shutdown
        ui_->comboBoxGPIOShutdown->setCurrentText(getparams[28]);
        ui_->spinBoxGPIOShutdownDelay->setValue(getparams[29].toInt());

        // Wifi Credentials
        ui_->lineEditWifiClientSSID->setText(getparams[30]);

        // Wifi Hotspot Credentials
        if (getparams[31] == "1") {
            ui_->checkBoxHotspot->setChecked(true);
        } else {
            ui_->checkBoxHotspot->setChecked(false);
        }

        ui_->lineEditWifiHotspotSSID->setText(getparams[32]);

        // set cam
        ui_->comboBoxCam->setCurrentText(getparams[33]);

        // set bluetooth
        if (getparams[34] == "1") {
            // check external bluetooth enabled
            if (getparams[36] == "1") {
                ui_->radioButtonUseExternalBluetoothAdapter->setChecked(true);
            } else {
                ui_->radioButtonUseLocalBluetoothAdapter->setChecked(true);
            }
            // mac
            ui_->lineEditExternalBluetoothAdapterAddress->setText(getparams[37]);
        } else {
            ui_->radioButtonDisableBluetooth->setChecked(true);
            ui_->lineEditExternalBluetoothAdapterAddress->setText("");
        }
        if (getparams[35] == "1") {
            ui_->checkBoxBluetoothAutoPair->setChecked(true);
        } else {
            ui_->checkBoxBluetoothAutoPair->setChecked(false);
        }
        // set timezone
        ui_->comboBoxBluetooth->setCurrentText(getparams[38]);
    }
}

void SettingsWindow::onShowBindings()
{
    const QString message = QString("Enter -> [Enter] \n")
                            + QString("Left -> [Left] \n")
                            + QString("Right -> [Right] \n")
                            + QString("Up -> [Up] \n")
                            + QString("Down -> [Down] \n")
                            + QString("Back -> [Esc] \n")
                            + QString("Home -> [H] \n")
                            + QString("Phone -> [P] \n")
                            + QString("Call end -> [O] \n")
                            + QString("Play -> [X] \n")
                            + QString("Pause -> [C] \n")
                            + QString("Previous track -> [V]/[Media Previous] \n")
                            + QString("Next track -> [N]/[Media Next] \n")
                            + QString("Toggle play -> [B]/[Media Play] \n")
                            + QString("Voice command -> [M] \n")
                            + QString("Wheel left -> [1] \n")
                            + QString("Wheel right -> [2]");

    QMessageBox confirmationMessage(QMessageBox::Information, "Information", message, QMessageBox::Ok);
    confirmationMessage.setWindowFlags(Qt::WindowStaysOnTopHint);
    confirmationMessage.exec();
}

}
}
}
}
