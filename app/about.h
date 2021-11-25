#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>
#include <QDesktopServices>
#include <QUrl>


namespace Ui {
    class About;
}

class About : public QDialog
{
    Q_OBJECT

public:

    explicit About(QWidget *parent = nullptr);

    ~About();

private slots:

    void on_buttonCancel_clicked();

    void on_buttonBitcoin_clicked();

    void on_buttonPayPal_clicked();

private:

    Ui::About *ui;
};

#endif // ABOUT_H
