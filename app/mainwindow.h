#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QShowEvent>
#include <QCloseEvent>
#include <QSettings>
#include <iostream>

typedef enum Column{
    FILENAME, STREAM,   FORMAT, SAMPLING, DEPTH,
    BITRATE,  DURATION, PATH,   TECH_DUR
} Column;


QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

    int _method;

    QString _message;

    QString _input_file;

    QString _output_file;

    QString _folder_output;

    QString _fmt;

    int _sampling;

    int _bit_depth;

    int _bitrate;

    float _dur_mod;

    QString _preset;

private slots:

    void showEvent(QShowEvent *event);

    void closeEvent(QCloseEvent *event);

    void on_buttonAdd_clicked();

    void on_buttonRemove_clicked();

    void on_buttonNext_clicked();

    void on_actionSettings_clicked();  

    void on_comboBox_codec_currentTextChanged();

    void make_preset();

    void encode_file();

    void extract_file();

private:

    Ui::MainWindow *ui;

    static const int COLUMNS_COUNT = 9;

    QString _openDir;

    QSettings *_settings;

    bool _windowActivated;

    void on_actionAbout_clicked();

    QString timeConverter(double &time);

    void showMessage(const QString &_message);
};

#endif // MAINWINDOW_H
