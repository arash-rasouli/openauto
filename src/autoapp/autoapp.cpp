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

#include <thread>
#include <QApplication>
#include <QDesktopWidget>
#include <f1x/aasdk/USB/USBHub.hpp>
#include <f1x/aasdk/USB/ConnectedAccessoriesEnumerator.hpp>
#include <f1x/aasdk/USB/AccessoryModeQueryChain.hpp>
#include <f1x/aasdk/USB/AccessoryModeQueryChainFactory.hpp>
#include <f1x/aasdk/USB/AccessoryModeQueryFactory.hpp>
#include <f1x/aasdk/TCP/TCPWrapper.hpp>
#include <f1x/openauto/autoapp/App.hpp>
#include <f1x/openauto/autoapp/Configuration/IConfiguration.hpp>
#include <f1x/openauto/autoapp/Configuration/RecentAddressesList.hpp>
#include <f1x/openauto/autoapp/Service/AndroidAutoEntityFactory.hpp>
#include <f1x/openauto/autoapp/Service/ServiceFactory.hpp>
#include <f1x/openauto/autoapp/Configuration/Configuration.hpp>
#include <f1x/openauto/autoapp/UI/MainWindow.hpp>
#include <f1x/openauto/autoapp/UI/SettingsWindow.hpp>
#include <f1x/openauto/autoapp/UI/ConnectDialog.hpp>
#include <f1x/openauto/autoapp/UI/WarningDialog.hpp>
#include <f1x/openauto/autoapp/UI/UpdateDialog.hpp>
#include <f1x/openauto/Common/Log.hpp>

namespace aasdk = f1x::aasdk;
namespace autoapp = f1x::openauto::autoapp;
using ThreadPool = std::vector<std::thread>;

void startUSBWorkers(boost::asio::io_service& ioService, libusb_context* usbContext, ThreadPool& threadPool)
{
    auto usbWorker = [&ioService, usbContext]() {
        timeval libusbEventTimeout{180, 0};

        while(!ioService.stopped())
        {
            libusb_handle_events_timeout_completed(usbContext, &libusbEventTimeout, nullptr);
        }
    };

    threadPool.emplace_back(usbWorker);
    threadPool.emplace_back(usbWorker);
    threadPool.emplace_back(usbWorker);
    threadPool.emplace_back(usbWorker);
}

void startIOServiceWorkers(boost::asio::io_service& ioService, ThreadPool& threadPool)
{
    auto ioServiceWorker = [&ioService]() {
        ioService.run();
    };

    threadPool.emplace_back(ioServiceWorker);
    threadPool.emplace_back(ioServiceWorker);
    threadPool.emplace_back(ioServiceWorker);
    threadPool.emplace_back(ioServiceWorker);
}

int main(int argc, char* argv[])
{
    libusb_context* usbContext;
    if(libusb_init(&usbContext) != 0)
    {
        OPENAUTO_LOG(error) << "[OpenAuto] libusb init failed.";
        return 1;
    }

    boost::asio::io_service ioService;
    boost::asio::io_service::work work(ioService);
    std::vector<std::thread> threadPool;
    startUSBWorkers(ioService, usbContext, threadPool);
    startIOServiceWorkers(ioService, threadPool);

    QApplication qApplication(argc, argv);
    const int width = QApplication::desktop()->width();
    const int height = QApplication::desktop()->height();
    OPENAUTO_LOG(info) << "[OpenAuto] Display width: " << width;
    OPENAUTO_LOG(info) << "[OpenAuto] Display height: " << height;

    auto configuration = std::make_shared<autoapp::configuration::Configuration>();

    autoapp::ui::MainWindow mainWindow(configuration);
    mainWindow.setWindowFlags(Qt::WindowStaysOnTopHint);

    autoapp::ui::SettingsWindow settingsWindow(configuration);
    settingsWindow.setWindowFlags(Qt::WindowStaysOnTopHint);

    settingsWindow.setFixedSize(width, height);
    settingsWindow.adjustSize();

    autoapp::configuration::RecentAddressesList recentAddressesList(7);
    recentAddressesList.read();

    aasdk::tcp::TCPWrapper tcpWrapper;
    autoapp::ui::ConnectDialog connectdialog(ioService, tcpWrapper, recentAddressesList);
    connectdialog.setWindowFlags(Qt::WindowStaysOnTopHint);
    connectdialog.move((width - 500)/2,(height-300)/2);

    autoapp::ui::WarningDialog warningdialog;
    warningdialog.setWindowFlags(Qt::WindowStaysOnTopHint);
    warningdialog.move((width - 500)/2,(height-300)/2);

    autoapp::ui::UpdateDialog updatedialog;
    updatedialog.setWindowFlags(Qt::WindowStaysOnTopHint);
    updatedialog.setFixedSize(500, 260);
    updatedialog.move((width - 500)/2,(height-260)/2);

    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::exit, []() { system("touch /tmp/shutdown"); std::exit(0); });
    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::reboot, []() { system("touch /tmp/reboot"); std::exit(0); });
    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::openSettings, &settingsWindow, &autoapp::ui::SettingsWindow::loadSystemValues);
    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::openSettings, &settingsWindow, &autoapp::ui::SettingsWindow::showFullScreen);
    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::openConnectDialog, &connectdialog, &autoapp::ui::ConnectDialog::loadClientList);
    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::openConnectDialog, &connectdialog, &autoapp::ui::ConnectDialog::exec);
    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::openUpdateDialog, &updatedialog, &autoapp::ui::UpdateDialog::updateCheck);
    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::openUpdateDialog, &updatedialog, &autoapp::ui::UpdateDialog::exec);

    if (configuration->showCursor() == false) {
        qApplication.setOverrideCursor(Qt::BlankCursor);
    } else {
        qApplication.setOverrideCursor(Qt::ArrowCursor);
    }

    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::cameraHide, [&qApplication]() {
#ifdef RASPBERRYPI3
        system("/opt/crankshaft/cameracontrol.py Background &");
        OPENAUTO_LOG(info) << "[CS] Ran RPiCameraControl script.";
#else
        OPENAUTO_LOG(info) << "[CS] You are not running this on a Raspberry Pi, skipping Cam script.";
#endif
    });

    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::cameraShow, [&qApplication]() {
#ifdef RASPBERRYPI3
        system("/opt/crankshaft/cameracontrol.py Foreground &");
        OPENAUTO_LOG(info) << "[CS] Ran RPiCameraControl script.";
#else
        OPENAUTO_LOG(info) << "[CS] You are not running this on a Raspberry Pi, skipping Cam script.";
#endif
    });

    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::hideRearCam, [&qApplication]() {
#ifdef RASPBERRYPI3
        system("/opt/crankshaft/cameracontrol.py DashcamMode &");
        system("sudo rm /tmp/rearcam_enabled &");
        OPENAUTO_LOG(info) << "[CS] Ran RPiCameraControl script.";
#else
        OPENAUTO_LOG(info) << "[CS] You are not running this on a Raspberry Pi, skipping Cam script.";
#endif
    });

    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::showRearCam, [&qApplication]() {
#ifdef RASPBERRYPI3
        system("/opt/crankshaft/cameracontrol.py Rearcam &");
        OPENAUTO_LOG(info) << "[CS] Ran RPiCameraControl script.";
#else
        OPENAUTO_LOG(info) << "[CS] You are not running this on a Raspberry Pi, skipping Cam script.";
#endif
    });

    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::cameraRecord, [&qApplication]() {
#ifdef RASPBERRYPI3
        system("/opt/crankshaft/cameracontrol.py Record &");
        OPENAUTO_LOG(info) << "[CS] Ran RPiCameraControl script.";
#else
        OPENAUTO_LOG(info) << "[CS] You are not running this on a Raspberry Pi, skipping Cam script.";
#endif
    });

    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::cameraStop, [&qApplication]() {
#ifdef RASPBERRYPI3
        system("/opt/crankshaft/cameracontrol.py Stop &");
        OPENAUTO_LOG(info) << "[CS] Ran RPiCameraControl script.";
#else
        OPENAUTO_LOG(info) << "[CS] You are not running this on a Raspberry Pi, skipping Cam script.";
#endif
    });

    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::cameraSave, [&qApplication]() {
#ifdef RASPBERRYPI3
        system("/opt/crankshaft/cameracontrol.py Save &");
        OPENAUTO_LOG(info) << "[CS] Ran RPiCameraControl script.";
#else
        OPENAUTO_LOG(info) << "[CS] You are not running this on a Raspberry Pi, skipping Cam script.";
#endif
    });

    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::TriggerScriptNight, [&qApplication]() {
#ifdef RASPBERRYPI3
        system("/opt/crankshaft/service_daynight.sh app night");
        OPENAUTO_LOG(info) << "[CS] Run night script.";
#else
        OPENAUTO_LOG(info) << "[CS] You are not running this on a Raspberry Pi, skipping Day/Night script.";
#endif
    });

    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::TriggerScriptDay, [&qApplication]() {
#ifdef RASPBERRYPI3
        system("/opt/crankshaft/service_daynight.sh app day");
        OPENAUTO_LOG(info) << "[CS] Run day script.";
#else
        OPENAUTO_LOG(info) << "[CS] You are not running this on a Raspberry Pi, skipping Day/Night script.";
#endif
    });

    mainWindow.showFullScreen();
    mainWindow.setFixedSize(width, height);
    mainWindow.adjustSize();

    aasdk::usb::USBWrapper usbWrapper(usbContext);
    aasdk::usb::AccessoryModeQueryFactory queryFactory(usbWrapper, ioService);
    aasdk::usb::AccessoryModeQueryChainFactory queryChainFactory(usbWrapper, ioService, queryFactory);
    autoapp::service::ServiceFactory serviceFactory(ioService, configuration);
    autoapp::service::AndroidAutoEntityFactory androidAutoEntityFactory(ioService, configuration, serviceFactory);

    auto usbHub(std::make_shared<aasdk::usb::USBHub>(usbWrapper, ioService, queryChainFactory));
    auto connectedAccessoriesEnumerator(std::make_shared<aasdk::usb::ConnectedAccessoriesEnumerator>(usbWrapper, ioService, queryChainFactory));
    auto app = std::make_shared<autoapp::App>(ioService, usbWrapper, tcpWrapper, androidAutoEntityFactory, std::move(usbHub), std::move(connectedAccessoriesEnumerator));

    QObject::connect(&connectdialog, &autoapp::ui::ConnectDialog::connectionSucceed, [&app](auto socket) {
        app->start(std::move(socket));
    });

    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::TriggerAppStart, [&app]() {
        OPENAUTO_LOG(info) << "[Autoapp] Manual start android auto entity by reset usb.";
        try {
            if (std::ifstream("/tmp/android_device")) {
                //system("/usr/local/bin/autoapp_helper usbreset");
                app->disableAutostartEntity = false;
                app->waitForUSBDevice();
            }
        } catch (...) {
            OPENAUTO_LOG(info) << "[Autoapp] Exception in Manual start android auto entity by reset usb.";
        }
    });

    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::TriggerAppStop, [&app]() {
        OPENAUTO_LOG(info) << "[Autoapp] Manual stop android auto entity.";
        try {
            if (std::ifstream("/tmp/android_device")) {
                app->disableAutostartEntity = true;
                system("/usr/local/bin/autoapp_helper usbreset");
                usleep(500000);
                app->stop();
            } else {
                app->stop();
                //app->onAndroidAutoQuit();
            }
        } catch (...) {
            OPENAUTO_LOG(info) << "[Autoapp] Exception in Manual stop android auto entity.";
        }
    });

    warningdialog.show();

    app->waitForUSBDevice();

    auto result = qApplication.exec();

    std::for_each(threadPool.begin(), threadPool.end(), std::bind(&std::thread::join, std::placeholders::_1));

    libusb_exit(usbContext);
    return result;
}
