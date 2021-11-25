#include "progress.h"
#include "ui_progress.h"
#include <QMessageBox>


Progress::Progress(QWidget *parent):
    QDialog(parent),
    ui(new Ui::Progress)
{
    ui->setupUi(this);
    this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

Progress::~Progress()
{
    delete ui;
}

void Progress::closeEvent(QCloseEvent *event)
{
    event->ignore();
    if (process->state() != QProcess::NotRunning) {
        QMessageBox msgBox(this);
        msgBox.setStyleSheet("QLabel{min-width: 200px; min-height: 90px; font-size: 8em;} "
                             "QPushButton{width: 70px; height: 24px; background: #1E2F50; "
                             "border: 1px solid #646464; font-size: 8em;}");
        msgBox.setWindowTitle("Simplest Studio");
        msgBox.setWindowIcon(QIcon(":/resources/icons/64x64/simplest-studio.png"));
        msgBox.setText("Stop encoding?");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        msgBox.setModal(true);
        if (msgBox.exec() == QMessageBox::Ok) {
            process->kill();
            event->accept();
        }
    }
    else {
        event->accept();
    }
}

void Progress::setParameters(QString *message, const QString &input_file,
                             const QString &output_file, const QString &folder_output,  const QString &preset,
                             const bool &encodingProcess, float &dur_mod)
{
    _encodingProcess = encodingProcess;
    _message = message;
    _input_file = input_file;
    _output_file = output_file;
    _folder_output = folder_output;
    _preset = preset;
    _dur_mod = dur_mod;
    ui->progressBar->setValue(0);
    calling_pr = true;
    process = new QProcess(this);
    process->setProcessChannelMode(QProcess::MergedChannels);

    if (_encodingProcess) {
        ui->label_operationType->setText("Encoding");
        connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(progress_encoding()));
        encoding();
    }
    else {
        ui->label_operationType->setText("Extracting");
        connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(progress_extracting()));
        extracting();
    }
}

void Progress::encoding()
{
    connect(process, SIGNAL(finished(int)), this, SLOT(error()));
    loop_start = time (nullptr);
    QStringList arguments;
    arguments << "-hide_banner" << "-i" << _input_file << _preset.split(" ") << "-y" << _output_file;
    process->start("ffmpeg", arguments);
    if (!process->waitForStarted()) {
        std::cout << "cmd command not found!!!" << std::endl;
        disconnect(process, SIGNAL(finished(int)), this, SLOT(error()));
        *_message = "An unknown error occurred!\n Possible FFMPEG not installed.\n";
        this->close();                                    // Exit on error from progress to mainwinow //
    }
}

void Progress::extracting()
{
    connect(process, SIGNAL(finished(int)), this, SLOT(error()));
    loop_start = time (NULL);
    QStringList arguments;
    arguments << _preset.split(" ") << _input_file;
    process->setWorkingDirectory(_folder_output);
    process->start("sacd_extract", arguments);
    if (!process->waitForStarted()) {
        std::cout << "cmd command not found!!!" << std::endl;
        disconnect(process, SIGNAL(finished(int)), this, SLOT(error()));
        *_message = "An unknown error occurred!\n Possible 'sacd_extract' not installed.\n";
        this->close();                                    // Exit on error from extracting to mainwinow //
    }
}

void Progress::progress_encoding()
{
    QString &&line = QString(process->readAllStandardOutput());
    const QString line_mod = line.replace("   ", " ").replace("  ", " ").replace("  ", " ").replace("= ", "=");
    //std::cout << line_mod.toStdString() << std::endl;
    const int &&pos_st = line_mod.indexOf("time=");
    if (pos_st != -1) {
        const time_t &&iter_start = time(nullptr);
        const QString data = line_mod.split("time=").at(1);
        const QString data_mod = data.split(' ').at(0);
        const QStringList &&data_mod_2 = data_mod.split(':');
        const float &&h_cur = data_mod_2.at(0).toFloat();
        const float &&m_cur = data_mod_2.at(1).toFloat();
        const float &&s_cur = data_mod_2.at(2).toFloat();
        const float &&dur = 3600.f*h_cur + 60.f*m_cur  + s_cur;

        const time_t &&timer = iter_start - loop_start;
        const float &&full_time = (timer * _dur_mod) / dur;
        float rem_time = full_time - timer;
        if (rem_time < 0.0f) rem_time = 0.0f;
        if (rem_time > MAXIMUM_ALLOWED_TIME) rem_time = MAXIMUM_ALLOWED_TIME;
        int percent = static_cast<int>(round((dur * 100.f) / _dur_mod));
        if (percent > 100) percent = 100;
        ui->progressBar->setValue(percent);
        ui->label_remaining->setText(timeConverter(rem_time));
        if ((percent >= 50) && (calling_pr)) {
            disconnect(process, SIGNAL(finished(int)), this, SLOT(error()));
            connect(process, SIGNAL(finished(int)), this, SLOT(complete()));
            calling_pr = false;
        }
    }
}

void Progress::progress_extracting()
{
    QString &&line = QString(process->readAllStandardOutput());
    const QString line_mod = line.replace(": ", ":");
    const int &&pos_st = line_mod.indexOf("Total:");
    if (pos_st != -1) {
          const QString data = line_mod.split("Total:").at(1);
          const QString data_mod_str = data.split("%").at(0);
          const int &&data_mod = data_mod_str.toInt();
          const time_t &&iter_start = time(nullptr);
          int timer = iter_start - loop_start;
          const float &&full_time = (timer * 100) / (data_mod + 0.001);
          float rem_time = full_time - timer;
          if (rem_time < 0) rem_time = 0;
          int percent = data_mod;
          if (percent > 100) percent = 100;
          ui->progressBar->setValue(percent);
          ui->label_remaining->setText(timeConverter(rem_time));
          if ((percent >= 50) && (calling_pr == true)) {
              disconnect(process, SIGNAL(finished(int)), this, SLOT(error()));
              connect(process, SIGNAL(finished(int)), this, SLOT(complete_extract()));
              calling_pr = false;
          }
    }
}

void Progress::complete()
{
    disconnect(process, SIGNAL(finished(int)), this, SLOT(complete()));
    *_message = "Completed!\n";
    this->accept();
}

void Progress::error()
{
    disconnect(process, SIGNAL(finished(int)), this, SLOT(error()));
    *_message = "An error occured!\nPossible reasons:\n - incorrect settings or error in the input file,\n - no enough space.";
    this->close();
}

QString Progress::timeConverter(float &time)     /*** Time converter to hh:mm:ss ***/
{
    const int &&h = static_cast<int>(trunc(time / 3600));
    const int &&m = static_cast<int>(trunc((time - (float)(h * 3600)) / 60));
    const int &&s = static_cast<int>(round(time - (float)(h * 3600) - (float)(m * 60)));
    const QString &&hrs = QString::number(h).rightJustified(2, '0');
    const QString &&min = QString::number(m).rightJustified(2, '0');
    const QString &&sec = QString::number(s).rightJustified(2, '0');
    return QString("%1:%2:%3").arg(hrs, min, sec);
}
