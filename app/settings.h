#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QFile>
#include <QDir>
#include <iostream>


namespace Ui {
    class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:

    explicit Settings(QWidget *parent = nullptr);

    ~Settings();

    void setParemeters(int *method);

private slots:

    void on_buttonCancel_clicked();

    void on_buttonReset_clicked();

    void on_buttonApply_clicked();

private:

    Ui::Settings *ui;

    QString _message;

    int *_method;

    void showMessage(const QString &_message);
};

#endif // SETTINGS_H
