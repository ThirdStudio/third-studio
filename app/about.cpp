#include "about.h"
#include "ui_about.h"


About::About(QWidget *parent): QDialog(parent), ui(new Ui::About)
{
    ui->setupUi(this);
    this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

About::~About()
{
    delete ui;
}

void About::on_buttonCancel_clicked()
{
    close();
}

void About::on_buttonPayPal_clicked()
{
    QDesktopServices::openUrl(QUrl("https://paypal.me/KozhukharenkoOleg?locale.x=ru_RU", QUrl::TolerantMode));
}

void About::on_buttonBitcoin_clicked()
{
    QDesktopServices::openUrl(QUrl("https://github.com/SimplestStudio/simplest-studio", QUrl::TolerantMode));
}
