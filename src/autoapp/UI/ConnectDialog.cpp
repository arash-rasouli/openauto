#include <QMessageBox>
#include <f1x/openauto/autoapp/UI/ConnectDialog.hpp>
#include "ui_connectdialog.h"
#include <QFileInfo>
#include <QTextStream>

namespace f1x
{
namespace openauto
{
namespace autoapp
{
namespace ui
{

ConnectDialog::ConnectDialog(boost::asio::io_service& ioService, aasdk::tcp::ITCPWrapper& tcpWrapper, openauto::autoapp::configuration::IRecentAddressesList& recentAddressesList, QWidget *parent)
    : QDialog(parent)
    , ioService_(ioService)
    , tcpWrapper_(tcpWrapper)
    , recentAddressesList_(recentAddressesList)
    , ui_(new Ui::ConnectDialog)
{
    qRegisterMetaType<aasdk::tcp::ITCPEndpoint::SocketPointer>("aasdk::tcp::ITCPEndpoint::SocketPointer");
    qRegisterMetaType<std::string>("std::string");

    ui_->setupUi(this);
    connect(ui_->pushButtonCancel, &QPushButton::clicked, this, &ConnectDialog::close);
    connect(ui_->pushButtonConnect, &QPushButton::clicked, this, &ConnectDialog::onConnectButtonClicked);
    connect(ui_->listViewRecent, &QListView::clicked, this, &ConnectDialog::onRecentAddressClicked);
    connect(this, &ConnectDialog::connectionSucceed, this, &ConnectDialog::onConnectionSucceed);
    connect(this, &ConnectDialog::connectionFailed, this, &ConnectDialog::onConnectionFailed);
    connect(ui_->pushButtonUpdate, &QPushButton::clicked, this, &ConnectDialog::onUpdateButtonClicked);

    ui_->listViewRecent->setModel(&recentAddressesModel_);
    this->loadRecentList();
    loadTempRecentList();

    ui_->progressBarConnect->hide();
    ui_->lineEditIPAddress->setFocus();
}

ConnectDialog::~ConnectDialog()
{
    delete ui_;
}

void ConnectDialog::onConnectButtonClicked()
{
    this->setControlsEnabledStatus(false);

    const auto& ipAddress = ui_->lineEditIPAddress->text().toStdString();
    auto socket = std::make_shared<boost::asio::ip::tcp::socket>(ioService_);
    ui_->progressBarConnect->show();
    try
    {
        tcpWrapper_.asyncConnect(*socket, ipAddress, 5277, std::bind(&ConnectDialog::connectHandler, this, std::placeholders::_1, ipAddress, socket));
    }
    catch(const boost::system::system_error& se)
    {
        emit connectionFailed(QString(se.what()));
    }
    loadTempRecentList();
}

void ConnectDialog::onUpdateButtonClicked()
{
    system("/usr/local/bin/autoapp_helper updaterecent");
    loadTempRecentList();
}

void ConnectDialog::connectHandler(const boost::system::error_code& ec, const std::string& ipAddress, aasdk::tcp::ITCPEndpoint::SocketPointer socket)
{
    if(!ec)
    {
        emit connectionSucceed(std::move(socket), ipAddress);
        this->close();
    }
    else
    {
        emit connectionFailed(QString::fromStdString(ec.message()));
    }
}

void ConnectDialog::onConnectionSucceed(aasdk::tcp::ITCPEndpoint::SocketPointer, const std::string& ipAddress)
{
    ui_->progressBarConnect->hide();
    this->insertIpAddress(ipAddress);
    this->setControlsEnabledStatus(true);
}

void ConnectDialog::onConnectionFailed(const QString& message)
{
    this->setControlsEnabledStatus(true);

    ui_->progressBarConnect->hide();
    QMessageBox errorMessage(QMessageBox::Critical, "Connect error", message, QMessageBox::Ok);
    errorMessage.setWindowFlags(Qt::WindowStaysOnTopHint);
    errorMessage.exec();
}

void ConnectDialog::onRecentAddressClicked(const QModelIndex& index)
{
    const auto& recentAddressesList = recentAddressesList_.getList();

    if(static_cast<size_t>(index.row()) <= recentAddressesList.size())
    {
        ui_->lineEditIPAddress->setText(QString::fromStdString(recentAddressesList.at(index.row())));
    }
}

void ConnectDialog::setControlsEnabledStatus(bool status)
{
    ui_->pushButtonConnect->setVisible(status);
    ui_->pushButtonCancel->setEnabled(status);
    ui_->lineEditIPAddress->setEnabled(status);
    ui_->listViewRecent->setEnabled(status);
}

void ConnectDialog::loadRecentList()
{
    QStringList stringList;
    const auto& configList = recentAddressesList_.getList();

    for(const auto& element : configList)
    {
        stringList.append(QString::fromStdString(element));
    }

    recentAddressesModel_.setStringList(stringList);
}

void ConnectDialog::loadTempRecentList()
{
    QFileInfo recentFile("/tmp/temp_recent_list");
    if (recentFile.exists()) {
        QFile versionFile(QString("/tmp/temp_recent_list"));
        versionFile.open(QIODevice::ReadOnly);
        QTextStream data(&versionFile);
        while (!data.atEnd())
        {
            QString ip = data.readLine();
            ConnectDialog::insertIpAddress(ip.toStdString());
        }
        versionFile.close();
    }
}

void ConnectDialog::insertIpAddress(const std::string& ipAddress)
{
    recentAddressesList_.insertAddress(ipAddress);
    this->loadRecentList();
}

}
}
}
}
