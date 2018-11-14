#include <QDialog>
#include <QTimer>
#include <QList>
#include <QInputDialog>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QNetworkConfiguration>
#include <QNetworkConfigurationManager>
#include <QNetworkSession>

namespace Ui {
class WifiDialog;
}

namespace f1x
{
namespace openauto
{
namespace autoapp
{
namespace ui
{

class WifiDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WifiDialog(QWidget *parent = nullptr);
    ~WifiDialog() override;

    bool connected;
    int foundCount;
    QNetworkConfiguration netcfg;
    QStringList WiFisList;
    QList<QNetworkConfiguration> netcfgList;

    Ui::WifiDialog* ui_;

public slots:
    void findActiveWirelesses();
    void connectionStatusOpened();
    void connectionStatusClosed();
    void statusBarMessage(QString msg, QString color);
    void configurationStateChanged();

private slots:
    void on_btnConnect_clicked();

private:
    QTimer *findTimer;
    QStandardItemModel* listModel;
    QNetworkSession *session;
};

}
}
}
}
