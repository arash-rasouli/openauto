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

#include <f1x/openauto/Common/Log.hpp>
#include <f1x/openauto/autoapp/Service/WifiService.hpp>
#include <fstream>
#include <QString>

namespace f1x
{
namespace openauto
{
namespace autoapp
{
namespace service
{

WifiService::WifiService(configuration::IConfiguration::Pointer configuration)
    : configuration_(std::move(configuration))
{

}

void WifiService::start()
{
}

void WifiService::stop()
{
}

void WifiService::pause()
{
}

void WifiService::resume()
{
}

void WifiService::fillFeatures(aasdk::proto::messages::ServiceDiscoveryResponse& response)
{
    OPENAUTO_LOG(info) << "[WifiService] fill features.";

    auto* channelDescriptor = response.add_channels();
    channelDescriptor->set_channel_id(14);

    auto* channel = channelDescriptor->mutable_wifi_channel();
    channel->set_ssid(configuration_->getParamFromFile("/etc/hostapd/hostapd.conf","ssid").toStdString());
}

}
}
}
}
