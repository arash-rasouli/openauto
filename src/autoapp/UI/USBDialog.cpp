#include <QMessageBox>
#include <QFileInfo>
#include <QTextStream>
#include <f1x/openauto/autoapp/UI/USBDialog.hpp>
#include <ui_usbdialog.h>
#include <libusb.h>
#include <blkid/blkid.h>

namespace f1x
{
namespace openauto
{
namespace autoapp
{
namespace ui
{

USBDialog::USBDialog(QWidget *parent)
    : QDialog(parent)
    , ui_(new Ui::USBDialog)
{
    ui_->setupUi(this);
    connect(ui_->pushButtonClose, &QPushButton::clicked, this, &USBDialog::close);
    connect(ui_->pushButtonUpdate, &QPushButton::clicked, this, &USBDialog::scanDrives);
    scanDrives();
}

USBDialog::~USBDialog()
{
    delete ui_;
}

}
}
}
}

void f1x::openauto::autoapp::ui::USBDialog::scanDrives()
{
    int currentdevice = 0;
    int cleaner = ui_->listWidgetUSB->count();
    while (cleaner > -1) {
        ui_->listWidgetUSB->takeItem(cleaner);
        cleaner--;
    }
    ui_->listWidgetUSB->clear();

    QFile partitionsFile(QString("/proc/partitions"));
    partitionsFile.open(QIODevice::ReadOnly);
    QTextStream data_return(&partitionsFile);
    QStringList localpartitions;
    localpartitions = data_return.readAll().split("\n");
    partitionsFile.close();

    while (currentdevice < localpartitions.count()-1) {
        QString line = localpartitions[currentdevice].simplified();
        QString device = line.split(" ")[3];

        // filter partition list for drives - not nice but working
        if (!device.startsWith("ram") && !device.startsWith("name") && !device.startsWith("mmc") && device != "") {
            if (!device.endsWith("0") && !device.endsWith("1") && !device.endsWith("2") && !device.endsWith("3") && !device.endsWith("4") && !device.endsWith("5") && !device.endsWith("6") && !device.endsWith("7") && !device.endsWith("8") && !device.endsWith("9")) {
                device = "/dev/" + device;
                const char *dev_base_name = QString(device).toStdString().c_str();
                blkid_probe pr = blkid_new_probe_from_filename(dev_base_name);
                blkid_partlist ls;
                int nparts, i=1;
                ls = blkid_probe_get_partitions(pr);
                nparts = blkid_partlist_numof_partitions(ls);

                const char *label;
                const char *type;
                const char *mounted;

                while (i <= nparts) {
                    QString partitionpath = QString(device) + QString::number(i);
                    const char *dev_name = QString(partitionpath).toStdString().c_str();
                    pr = blkid_new_probe_from_filename(dev_name);
                    blkid_do_probe(pr);
                    blkid_probe_lookup_value(pr, "LABEL", &label, nullptr);
                    blkid_probe_lookup_value(pr, "TYPE", &type, nullptr);
                    // exclude CSSTORAGE cause mounted another way by system service
                    if (QString::fromStdString(label) != "CSSTORAGE") {
                        // check mount state
                        if (system(qPrintable("mount | grep " + partitionpath + " >/dev/null")) != 0) {
                            mounted = "unmounted";
                        } else {
                            mounted = "mounted";
                        }
                        ui_->listWidgetUSB->addItem(QString::fromStdString(dev_name) + " (" + QString::fromStdString(label) + " / " + QString::fromStdString(type) + " | " + QString::fromStdString(mounted) + ")");
                        ui_->listWidgetUSB->update();
                    }
                    i++;
                }
                blkid_free_probe(pr);
            }
        }
        currentdevice++;
    }
    system("/usr/local/bin/autoapp_helper cleansymlinks &");
}

void f1x::openauto::autoapp::ui::USBDialog::on_pushButtonMount_clicked()
{
    QString selected = ui_->listWidgetUSB->item(ui_->listWidgetUSB->currentRow())->text();
    QString mountfulldevicepath = selected.split(" ")[0];
    QString mountdevice = mountfulldevicepath.split("/")[2];
    system(qPrintable("sudo umount " + mountfulldevicepath));
    system(qPrintable("sudo mkdir -p /media/USBDRIVES/" + mountdevice));
    system(qPrintable("sudo chmod 777 /media/USBDRIVES/" + mountdevice));
    system(qPrintable("sudo mount " + mountfulldevicepath + " /media/USBDRIVES/" + mountdevice));
    system(qPrintable("ln -s /media/USBDRIVES/" + mountdevice + "/Music/* /media/MYMEDIA"));
    scanDrives();
}

void f1x::openauto::autoapp::ui::USBDialog::on_pushButtonRemove_clicked()
{
    QString selected = ui_->listWidgetUSB->item(ui_->listWidgetUSB->currentRow())->text();
    QString mountfulldevicepath = selected.split(" ")[0];
    system(qPrintable("sudo umount " + mountfulldevicepath));
    scanDrives();
}
