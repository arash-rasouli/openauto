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

#include <f1x/openauto/autoapp/Configuration/Configuration.hpp>
#include <f1x/openauto/Common/Log.hpp>
#include <QTouchDevice>

namespace f1x
{
namespace openauto
{
namespace autoapp
{
namespace configuration
{

const std::string Configuration::cConfigFileName = "openauto.ini";

const std::string Configuration::cGeneralShowClockKey = "General.ShowClock";

const std::string Configuration::cGeneralShowBigClockKey = "General.ShowBigClock";
const std::string Configuration::cGeneralOldGUIKey = "General.OldGUI";
const std::string Configuration::cGeneralAlphaTransKey = "General.AlphaTrans";
const std::string Configuration::cGeneralHideMenuToggleKey = "General.HideMenuToggle";
const std::string Configuration::cGeneralHideAlphaKey = "General.HideAlpha";
const std::string Configuration::cGeneralShowLuxKey = "General.ShowLux";
const std::string Configuration::cGeneralShowCursorKey = "General.ShowCursor";
const std::string Configuration::cGeneralHideBrightnessControlKey = "General.HideBrightnessControl";
const std::string Configuration::cGeneralShowNetworkinfoKey = "General.ShowNetworkinfo";
const std::string Configuration::cGeneralHideWarningKey = "General.HideWarning";

const std::string Configuration::cGeneralHandednessOfTrafficTypeKey = "General.HandednessOfTrafficType";

const std::string Configuration::cGeneralMp3MasterPathKey = "General.Mp3MasterPath";
const std::string Configuration::cGeneralMp3SubFolderKey = "General.Mp3SubFolder";
const std::string Configuration::cGeneralMp3TrackKey = "General.Mp3Track";
const std::string Configuration::cGeneralMp3AutoPlayKey = "General.Mp3AutoPlay";
const std::string Configuration::cGeneralShowAutoPlayKey = "General.ShowAutoPlay";
const std::string Configuration::cGeneralInstantPlayKey = "General.InstantPlay";

const std::string Configuration::cVideoFPSKey = "Video.FPS";
const std::string Configuration::cVideoResolutionKey = "Video.Resolution";
const std::string Configuration::cVideoScreenDPIKey = "Video.ScreenDPI";
const std::string Configuration::cVideoOMXLayerIndexKey = "Video.OMXLayerIndex";
const std::string Configuration::cVideoMarginWidth = "Video.MarginWidth";
const std::string Configuration::cVideoMarginHeight = "Video.MarginHeight";

const std::string Configuration::cAudioMusicAudioChannelEnabled = "Audio.MusicAudioChannelEnabled";
const std::string Configuration::cAudioSpeechAudioChannelEnabled = "Audio.SpeechAudioChannelEnabled";
const std::string Configuration::cAudioOutputBackendType = "Audio.OutputBackendType";



const std::string Configuration::cInputEnableTouchscreenKey = "Input.EnableTouchscreen";
const std::string Configuration::cInputEnablePlayerControlKey = "Input.EnablePlayerControl";
const std::string Configuration::cInputPlayButtonKey = "Input.PlayButton";
const std::string Configuration::cInputPauseButtonKey = "Input.PauseButton";
const std::string Configuration::cInputTogglePlayButtonKey = "Input.TogglePlayButton";
const std::string Configuration::cInputNextTrackButtonKey = "Input.NextTrackButton";
const std::string Configuration::cInputPreviousTrackButtonKey = "Input.PreviousTrackButton";
const std::string Configuration::cInputHomeButtonKey = "Input.HomeButton";
const std::string Configuration::cInputCallEndButtonKey = "Input.CallEndButton";
const std::string Configuration::cInputVoiceCommandButtonKey = "Input.VoiceCommandButton";
const std::string Configuration::cInputLeftButtonKey = "Input.LeftButton";
const std::string Configuration::cInputRightButtonKey = "Input.RightButton";
const std::string Configuration::cInputUpButtonKey = "Input.UpButton";
const std::string Configuration::cInputDownButtonKey = "Input.DownButton";
const std::string Configuration::cInputScrollWheelButtonKey = "Input.ScrollWheelButton";
const std::string Configuration::cInputBackButtonKey = "Input.BackButton";
const std::string Configuration::cInputEnterButtonKey = "Input.EnterButton";
const std::string Configuration::cInputNavButtonKey = "Input.NavButton";

Configuration::Configuration()
{
    this->load();
}

void Configuration::load()
{
    boost::property_tree::ptree iniConfig;

    try
    {
        boost::property_tree::ini_parser::read_ini(cConfigFileName, iniConfig);

        handednessOfTrafficType_ = static_cast<HandednessOfTrafficType>(iniConfig.get<uint32_t>(cGeneralHandednessOfTrafficTypeKey,
                                                                                              static_cast<uint32_t>(HandednessOfTrafficType::LEFT_HAND_DRIVE)));
        showClock_ = iniConfig.get<bool>(cGeneralShowClockKey, true);
        showBigClock_ = iniConfig.get<bool>(cGeneralShowBigClockKey, false);
        oldGUI_ = iniConfig.get<bool>(cGeneralOldGUIKey, false);
        alphaTrans_ = iniConfig.get<size_t>(cGeneralAlphaTransKey, 50);
        hideMenuToggle_ = iniConfig.get<bool>(cGeneralHideMenuToggleKey, false);
        hideAlpha_ = iniConfig.get<bool>(cGeneralHideAlphaKey, false);
        showLux_ = iniConfig.get<bool>(cGeneralShowLuxKey, false);
        showCursor_ = iniConfig.get<bool>(cGeneralShowCursorKey, false);
        hideBrightnessControl_ = iniConfig.get<bool>(cGeneralHideBrightnessControlKey, false);
        hideWarning_ = iniConfig.get<bool>(cGeneralHideWarningKey, false);
        showNetworkinfo_ = iniConfig.get<bool>(cGeneralShowNetworkinfoKey, false);
        mp3MasterPath_ = iniConfig.get<std::string>(cGeneralMp3MasterPathKey, "/media/MYMEDIA");
        mp3SubFolder_ = iniConfig.get<std::string>(cGeneralMp3SubFolderKey, "/");
        mp3Track_ = iniConfig.get<size_t>(cGeneralMp3TrackKey, 0);
        mp3AutoPlay_ = iniConfig.get<bool>(cGeneralMp3AutoPlayKey, false);
        showAutoPlay_ = iniConfig.get<bool>(cGeneralShowAutoPlayKey, false);
        instantPlay_ = iniConfig.get<bool>(cGeneralInstantPlayKey, false);

        videoFPS_ = static_cast<aasdk::proto::enums::VideoFPS::Enum>(iniConfig.get<uint32_t>(cVideoFPSKey,
                                                                                             aasdk::proto::enums::VideoFPS::_30));

        videoResolution_ = static_cast<aasdk::proto::enums::VideoResolution::Enum>(iniConfig.get<uint32_t>(cVideoResolutionKey,
                                                                                                           aasdk::proto::enums::VideoResolution::_480p));
        screenDPI_ = iniConfig.get<size_t>(cVideoScreenDPIKey, 140);

        omxLayerIndex_ = iniConfig.get<int32_t>(cVideoOMXLayerIndexKey, 1);
        videoMargins_ = QRect(0, 0, iniConfig.get<int32_t>(cVideoMarginWidth, 0), iniConfig.get<int32_t>(cVideoMarginHeight, 0));

        enableTouchscreen_ = iniConfig.get<bool>(cInputEnableTouchscreenKey, true);
        enablePlayerControl_ = iniConfig.get<bool>(cInputEnablePlayerControlKey, false);
        this->readButtonCodes(iniConfig);

   


        musicAudioChannelEnabled_ = iniConfig.get<bool>(cAudioMusicAudioChannelEnabled, true);
        speechAudiochannelEnabled_ = iniConfig.get<bool>(cAudioSpeechAudioChannelEnabled, true);
        audioOutputBackendType_ = static_cast<AudioOutputBackendType>(iniConfig.get<uint32_t>(cAudioOutputBackendType, static_cast<uint32_t>(AudioOutputBackendType::RTAUDIO)));
    }
    catch(const boost::property_tree::ini_parser_error& e)
    {
        OPENAUTO_LOG(warning) << "[Configuration] failed to read configuration file: " << cConfigFileName
                            << ", error: " << e.what()
                            << ". Using default configuration.";
        this->reset();
    }
}

void Configuration::reset()
{
    handednessOfTrafficType_ = HandednessOfTrafficType::LEFT_HAND_DRIVE;
    showClock_ = true;
    showBigClock_ = false;
    oldGUI_ = false;
    alphaTrans_ = 50;
    hideMenuToggle_ = false;
    hideAlpha_ = false;
    showLux_ = false;
    showCursor_ = false;
    hideBrightnessControl_ = false;
    hideWarning_ = false;
    showNetworkinfo_ = false;
    mp3MasterPath_ = "/media/MYMEDIA";
    mp3SubFolder_ = "/";
    mp3Track_ = 0;
    mp3AutoPlay_ = false;
    showAutoPlay_ = false;
    instantPlay_ = false;
    videoFPS_ = aasdk::proto::enums::VideoFPS::_30;
    videoResolution_ = aasdk::proto::enums::VideoResolution::_480p;
    screenDPI_ = 140;
    omxLayerIndex_ = 1;
    videoMargins_ = QRect(0, 0, 0, 0);
    enableTouchscreen_ = true;
    enablePlayerControl_ = false;
    buttonCodes_.clear();

    musicAudioChannelEnabled_ = true;
    speechAudiochannelEnabled_ = true;
    audioOutputBackendType_ = AudioOutputBackendType::QT;
}

void Configuration::save()
{
    boost::property_tree::ptree iniConfig;
    iniConfig.put<uint32_t>(cGeneralHandednessOfTrafficTypeKey, static_cast<uint32_t>(handednessOfTrafficType_));

    iniConfig.put<bool>(cGeneralShowClockKey, showClock_);
    iniConfig.put<bool>(cGeneralShowBigClockKey, showBigClock_);
    iniConfig.put<bool>(cGeneralOldGUIKey, oldGUI_);
    iniConfig.put<size_t>(cGeneralAlphaTransKey, alphaTrans_);
    iniConfig.put<bool>(cGeneralHideMenuToggleKey, hideMenuToggle_);
    iniConfig.put<bool>(cGeneralHideAlphaKey, hideAlpha_);
    iniConfig.put<bool>(cGeneralShowLuxKey, showLux_);
    iniConfig.put<bool>(cGeneralShowCursorKey, showCursor_);
    iniConfig.put<bool>(cGeneralHideBrightnessControlKey, hideBrightnessControl_);
    iniConfig.put<bool>(cGeneralHideWarningKey, hideWarning_);
    iniConfig.put<bool>(cGeneralShowNetworkinfoKey, showNetworkinfo_);
    iniConfig.put<std::string>(cGeneralMp3MasterPathKey, mp3MasterPath_);
    iniConfig.put<std::string>(cGeneralMp3SubFolderKey, mp3SubFolder_);
    iniConfig.put<int32_t>(cGeneralMp3TrackKey, mp3Track_);
    iniConfig.put<bool>(cGeneralMp3AutoPlayKey, mp3AutoPlay_);
    iniConfig.put<bool>(cGeneralShowAutoPlayKey, showAutoPlay_);
    iniConfig.put<bool>(cGeneralInstantPlayKey, instantPlay_);

    iniConfig.put<uint32_t>(cVideoFPSKey, static_cast<uint32_t>(videoFPS_));
    iniConfig.put<uint32_t>(cVideoResolutionKey, static_cast<uint32_t>(videoResolution_));
    iniConfig.put<size_t>(cVideoScreenDPIKey, screenDPI_);
    iniConfig.put<int32_t>(cVideoOMXLayerIndexKey, omxLayerIndex_);
    iniConfig.put<uint32_t>(cVideoMarginWidth, videoMargins_.width());
    iniConfig.put<uint32_t>(cVideoMarginHeight, videoMargins_.height());

    iniConfig.put<bool>(cInputEnableTouchscreenKey, enableTouchscreen_);
    iniConfig.put<bool>(cInputEnablePlayerControlKey, enablePlayerControl_);
    this->writeButtonCodes(iniConfig);


    iniConfig.put<bool>(cAudioMusicAudioChannelEnabled, musicAudioChannelEnabled_);
    iniConfig.put<bool>(cAudioSpeechAudioChannelEnabled, speechAudiochannelEnabled_);
    iniConfig.put<uint32_t>(cAudioOutputBackendType, static_cast<uint32_t>(audioOutputBackendType_));
    boost::property_tree::ini_parser::write_ini(cConfigFileName, iniConfig);
}

bool Configuration::hasTouchScreen() const
{
    auto touchdevs = QTouchDevice::devices();

    OPENAUTO_LOG(info) << "[Touchdev] " <<
                          "Querying available touch devices [" <<
                          touchdevs.length() << " available]";

    for (int i = 0; i < touchdevs.length(); i++) {
        if (touchdevs[i]->type() == QTouchDevice::TouchScreen) {
            OPENAUTO_LOG(info) << "[Touchdev] Device " << i <<
                                  ": " << touchdevs[i]->name().toStdString() <<
                                  ", type " << touchdevs[i]->type();
            return true;
        }
    }
    return false;
}

void Configuration::setHandednessOfTrafficType(HandednessOfTrafficType value)
{
    handednessOfTrafficType_ = value;
}

HandednessOfTrafficType Configuration::getHandednessOfTrafficType() const
{
    return handednessOfTrafficType_;
}

void Configuration::showClock(bool value)
{
    showClock_ = value;
}

bool Configuration::showClock() const
{
    return showClock_;
}

void Configuration::showBigClock(bool value)
{
    showBigClock_ = value;
}

bool Configuration::showBigClock() const
{
    return showBigClock_;
}

void Configuration::oldGUI(bool value)
{
    oldGUI_ = value;
}

bool Configuration::oldGUI() const
{
    return oldGUI_;
}

size_t Configuration::getAlphaTrans() const
{
    return alphaTrans_;
}

void Configuration::setAlphaTrans(size_t value)
{
    alphaTrans_ = value;
}

void Configuration::hideMenuToggle(bool value)
{
    hideMenuToggle_ = value;
}

bool Configuration::hideMenuToggle() const
{
    return hideMenuToggle_;
}

void Configuration::hideAlpha(bool value)
{
    hideAlpha_ = value;
}

bool Configuration::hideAlpha() const
{
    return hideAlpha_;
}

void Configuration::showLux(bool value)
{
    showLux_ = value;
}

bool Configuration::showLux() const
{
    return showLux_;
}

void Configuration::showCursor(bool value)
{
    showCursor_ = value;
}

bool Configuration::showCursor() const
{
    return showCursor_;
}

void Configuration::hideBrightnessControl(bool value)
{
    hideBrightnessControl_ = value;
}

bool Configuration::hideBrightnessControl() const
{
    return hideBrightnessControl_;
}

void Configuration::hideWarning(bool value)
{
    hideWarning_ = value;
}

bool Configuration::hideWarning() const
{
    return hideWarning_;
}

void Configuration::showNetworkinfo(bool value)
{
    showNetworkinfo_ = value;
}

bool Configuration::showNetworkinfo() const
{
    return showNetworkinfo_;
}

std::string Configuration::getMp3MasterPath() const
{
    return mp3MasterPath_;
}

void Configuration::setMp3MasterPath(const std::string& value)
{
    mp3MasterPath_ = value;
}

std::string Configuration::getMp3SubFolder() const
{
    return mp3SubFolder_;
}

void Configuration::setMp3Track(int32_t value)
{
    mp3Track_ = value;
}

void Configuration::setMp3SubFolder(const std::string& value)
{
    mp3SubFolder_ = value;
}

int32_t Configuration::getMp3Track() const
{
    return mp3Track_;
}

void Configuration::mp3AutoPlay(bool value)
{
    mp3AutoPlay_ = value;
}

bool Configuration::mp3AutoPlay() const
{
    return mp3AutoPlay_;
}

void Configuration::showAutoPlay(bool value)
{
    showAutoPlay_ = value;
}

bool Configuration::showAutoPlay() const
{
    return showAutoPlay_;
}

void Configuration::instantPlay(bool value)
{
    instantPlay_ = value;
}

bool Configuration::instantPlay() const
{
    return instantPlay_;
}

aasdk::proto::enums::VideoFPS::Enum Configuration::getVideoFPS() const
{
    return videoFPS_;
}

void Configuration::setVideoFPS(aasdk::proto::enums::VideoFPS::Enum value)
{
    videoFPS_ = value;
}

aasdk::proto::enums::VideoResolution::Enum Configuration::getVideoResolution() const
{
    return videoResolution_;
}

void Configuration::setVideoResolution(aasdk::proto::enums::VideoResolution::Enum value)
{
    videoResolution_ = value;
}

size_t Configuration::getScreenDPI() const
{
    return screenDPI_;
}

void Configuration::setScreenDPI(size_t value)
{
    screenDPI_ = value;
}

void Configuration::setOMXLayerIndex(int32_t value)
{
    omxLayerIndex_ = value;
}

int32_t Configuration::getOMXLayerIndex() const
{
    return omxLayerIndex_;
}

void Configuration::setVideoMargins(QRect value)
{
    videoMargins_ = value;
}

QRect Configuration::getVideoMargins() const
{
    return videoMargins_;
}

bool Configuration::getTouchscreenEnabled() const
{
    return enableTouchscreen_;
}

void Configuration::setTouchscreenEnabled(bool value)
{
    enableTouchscreen_ = value;
}

bool Configuration::playerButtonControl() const
{
    return enablePlayerControl_;
}

void Configuration::playerButtonControl(bool value)
{
    enablePlayerControl_ = value;
}

Configuration::ButtonCodes Configuration::getButtonCodes() const
{
    return buttonCodes_;
}

void Configuration::setButtonCodes(const ButtonCodes& value)
{
    buttonCodes_ = value;
}



bool Configuration::musicAudioChannelEnabled() const
{
    return musicAudioChannelEnabled_;
}

void Configuration::setMusicAudioChannelEnabled(bool value)
{
    musicAudioChannelEnabled_ = value;
}

bool Configuration::speechAudioChannelEnabled() const
{
    return speechAudiochannelEnabled_;
}

void Configuration::setSpeechAudioChannelEnabled(bool value)
{
    speechAudiochannelEnabled_ = value;
}

AudioOutputBackendType Configuration::getAudioOutputBackendType() const
{
    return audioOutputBackendType_;
}

void Configuration::setAudioOutputBackendType(AudioOutputBackendType value)
{
    audioOutputBackendType_ = value;
}

QString Configuration::getCSValue(QString searchString) const
{
    using namespace std;
    ifstream inFile;
    ifstream inFile2;
    string line;
    searchString = searchString.append("=");
    inFile.open("/boot/crankshaft/crankshaft_env.sh");
    inFile2.open("/opt/crankshaft/crankshaft_default_env.sh");

    size_t pos;

    if(inFile) {
        while(inFile.good())
        {
            getline(inFile,line); // get line from file
            if (line[0] != '#') {
                pos=line.find(searchString.toStdString()); // search
                if(pos!=std::string::npos) // string::npos is returned if string is not found
                {
                    int equalPosition = line.find("=");
                    QString value = line.substr(equalPosition + 1).c_str();
                    value.replace("\"","");
                    OPENAUTO_LOG(info) << "[Configuration] CS param found: " << searchString.toStdString() << " Value:" << value.toStdString();
                    return value;
                }
            }
        }
        OPENAUTO_LOG(warning) << "[Configuration] unable to find cs param: " << searchString.toStdString();
        OPENAUTO_LOG(warning) << "[Configuration] Fallback to /opt/crankshaft/crankshaft_default_env.sh)";
        while(inFile2.good())
        {
            getline(inFile2,line); // get line from file
            if (line[0] != '#') {
                pos=line.find(searchString.toStdString()); // search
                if(pos!=std::string::npos) // string::npos is returned if string is not found
                {
                    int equalPosition = line.find("=");
                    QString value = line.substr(equalPosition + 1).c_str();
                    value.replace("\"","");
                    OPENAUTO_LOG(info) << "[Configuration] CS param found: " << searchString.toStdString() << " Value:" << value.toStdString();
                    return value;
                }
            }
        }
        return "";
    } else {
        OPENAUTO_LOG(warning) << "[Configuration] unable to open cs param file (/boot/crankshaft/crankshaft_env.sh)";
        OPENAUTO_LOG(warning) << "[Configuration] Fallback to /opt/crankshaft/crankshaft_default_env.sh)";

        while(inFile2.good())
        {
            getline(inFile2,line); // get line from file
            if (line[0] != '#') {
                pos=line.find(searchString.toStdString()); // search
                if(pos!=std::string::npos) // string::npos is returned if string is not found
                {
                    int equalPosition = line.find("=");
                    QString value = line.substr(equalPosition + 1).c_str();
                    value.replace("\"","");
                    OPENAUTO_LOG(info) << "[Configuration] CS param found: " << searchString.toStdString() << " Value:" << value.toStdString();
                    return value;
                }
            }
        }
        return "";
    }
}

QString Configuration::getParamFromFile(QString fileName, QString searchString) const
{
    OPENAUTO_LOG(info) << "[Configuration] Request param from file: " << fileName.toStdString() << " param: " << searchString.toStdString();
    using namespace std;
    ifstream inFile;
    string line;
    if (!searchString.contains("dtoverlay")) {
        searchString = searchString.append("=");
    }
    inFile.open(fileName.toStdString());

    size_t pos;

    if(inFile) {
        while(inFile.good())
        {
            getline(inFile,line); // get line from file
            if (line[0] != '#') {
                pos=line.find(searchString.toStdString()); // search
                if(pos!=std::string::npos) // string::npos is returned if string is not found
                {
                    int equalPosition = line.find("=");
                    QString value = line.substr(equalPosition + 1).c_str();
                    value.replace("\"","");
                    OPENAUTO_LOG(info) << "[Configuration] Param from file: " << fileName.toStdString() << " found: " << searchString.toStdString() << " Value:" << value.toStdString();
                    return value;
                }
            }
        }
        return "";
    } else {
        return "";
    }
}

QString Configuration::readFileContent(QString fileName) const
{
    using namespace std;
    ifstream inFile;
    string line;
    inFile.open(fileName.toStdString());
    string result = "";
    if(inFile) {
        while(inFile.good())
        {
            getline(inFile,line); // get line from file
            result.append(line);
        }
        return result.c_str();
    } else {
        return "";
    }
}

void Configuration::readButtonCodes(boost::property_tree::ptree& iniConfig)
{
    this->insertButtonCode(iniConfig, cInputPlayButtonKey, aasdk::proto::enums::ButtonCode::PLAY);
    this->insertButtonCode(iniConfig, cInputPauseButtonKey, aasdk::proto::enums::ButtonCode::PAUSE);
    this->insertButtonCode(iniConfig, cInputTogglePlayButtonKey, aasdk::proto::enums::ButtonCode::TOGGLE_PLAY);
    this->insertButtonCode(iniConfig, cInputNextTrackButtonKey, aasdk::proto::enums::ButtonCode::NEXT);
    this->insertButtonCode(iniConfig, cInputPreviousTrackButtonKey, aasdk::proto::enums::ButtonCode::PREV);
    this->insertButtonCode(iniConfig, cInputHomeButtonKey, aasdk::proto::enums::ButtonCode::HOME);
    this->insertButtonCode(iniConfig, cInputCallEndButtonKey, aasdk::proto::enums::ButtonCode::CALL_END);
    this->insertButtonCode(iniConfig, cInputVoiceCommandButtonKey, aasdk::proto::enums::ButtonCode::MICROPHONE_1);
    this->insertButtonCode(iniConfig, cInputLeftButtonKey, aasdk::proto::enums::ButtonCode::LEFT);
    this->insertButtonCode(iniConfig, cInputRightButtonKey, aasdk::proto::enums::ButtonCode::RIGHT);
    this->insertButtonCode(iniConfig, cInputUpButtonKey, aasdk::proto::enums::ButtonCode::UP);
    this->insertButtonCode(iniConfig, cInputDownButtonKey, aasdk::proto::enums::ButtonCode::DOWN);
    this->insertButtonCode(iniConfig, cInputScrollWheelButtonKey, aasdk::proto::enums::ButtonCode::SCROLL_WHEEL);
    this->insertButtonCode(iniConfig, cInputBackButtonKey, aasdk::proto::enums::ButtonCode::BACK);
    this->insertButtonCode(iniConfig, cInputEnterButtonKey, aasdk::proto::enums::ButtonCode::ENTER);
    this->insertButtonCode(iniConfig, cInputNavButtonKey, aasdk::proto::enums::ButtonCode::NAVIGATION);
}

void Configuration::insertButtonCode(boost::property_tree::ptree& iniConfig, const std::string& buttonCodeKey, aasdk::proto::enums::ButtonCode::Enum buttonCode)
{
    if(iniConfig.get<bool>(buttonCodeKey, false))
    {
        buttonCodes_.push_back(buttonCode);
    }
}

void Configuration::writeButtonCodes(boost::property_tree::ptree& iniConfig)
{
    iniConfig.put<bool>(cInputPlayButtonKey, std::find(buttonCodes_.begin(), buttonCodes_.end(), aasdk::proto::enums::ButtonCode::PLAY) != buttonCodes_.end());
    iniConfig.put<bool>(cInputPauseButtonKey, std::find(buttonCodes_.begin(), buttonCodes_.end(), aasdk::proto::enums::ButtonCode::PAUSE) != buttonCodes_.end());
    iniConfig.put<bool>(cInputTogglePlayButtonKey, std::find(buttonCodes_.begin(), buttonCodes_.end(), aasdk::proto::enums::ButtonCode::TOGGLE_PLAY) != buttonCodes_.end());
    iniConfig.put<bool>(cInputNextTrackButtonKey, std::find(buttonCodes_.begin(), buttonCodes_.end(), aasdk::proto::enums::ButtonCode::NEXT) != buttonCodes_.end());
    iniConfig.put<bool>(cInputPreviousTrackButtonKey, std::find(buttonCodes_.begin(), buttonCodes_.end(), aasdk::proto::enums::ButtonCode::PREV) != buttonCodes_.end());
    iniConfig.put<bool>(cInputHomeButtonKey, std::find(buttonCodes_.begin(), buttonCodes_.end(), aasdk::proto::enums::ButtonCode::HOME) != buttonCodes_.end());
    iniConfig.put<bool>(cInputCallEndButtonKey, std::find(buttonCodes_.begin(), buttonCodes_.end(), aasdk::proto::enums::ButtonCode::CALL_END) != buttonCodes_.end());
    iniConfig.put<bool>(cInputVoiceCommandButtonKey, std::find(buttonCodes_.begin(), buttonCodes_.end(), aasdk::proto::enums::ButtonCode::MICROPHONE_1) != buttonCodes_.end());
    iniConfig.put<bool>(cInputLeftButtonKey, std::find(buttonCodes_.begin(), buttonCodes_.end(), aasdk::proto::enums::ButtonCode::LEFT) != buttonCodes_.end());
    iniConfig.put<bool>(cInputRightButtonKey, std::find(buttonCodes_.begin(), buttonCodes_.end(), aasdk::proto::enums::ButtonCode::RIGHT) != buttonCodes_.end());
    iniConfig.put<bool>(cInputUpButtonKey, std::find(buttonCodes_.begin(), buttonCodes_.end(), aasdk::proto::enums::ButtonCode::UP) != buttonCodes_.end());
    iniConfig.put<bool>(cInputDownButtonKey, std::find(buttonCodes_.begin(), buttonCodes_.end(), aasdk::proto::enums::ButtonCode::DOWN) != buttonCodes_.end());
    iniConfig.put<bool>(cInputScrollWheelButtonKey, std::find(buttonCodes_.begin(), buttonCodes_.end(), aasdk::proto::enums::ButtonCode::SCROLL_WHEEL) != buttonCodes_.end());
    iniConfig.put<bool>(cInputBackButtonKey, std::find(buttonCodes_.begin(), buttonCodes_.end(), aasdk::proto::enums::ButtonCode::BACK) != buttonCodes_.end());
    iniConfig.put<bool>(cInputEnterButtonKey, std::find(buttonCodes_.begin(), buttonCodes_.end(), aasdk::proto::enums::ButtonCode::ENTER) != buttonCodes_.end());
    iniConfig.put<bool>(cInputNavButtonKey, std::find(buttonCodes_.begin(), buttonCodes_.end(), aasdk::proto::enums::ButtonCode::NAVIGATION) != buttonCodes_.end());
}

}
}
}
}
