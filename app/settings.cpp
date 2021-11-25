#include "settings.h"
#include "ui_settings.h"
#include "message.h"


Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
    this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

Settings::~Settings()
{
    delete ui;
}

void Settings::setParemeters(int *method)
{
    _method = method;
    ui->comboBox_method->setCurrentIndex(*_method);
}

void Settings::on_buttonCancel_clicked() // Close settings window
{
    this->close();
}

void Settings::on_buttonReset_clicked() // Reset settings
{
    ui->comboBox_method->setCurrentIndex(0);
}

void Settings::on_buttonApply_clicked() // Save settings
{
    *_method = ui->comboBox_method->currentIndex();
    this->close();
}

void Settings::showMessage(const QString &_message)
{
    Message messageWindow(this);
    messageWindow.setModal(true);
    messageWindow.setMessage(_message);
    messageWindow.exec();
}
