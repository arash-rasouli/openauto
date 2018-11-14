#include <f1x/openauto/autoapp/UI/WifiDialog.hpp>
#include <ui_wifidialog.h>

namespace f1x
{
namespace openauto
{
namespace autoapp
{
namespace ui
{

WifiDialog::WifiDialog(QWidget *parent)
    : QDialog(parent)
    , ui_(new Ui::WifiDialog)
{
    ui_->setupUi(this);

    findTimer = new QTimer();
    findTimer->setInterval(1000);
    connect(findTimer,&QTimer::timeout,this,&WifiDialog::findActiveWirelesses);
    connect(ui_->pushButtonClose, &QPushButton::clicked, this, &WifiDialog::close);
    findTimer->start();
    connected = false;
    foundCount = 0;
    ui_->treeWidgetWiFis->setColumnWidth(0,50);
    ui_->treeWidgetWiFis->setColumnWidth(1,200);
    findActiveWirelesses();
}

WifiDialog::~WifiDialog()
{
    delete ui_;
}

}
}
}
}

void f1x::openauto::autoapp::ui::WifiDialog::findActiveWirelesses()
{
    QNetworkConfigurationManager *ncm = new QNetworkConfigurationManager();
    connect(ncm, &QNetworkConfigurationManager::onlineStateChanged, this, &WifiDialog::configurationStateChanged);

    netcfgList = ncm->allConfigurations();
    WiFisList.clear();
    for (auto &x : netcfgList)
    {
        if (x.bearerType() == QNetworkConfiguration::BearerWLAN)
        {
            if(x.name() == "")
                WiFisList << "Unknown(Other Network)";
            else
                WiFisList << x.name();

            qDebug() << x.type();
        }
    }
    for(int i=0; i<WiFisList.size(); i++)
    {
        bool exist = false;
        QTreeWidgetItem * item = new QTreeWidgetItem();
        for(int j=0; j<ui_->treeWidgetWiFis->topLevelItemCount(); j++)
        {
            QTreeWidgetItem *index = ui_->treeWidgetWiFis->topLevelItem(j);
            QString str = index->text(1);
            if(str == WiFisList[i])
            {
                exist = true;
                break;
            }
        }
        if(!exist)
        {
            item->setTextAlignment(0,Qt::AlignVCenter);
            item->setTextAlignment(1,Qt::AlignHCenter);
            item->setText(0,QString::number(++foundCount));
            item->setText(1,WiFisList[i]);
            ui_->treeWidgetWiFis->addTopLevelItem(item);
        }
    }
}

void f1x::openauto::autoapp::ui::WifiDialog::connectionStatusOpened()
{
    connected = true;
    ui_->btnConnect->setText("Disconnect");
    statusBarMessage("Successfully Connected.","green");
}

void f1x::openauto::autoapp::ui::WifiDialog::connectionStatusClosed()
{
    connected = false;
    ui_->btnConnect->setText("Connect");
    statusBarMessage("Successfully Disonnected.","red");
}

void f1x::openauto::autoapp::ui::WifiDialog::on_btnConnect_clicked()
{
    if(!connected)
    {
        QString pass = QInputDialog::getText(this, "Password", "Enter Password:",QLineEdit::Password);
        if(pass.isEmpty()) return;

        QModelIndex index = ui_->treeWidgetWiFis->currentIndex();
        QString str = index.data(Qt::DisplayRole).toString();
        for (auto &x : netcfgList)
        {
            if (x.bearerType() == QNetworkConfiguration::BearerWLAN)
            {
                if (x.name() == str)
                {
                    netcfg = x;
                    session = new QNetworkSession(netcfg, this);
                    connect(session,&QNetworkSession::opened,this,&WifiDialog::connectionStatusOpened);
                    connect(session,&QNetworkSession::closed,this,&WifiDialog::connectionStatusClosed);
                    session->open();
                    if(session->isOpen())
                    {
                        connected = true;
                        ui_->btnConnect->setText("Disconnect");
                    }
                }
            }
        }
    }
    else
    {
        session->close();
        if(!session->isOpen())
        {
            connected = false;
            ui_->btnConnect->setText("Connect");
        }
    }
}

void f1x::openauto::autoapp::ui::WifiDialog::statusBarMessage(QString msg, QString color)
{
    ui_->statusBar->clearMessage();
    ui_->statusBar->setStyleSheet("color:"+color+";");
    ui_->statusBar->showMessage(msg, 5000);
}

void f1x::openauto::autoapp::ui::WifiDialog::configurationStateChanged()
{
    qDebug()<< "emited";
}
