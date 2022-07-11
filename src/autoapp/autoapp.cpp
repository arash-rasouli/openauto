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
#include <f1x/openauto/autoapp/App.hpp>
#include <f1x/openauto/autoapp/Configuration/IConfiguration.hpp>
#include <f1x/openauto/autoapp/Configuration/RecentAddressesList.hpp>
#include <f1x/openauto/autoapp/Service/AndroidAutoEntityFactory.hpp>
#include <f1x/openauto/autoapp/Service/ServiceFactory.hpp>
#include <f1x/openauto/autoapp/Configuration/Configuration.hpp>
#include <f1x/openauto/autoapp/UI/MainWindow.hpp>
#include <f1x/openauto/autoapp/UI/SettingsWindow.hpp>
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
    //mainWindow.setWindowFlags(Qt::WindowStaysOnTopHint);

    autoapp::ui::SettingsWindow settingsWindow(configuration);
    //settingsWindow.setWindowFlags(Qt::WindowStaysOnTopHint);

    settingsWindow.setFixedSize(width, height);
    settingsWindow.adjustSize();

    autoapp::configuration::RecentAddressesList recentAddressesList(7);
    recentAddressesList.read();

    autoapp::ui::WarningDialog warningdialog;
    //warningdialog.setWindowFlags(Qt::WindowStaysOnTopHint);
    warningdialog.move((width - 500)/2,(height-300)/2);

    autoapp::ui::UpdateDialog updatedialog;
    //updatedialog.setWindowFlags(Qt::WindowStaysOnTopHint);
    updatedialog.setFixedSize(500, 260);
    updatedialog.move((width - 500)/2,(height-260)/2);

    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::exit, []() { system("touch /tmp/shutdown"); std::exit(0); });
    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::reboot, []() { system("touch /tmp/reboot"); std::exit(0); });
    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::openSettings, &settingsWindow, &autoapp::ui::SettingsWindow::showFullScreen);
    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::openSettings, &settingsWindow, &autoapp::ui::SettingsWindow::show_tab1);
    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::openSettings, &settingsWindow, &autoapp::ui::SettingsWindow::loadSystemValues);
    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::openUpdateDialog, &updatedialog, &autoapp::ui::UpdateDialog::updateCheck);
    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::openUpdateDialog, &updatedialog, &autoapp::ui::UpdateDialog::exec);

    if (configuration->showCursor() == false) {
        qApplication.setOverrideCursor(Qt::BlankCursor);
    } else {
        qApplication.setOverrideCursor(Qt::ArrowCursor);
    }

    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::cameraHide, [&qApplication]() {
        system("/opt/crankshaft/cameracontrol.py Background &");
        OPENAUTO_LOG(info) << "[Camera] Background.";
    });

    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::cameraShow, [&qApplication]() {
        system("/opt/crankshaft/cameracontrol.py Foreground &");
        OPENAUTO_LOG(info) << "[Camera] Foreground.";
    });

    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::cameraPosYUp, [&qApplication]() {
        system("/opt/crankshaft/cameracontrol.py PosYUp &");
        OPENAUTO_LOG(info) << "[Camera] PosY up.";
    });

    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::cameraPosYDown, [&qApplication]() {
        system("/opt/crankshaft/cameracontrol.py PosYDown &");
        OPENAUTO_LOG(info) << "[Camera] PosY down.";
    });

    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::cameraZoomPlus, [&qApplication]() {
        system("/opt/crankshaft/cameracontrol.py ZoomPlus &");
        OPENAUTO_LOG(info) << "[Camera] Zoom plus.";
    });

    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::cameraZoomMinus, [&qApplication]() {
        system("/opt/crankshaft/cameracontrol.py ZoomMinus &");
        OPENAUTO_LOG(info) << "[Camera] Zoom minus.";
    });

    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::cameraRecord, [&qApplication]() {
        system("/opt/crankshaft/cameracontrol.py Record &");
        OPENAUTO_LOG(info) << "[Camera] Record.";
    });

    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::cameraStop, [&qApplication]() {
        system("/opt/crankshaft/cameracontrol.py Stop &");
        OPENAUTO_LOG(info) << "[Camera] Stop.";
    });

    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::cameraSave, [&qApplication]() {
        system("/opt/crankshaft/cameracontrol.py Save &");
        OPENAUTO_LOG(info) << "[Camera] Save.";
    });

    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::TriggerScriptNight, [&qApplication]() {
        system("/opt/crankshaft/service_daynight.sh app night");
        OPENAUTO_LOG(info) << "[MainWindow] Night.";
    });

    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::TriggerScriptDay, [&qApplication]() {
        system("/opt/crankshaft/service_daynight.sh app day");
        OPENAUTO_LOG(info) << "[MainWindow] Day.";
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
    auto app = std::make_shared<autoapp::App>(ioService, usbWrapper, androidAutoEntityFactory, std::move(usbHub), std::move(connectedAccessoriesEnumerator));

    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::TriggerAppStart, [&app]() {
        OPENAUTO_LOG(info) << "[Autoapp] TriggerAppStart: Manual start android auto.";
        try {
            app->disableAutostartEntity = false;
            app->resume();
            app->waitForUSBDevice();
        } catch (...) {
            OPENAUTO_LOG(error) << "[Autoapp] TriggerAppStart: app->waitForUSBDevice();";
        }
    });

    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::TriggerAppStop, [&app]() {
        try {
            if (std::ifstream("/tmp/android_device")) {
                OPENAUTO_LOG(info) << "[Autoapp] TriggerAppStop: Manual stop usb android auto.";
                app->disableAutostartEntity = true;
                system("/usr/local/bin/autoapp_helper usbreset");
                usleep(500000);
                try {
                    app->stop();
                    //app->pause();
                } catch (...) {
                    OPENAUTO_LOG(error) << "[Autoapp] TriggerAppStop: stop();";
                }

            }
        } catch (...) {
            OPENAUTO_LOG(info) << "[Autoapp] Exception in manual stop android auto.";
        }
    });

    QObject::connect(&mainWindow, &autoapp::ui::MainWindow::CloseAllDialogs, [&settingsWindow, &updatedialog, &warningdialog]() {
        settingsWindow.close();
        warningdialog.close();
        updatedialog.close();
        OPENAUTO_LOG(info) << "[Autoapp] Close all possible open dialogs.";
    });

    if (configuration->hideWarning() == false) {
        warningdialog.show();
    }

    app->waitForUSBDevice();

    auto result = qApplication.exec();

    std::for_each(threadPool.begin(), threadPool.end(), std::bind(&std::thread::join, std::placeholders::_1));

    libusb_exit(usbContext);
    return result;
}
