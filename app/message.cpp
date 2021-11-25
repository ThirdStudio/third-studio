#include "message.h"
#include "ui_message.h"


Message::Message(QWidget *parent) : QDialog(parent), ui(new Ui::Message)
{
    ui->setupUi(this);
    this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

Message::~Message()
{
    delete ui;
}

void Message::setMessage(const QString &_message)
{    
    ui->textBrowser->clear();
    ui->textBrowser->setAlignment(Qt::AlignCenter);
    ui->textBrowser->append(_message);
    QTextCursor textCursor = ui->textBrowser->textCursor();
    textCursor.movePosition(QTextCursor::Start);
    ui->textBrowser->setTextCursor(textCursor);
}

void Message::on_buttonApply_clicked()
{
    close();
}
