#ifndef MESSAGE_H
#define MESSAGE_H

#include <QDialog>


namespace Ui {
    class Message;
}

class Message : public QDialog
{
    Q_OBJECT

public:

    explicit Message(QWidget *parent = nullptr);

    ~Message();

    void setMessage(const QString &_message);

private slots:

    void on_buttonApply_clicked();

private:

    Ui::Message *ui;
};

#endif // MESSAGE_H
