#ifndef PROGRESS_H
#define PROGRESS_H

#include <QDialog>
#include <QProcess>
#include <QCloseEvent>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <math.h>


namespace Ui {
    class Progress;
}

class Progress : public QDialog
{
    Q_OBJECT

public:

    explicit Progress(QWidget *parent = nullptr);

    ~Progress();

    void setParameters(QString *message, const QString &input_file, const QString &output_file,
                       const QString &folder_output, const QString &preset, const bool &encodingProcess,
                       float &dur_mod);
private slots:

    void closeEvent(QCloseEvent *event);

    void encoding();

    void extracting();

    void progress_encoding();

    void progress_extracting();

    void complete();

    void error();

private:

    Ui::Progress *ui;

    static constexpr float MAXIMUM_ALLOWED_TIME = 359999.0f;

    QProcess *process;

    bool calling_pr;

    QString _input_file;

    QString _output_file;

    QString _folder_output;

    QString _preset;

    QString *_message;

    float _dur_mod;

    bool _encodingProcess;

    time_t loop_start;

    QString timeConverter(float &time);
};


#endif // PROGRESS_H
