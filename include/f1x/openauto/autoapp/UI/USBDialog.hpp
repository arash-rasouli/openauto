#pragma once

#include <QDialog>

namespace Ui {
class USBDialog;
}

namespace f1x
{
namespace openauto
{
namespace autoapp
{
namespace ui
{

class USBDialog : public QDialog
{
    Q_OBJECT

public:
    explicit USBDialog(QWidget *parent = nullptr);
    ~USBDialog() override;

    Ui::USBDialog *ui_;

private slots:
    void scanDrives();
    void on_pushButtonMount_clicked();
    void on_pushButtonRemove_clicked();
};

}
}
}
}
