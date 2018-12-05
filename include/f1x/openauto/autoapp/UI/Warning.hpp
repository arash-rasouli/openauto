#include <QDialog>

namespace Ui {
class Warning;
}

namespace f1x
{
namespace openauto
{
namespace autoapp
{
namespace ui
{

class Warning : public QDialog
{
    Q_OBJECT

public:
    explicit Warning(QWidget *parent = nullptr);
    ~Warning() override;

    Ui::Warning* ui_;
};

}
}
}
}
