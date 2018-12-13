#include <f1x/openauto/autoapp/UI/Warning.hpp>
#include <ui_warning.h>
#include <QTimer>

namespace f1x
{
namespace openauto
{
namespace autoapp
{
namespace ui
{

Warning::Warning(QWidget *parent)
    : QDialog(parent)
    , ui_(new Ui::Warning)
{
    ui_->setupUi(this);

    connect(ui_->pushButtonClose, &QPushButton::clicked, this, &Warning::close);
    QTimer::singleShot(5000, this, SLOT(Autoclose()));
}

Warning::~Warning()
{
    delete ui_;
}

void Warning::Autoclose()
{
    Warning::close();
}

}
}
}
}
