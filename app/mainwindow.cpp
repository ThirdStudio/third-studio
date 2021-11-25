#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "about.h"
#include "progress.h"
#include "settings.h"
#include "smartsettings.h"
#include "message.h"


#if defined (Q_OS_UNIX)
    #ifndef UNICODE
        #define UNICODE
    #endif
    #include <unistd.h>
    #include <signal.h>
    #include <MediaInfo/MediaInfo.h>
    using namespace MediaInfoLib;
#elif defined(Q_OS_WIN64)
    #ifdef __MINGW64__
        #ifdef _UNICODE
            #define _itot _itow
        #else
            #define _itot itoa
        #endif
    #endif
    #include <windows.h>
    #include "MediaInfoDLL/MediaInfoDLL.h"
    using namespace MediaInfoDLL;
#endif



MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _windowActivated(false)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    if (_windowActivated == false) {
        _windowActivated = true;
        connect(ui->actionAdd_files, &QAction::triggered, this, &MainWindow::on_buttonAdd_clicked);
        connect(ui->actionRemove_from_list, &QAction::triggered, this, &MainWindow::on_buttonRemove_clicked);
        connect(ui->actionEditSettings, &QAction::triggered, this, &MainWindow::on_actionSettings_clicked);
        connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::on_actionAbout_clicked);

        for (int i = 1; i < 6; i++) ui->tableWidget->resizeColumnToContents(i);
        ui->tableWidget->setRowCount(0);
        ui->tableWidget->hideColumn(Column::TECH_DUR);

        _openDir = QDir::homePath();
        QString _settings_path = QDir::homePath() + QString("/SimplestStudio");
        _settings = new QSettings(_settings_path + QString("/settings.ini"), QSettings::IniFormat, this);

        // Read Main Window
        _settings->beginGroup("MainWindow");
        this->restoreState(_settings->value("MainWindow/state").toByteArray());
        this->restoreGeometry(_settings->value("MainWindow/geometry").toByteArray());
        _settings->endGroup();

        // Read Tables State
        _settings->beginGroup("Tables");
        ui->tableWidget->horizontalHeader()->restoreState(_settings->value("Tables/table_widget_state").toByteArray());
        _settings->endGroup();

        // Read Settings
        _settings->beginGroup("Settings");
        QString openDir = _settings->value("Settings/open_dir").toString();
        if (openDir != "") _openDir = openDir;
        _method = _settings->value("Settings/method").toInt();
        _settings->endGroup();

        _input_file = "";
        _output_file = "";
        _folder_output = _openDir;
        _fmt = "";
        _sampling = 0;
        _bit_depth = 0;
        _bitrate = 0;
        _dur_mod = 0;
        _preset = "";

        QFile file;
        QString list("");
        file.setFileName(":/resources/css/style_0.css");
        if (file.open(QFile::ReadOnly)) {
            list = QString::fromUtf8(file.readAll());
            file.close();
        }
        this->setStyleSheet(list);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();

    // Save Main Window
    _settings->beginGroup("MainWindow");
    _settings->setValue("MainWindow/state", this->saveState());
    _settings->setValue("MainWindow/geometry", this->saveGeometry());
    _settings->endGroup();

    // Save Tables
    _settings->beginGroup("Tables");
    _settings->setValue("Tables/table_widget_state", ui->tableWidget->horizontalHeader()->saveState());
    _settings->endGroup();

    // Save Settings
    _settings->beginGroup("Settings");
    _settings->setValue("Settings/open_dir", _openDir);
    _settings->setValue("Settings/method", _method);
    _settings->endGroup();

    event->accept();
}

void MainWindow::on_actionAbout_clicked()
{
    About about(this);
    about.setModal(true);
    about.exec();
}

void MainWindow::on_actionSettings_clicked()
{
    Settings settings(this);
    settings.setModal(true);
    settings.setParemeters(&_method);
    settings.exec();
}

void MainWindow::on_buttonAdd_clicked()  // open_file
{
    QStringList fileName;
    QFileDialog openFilesWindow(nullptr);
    openFilesWindow.setWindowTitle("Open File");
    openFilesWindow.setMinimumWidth(600);
    openFilesWindow.setWindowFlags(Qt::Dialog | Qt::SubWindow);
#if defined (Q_OS_UNIX)
    openFilesWindow->setOptions(QFileDialog::DontUseNativeDialog);
#endif
    openFilesWindow.setOptions(QFileDialog::DontResolveSymlinks);
    openFilesWindow.setAcceptMode(QFileDialog::AcceptOpen);
    openFilesWindow.setFileMode(QFileDialog::ExistingFiles);
    openFilesWindow.setDirectory(_openDir);
    openFilesWindow.setNameFilter(tr("Audio Files: *.mp3, *.flac, *.wav,"
                                     " *.ape, *.aac, *.ac3, *.dff, *.dts,"
                                     " *.ogg, *.m4a, *.mkv, *.iso (*.mp3"
                                     " *.flac *.wav *.ape *.aac *.ac3 *.dff"
                                     " *.dts *.ogg *.m4a *.mkv *.iso);;All files (*.*)"));
    if (openFilesWindow.exec() != QFileDialog::Accepted) {
        return;
    }
    fileName = openFilesWindow.selectedFiles();

    MediaInfo MI;
    int i = 1;
    while (i <= fileName.size()) {
        const QString file = fileName.at(i-1);
        QString params[COLUMNS_COUNT] = {};
        MI.Open(file.toStdWString());
        double dur = 0.001*QString::fromStdWString(MI.Get(Stream_General, 0, L"Duration")).toDouble();
        params[Column::FILENAME] = QFileInfo(file).fileName();
        params[Column::PATH] = QFileInfo(file).absolutePath();
        params[Column::DURATION] = timeConverter(dur);
        params[Column::TECH_DUR] = QString::number(dur, 'f', 3);
        if (i == 1) _openDir = params[Column::PATH];
        for (int stream = 0; stream < 15; stream++) {
            params[Column::STREAM] = QString::number(stream);
            params[Column::FORMAT] = QString::fromStdWString(MI.Get(Stream_Audio, stream, L"Format"));
            params[Column::SAMPLING] = QString::fromStdWString(MI.Get(Stream_Audio, stream, L"SamplingRate"));
            params[Column::DEPTH] = QString::fromStdWString(MI.Get(Stream_Audio, stream, L"BitDepth"));
            params[Column::BITRATE] = QString::number((QString::fromStdWString(MI.Get(Stream_Audio, stream,
                                       L"BitRate"))).toDouble()/1000) + QString(" kb/s");
            if (params[Column::FORMAT] == "") break;
            if (params[Column::DEPTH] == "") params[Column::DEPTH] = "Undef";
            const int numRows = ui->tableWidget->rowCount();
            ui->tableWidget->setRowCount(numRows + 1);
            const Qt::AlignmentFlag flags[COLUMNS_COUNT] = {
                Qt::AlignVCenter, Qt::AlignCenter, Qt::AlignCenter, Qt::AlignCenter, Qt::AlignCenter,
                Qt::AlignCenter, Qt::AlignCenter, Qt::AlignVCenter, Qt::AlignVCenter};
            for (int i = 0; i < COLUMNS_COUNT; i++) {
                QTableWidgetItem *item = new QTableWidgetItem(params[i]);
                item->setTextAlignment(flags[i]);
                ui->tableWidget->setItem(numRows, i, item);
            }
        }
        i++;
    }
    MI.Close();
    const int rowsCount = ui->tableWidget->rowCount();
    if (rowsCount > 0) ui->tableWidget->selectRow(rowsCount - 1);
}

void MainWindow::on_buttonNext_clicked() // smart_detect
{
    QString file("");
    int row = ui->tableWidget->currentRow();
    if (row != -1) {
        file = ui->tableWidget->item(row, Column::FILENAME)->text();
        _input_file = ui->tableWidget->item(row, Column::PATH)->text() + "/" + file;
        _folder_output = ui->tableWidget->item(row, Column::PATH)->text();
    }
    if ((_input_file == "") || (_folder_output == "")) {
        _message = "Select first input file!";
        showMessage(_message);
        return;
    }
    QString prefix = ui->comboBox_codec->currentText().toLower();
    const QString file_ext = QFileInfo(file).suffix();
    const QString file_without_ext = QFileInfo(file).completeBaseName();

    _output_file = _folder_output + "/" + file_without_ext + "_optimized." + prefix;
    std::cout << "Output file: " << _output_file.toStdString() << std::endl;         // Output file debug information //
    QString cur_format = ui->comboBox_codec->currentText();
    bool comp_out = (cur_format == "FLAC") || (cur_format == "WAV");
    bool comp_in = (file_ext != "flac") && (file_ext != "wav") && (file_ext != "dff");
    if (comp_out && comp_in) {
        _message = "Only lossless files are supported for converting to FLAC or WAV format.";
        showMessage(_message);
        return;
    }
    if ((ui->comboBox_sampling->currentText() == "Smart mode")
            || (ui->comboBox_compressLevel->currentText() == "Smart mode")
            || (ui->comboBox_depth->currentText() == "Smart mode")) {
        _fmt = ui->tableWidget->item(row, Column::FORMAT)->text();
        QString sampling_ = ui->tableWidget->item(row, Column::SAMPLING)->text();
        _sampling = sampling_.toInt();
        QString bit_depth_ = ui->tableWidget->item(row, Column::DEPTH)->text();
        _bit_depth = bit_depth_.toInt();
        QString bitrate_ = ui->tableWidget->item(row, Column::BITRATE)->text().remove(" kb/s");
        _bitrate = bitrate_.toInt();
        // Debugging information
        std::cout << "Format: " << _fmt.toStdString() << std::endl;
        std::cout << "Sampling: " << _sampling << std::endl;
        std::cout << "Bit depth: " << _bit_depth << std::endl;
        std::cout << "Bitrate: " << _bitrate << std::endl;
        //
        if ((_fmt == "") || (_sampling == 0) || (_bit_depth == 0)) {
            _message = "It is impossible to determine the settings for the Smart mode."
                       " Select the settings manually.";
            showMessage(_message);
            return;
        }
        else {
            QString _comboBox_codec_index = ui->comboBox_codec->currentText();
            int _comboBox_content_type_index = 0;
            int _comboBox_compression_index = 0;
            int _comboBox_bit_depth_index = 0;
            bool flag_no_smart = false;
            SmartSettings smartsettings(this);
            smartsettings.setModal(true);
            smartsettings.setParameters(_comboBox_codec_index, &_comboBox_content_type_index,
                                        &_comboBox_compression_index, &_comboBox_bit_depth_index,
                                        &_sampling, &_bit_depth, &flag_no_smart);
            smartsettings.exec();                                   // Calling smartsettings and wait for return //
            if (flag_no_smart == false) {
                return;
            }
            ui->comboBox_sampling->setCurrentIndex(_comboBox_content_type_index);   // Continue on return from smartsettings //
            ui->comboBox_depth->setCurrentIndex(_comboBox_bit_depth_index);
            ui->comboBox_compressLevel->setCurrentIndex(_comboBox_compression_index);
            make_preset();
        }
    }
    else {
        make_preset();
    }
}

void MainWindow::on_buttonRemove_clicked() // remove_file
{
    int row = ui->tableWidget->currentRow();
    if (row != -1) ui->tableWidget->removeRow(row);
}

void MainWindow::on_comboBox_codec_currentTextChanged() // settings_menu
{
    ui->comboBox_depth->clear();
    ui->comboBox_compressLevel->clear();
    ui->comboBox_sampling->clear();
    ui->label_compressLevel->clear();
    ui->comboBox_sampling->setEnabled(true);
    ui->comboBox_compressLevel->setEnabled(true);
    ui->comboBox_depth->setEnabled(true);

    QStringList flacDepth = {"Smart mode", "16 bits", "24 bits"};
    QStringList flacCompressLev = {"Smart mode", "12", "11", "10", "9", "8", "7", "6", "5", "4",
                                   "3", "2", "1", "0"};
    QStringList flacSampl = {"Smart mode", "8000", "11025", "16000", "22050", "24000", "32000",
                             "44100", "48000", "88200", "96000", "176400", "192000"};

    if (ui->comboBox_codec->currentText() == "FLAC") {
        ui->comboBox_depth->addItems(flacDepth);
        ui->comboBox_depth->setCurrentIndex(0);
        ui->comboBox_compressLevel->addItems(flacCompressLev);
        ui->comboBox_compressLevel->setCurrentIndex(0);
        ui->comboBox_sampling->addItems(flacSampl);
        ui->comboBox_sampling->setCurrentIndex(0);
        ui->label_compressLevel->setText("Compress level:");
    }
    else if (ui->comboBox_codec->currentText() == "MP3") {
        ui->comboBox_depth->addItems({"Auto"});
        ui->comboBox_depth->setCurrentIndex(0);
        ui->comboBox_compressLevel->addItems({"320k", "256k", "224k", "192k", "160k", "128k", "112k", "96k", "80k", "64k", "48k", "40k", "32k", "24k", "16k", "8k"});
        ui->comboBox_compressLevel->setCurrentIndex(3);
        ui->comboBox_sampling->addItems({"8000", "11025", "12000", "16000", "22050", "24000", "32000", "44100", "48000"});
        ui->comboBox_sampling->setCurrentIndex(7);
        ui->label_compressLevel->setText("Bitrate:");
        ui->comboBox_depth->setEnabled(false);
    }
    else if (ui->comboBox_codec->currentText() == "WAV") {
        ui->comboBox_depth->addItems({"Smart mode", "16 bits", "24 bits"});
        ui->comboBox_depth->setCurrentIndex(0);
        ui->comboBox_compressLevel->addItems({"Auto"});
        ui->comboBox_compressLevel->setCurrentIndex(0);
        ui->comboBox_sampling->addItems({"Smart mode", "8000", "11025", "16000", "22050", "24000", "32000", "44100", "48000", "88200", "96000", "176400", "192000"});
        ui->comboBox_sampling->setCurrentIndex(0);
        ui->label_compressLevel->setText("Bitrate:");
        ui->comboBox_compressLevel->setEnabled(false);
    }
    else if (ui->comboBox_codec->currentText() == "DFF") {
        ui->comboBox_depth->addItems({"1 bit"});
        ui->comboBox_depth->setCurrentIndex(0);
        ui->comboBox_compressLevel->addItems({"2 ch", "5.1 ch"});
        ui->comboBox_compressLevel->setCurrentIndex(0);
        ui->comboBox_sampling->addItems({"2822400"});
        ui->comboBox_sampling->setCurrentIndex(0);
        ui->label_compressLevel->setText("Channels:");
        ui->comboBox_sampling->setEnabled(false);
        ui->comboBox_depth->setEnabled(false);
    }
}

void MainWindow::make_preset() // make_preset
{
    QString sampling_rate = ui->comboBox_sampling->currentText();
    QString abitrate = ui->comboBox_compressLevel->currentText();
    QString compression_level = ui->comboBox_compressLevel->currentText();
    QString channels = ui->comboBox_compressLevel->currentText();
    QString sample_fmt = "";
    QString methods[3] = {
        "aresample",
        "soxr",
        "dither triangular hp"
    };
    QString method = methods[_method];

    if (ui->comboBox_depth->currentText() == "16 bits") {
        sample_fmt = "s16";
    }
    else if (ui->comboBox_depth->currentText() == "24 bits") {
        sample_fmt = "s24";
    }
    else if ((ui->comboBox_depth->currentText() == "24 bits") && (ui->comboBox_codec->currentText() == "FLAC")) {
        sample_fmt = "s32";
    }

    if ((ui->comboBox_codec->currentText() == "FLAC") && (method == "aresample")) {
        _preset = QString("-map 0:a:0 -vn -c:a flac -compression_level %1 -af aresample=out_sample_fmt=%2:out_sample_rate=%3").arg(compression_level, sample_fmt, sampling_rate);
        encode_file();
    }
    else if ((ui->comboBox_codec->currentText() == "FLAC") && (method == "soxr")) {
        _preset = QString("-map 0:a:0 -vn -c:a flac -compression_level %1 -resampler soxr -sample_fmt %2 -ar %3").arg(compression_level, sample_fmt, sampling_rate);
        encode_file();
    }
    else if ((ui->comboBox_codec->currentText() == "FLAC") && (method == "dither triangular hp")) {
        _preset = QString("-map 0:a:0 -vn -c:a flac -compression_level %1 -dither_method triangular_hp -sample_fmt %2 -ar %3").arg(compression_level, sample_fmt, sampling_rate);
        encode_file();
    }
    else if (ui->comboBox_codec->currentText() == "MP3") {
        _preset = QString("-map 0:a:0 -vn -c:a libmp3lame -ar %1 -b:a %2").arg(sampling_rate, abitrate);
        encode_file();
    }
    else if ((ui->comboBox_codec->currentText() == "WAV") && (sample_fmt == "s16")) {
        _preset = QString("-map 0:a:0 -vn -bitexact -c:a pcm_s16le -ar %1").arg(sampling_rate);
        encode_file();
    }
    else if ((ui->comboBox_codec->currentText() == "WAV") && (sample_fmt == "s24")) {
        _preset = QString("-map 0:a:0 -vn -bitexact -c:a pcm_s24le -ar %1").arg(sampling_rate);
        encode_file();
    }
    else if ((ui->comboBox_codec->currentText() == "DFF") && (channels == "2 ch")) {
        _preset = "sacd_extract -2 -p -c -i";
        extract_file();
    }
    else if ((ui->comboBox_codec->currentText() == "DFF") && (channels == "5.1 ch")) {
        _preset = "sacd_extract -m -p -c -i";
        extract_file();
    }
}

void MainWindow::extract_file() // extract_file
{
    std::cout << "Extract file ..." << std::endl;
    std::cout << "Preset: " << _preset.toStdString() << std::endl;
    ui->buttonAdd->setEnabled(false);
    //ui->buttonOutput->setEnabled(false);
    ui->buttonNext->setEnabled(false);
    ui->buttonRemove->setEnabled(false);

    bool _encodingProcess = false;
    Progress progress(this);
    progress.setModal(true);
    progress.setParameters(&_message, _input_file, _output_file,
                           _folder_output, _preset, _encodingProcess, _dur_mod);
    if (progress.exec() != QDialog::Accepted) {
        _message = "Extracting aborted!";
    }

    ui->buttonAdd->setEnabled(true); // Continue on return from progress //
    ui->buttonNext->setEnabled(true);
    ui->buttonRemove->setEnabled(true);
    int row = ui->tableWidget->currentRow();
    int numRows = ui->tableWidget->rowCount();
    if (numRows > (row + 1)) {
        ui->tableWidget->selectRow(row + 1);
    }
    showMessage(_message);
}

void MainWindow::encode_file() // encode_file
{
    std::cout << "Encode file ..." << std::endl;
    std::cout << "Preset: " << _preset.toStdString() << std::endl;
    int row = ui->tableWidget->currentRow();
    float dur = (ui->tableWidget->item(row, Column::TECH_DUR)->text()).toFloat();
    ui->buttonAdd->setEnabled(false);
    ui->buttonNext->setEnabled(false);
    ui->buttonRemove->setEnabled(false);
    if (dur == 0.0f) {
        std::cout << "No duration info!" << std::endl;
        ui->buttonAdd->setEnabled(true);
        ui->buttonNext->setEnabled(true);
        ui->buttonRemove->setEnabled(true);
        _message = "Select correct input file!";
        showMessage(_message);
        return;
    }

    _dur_mod = dur;
    bool _encodingProcess = true;
    Progress progress(this);
    progress.setModal(true);
    progress.setParameters(&_message, _input_file, _output_file,
                           _folder_output, _preset, _encodingProcess, _dur_mod);

    if (progress.exec() != QDialog::Accepted) {
        _message = "Encoding aborted!";
        showMessage(_message);
    }
    ui->buttonAdd->setEnabled(true); // Continue on return from progress //
    ui->buttonNext->setEnabled(true);
    ui->buttonRemove->setEnabled(true);

    int numRows = ui->tableWidget->rowCount();
    if (numRows > (row + 1)) {
        ui->tableWidget->selectRow(row + 1);
    }
    if (ui->comboBox_codec->currentText() == "FLAC") {
        ui->comboBox_sampling->setCurrentIndex(0);
        ui->comboBox_depth->setCurrentIndex(0);
        ui->comboBox_compressLevel->setCurrentIndex(0);
    }
    else if (ui->comboBox_codec->currentText() == "WAV") {
        ui->comboBox_sampling->setCurrentIndex(0);
        ui->comboBox_depth->setCurrentIndex(0);
    }
}

QString MainWindow::timeConverter(double &time)     /*** Time converter to hh:mm:ss.msc ***/
{
    const int &&h = static_cast<int>(trunc(time / 3600));
    const int &&m = static_cast<int>(trunc((time - (double)(h * 3600)) / 60));
    const int &&s = static_cast<int>(trunc(time - (double)(h * 3600) - (double)(m * 60)));
    const int &&ms = static_cast<int>(round(1000 * (time - (double)(h * 3600) - (double)(m * 60) - (double)s)));
    const QString &&hrs = QString::number(h).rightJustified(2, '0');
    const QString &&min = QString::number(m).rightJustified(2, '0');
    const QString &&sec = QString::number(s).rightJustified(2, '0');
    const QString &&msec = QString::number(ms).rightJustified(3, '0');
    return QString("%1:%2:%3.%4").arg(hrs, min, sec, msec);
}

void MainWindow::showMessage(const QString &_message)
{
    Message messageWindow(this);
    messageWindow.setModal(true);
    messageWindow.setMessage(_message);
    messageWindow.exec();
}
