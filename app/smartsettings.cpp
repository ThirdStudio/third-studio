#include "smartsettings.h"
#include "ui_smartsettings.h"


SmartSettings::SmartSettings(QWidget *parent):
    QDialog(parent),
    ui(new Ui::SmartSettings)
{
    ui->setupUi(this);
    this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

SmartSettings::~SmartSettings()
{
    delete ui;
}

void SmartSettings::setParameters(const QString &comboBox_codec_text, int *comboBox_content_type_index,
                                  int *comboBox_compression_index, int *comboBox_bit_depth_index,
                                  int *sampling, int *bit_depth, bool *flag_no_smart)
{
    _comboBox_codec_text = "";
    _comboBox_codec_text = comboBox_codec_text;
    _comboBox_content_type_index = comboBox_content_type_index;
    _comboBox_bit_depth_index = comboBox_bit_depth_index;
    _comboBox_compression_index = comboBox_compression_index;
    _sampling = sampling;
    _bit_depth = bit_depth;
    _flag_no_smart = flag_no_smart;
    if (_comboBox_codec_text == "FLAC")
    {
        ui->label_compression->show();
        ui->comboBox_compression->show();
    } else {
        ui->label_compression->hide();
        ui->comboBox_compression->hide();
    };
    change_optimize_for();
}

void SmartSettings::on_buttonCancel_clicked() // close
{
    close();
}

void SmartSettings::on_buttonContinue_clicked() // execute_make_preset
{
    int content_type = ui->comboBox_content_type->currentIndex();
    int bit_depth_output = ui->comboBox_bit_depth->currentIndex();
    if (_comboBox_codec_text == "FLAC") {
        int compression = ui->comboBox_compression->currentIndex();
        *_comboBox_content_type_index = content_type + 1;
        *_comboBox_compression_index = bit_depth_output + 1;
        *_comboBox_bit_depth_index = compression + 1;
    }
    else if (_comboBox_codec_text == "WAV") {
        *_comboBox_content_type_index = content_type + 1;
        *_comboBox_compression_index = bit_depth_output + 1;
    }
    *_flag_no_smart = true;
    close();
}

void SmartSettings::on_comboBox_optimize_for_currentTextChanged() // change_optimize_for
{
    change_optimize_for();
}

void SmartSettings::on_checkBox_preference_stateChanged() // change_optimize_for
{
    change_optimize_for();
}

void SmartSettings::change_optimize_for()
{
    QString optimize_for = ui->comboBox_optimize_for->currentText();
    if (*_sampling <= 8000) {
        ui->comboBox_content_type->setCurrentIndex(0);
    };
    if ((*_sampling <= 11025) && (*_sampling > 8000)) {
        ui->comboBox_content_type->setCurrentIndex(1);
    };
    if ((*_sampling <= 16000) && (*_sampling > 11025)) {
        ui->comboBox_content_type->setCurrentIndex(2);
    };
    if ((*_sampling <= 22050) && (*_sampling > 16000)) {
        ui->comboBox_content_type->setCurrentIndex(3);
    };
    if ((*_sampling <= 24000) && (*_sampling > 22050)) {
        ui->comboBox_content_type->setCurrentIndex(4);
    };
    if ((*_sampling <= 32000) && (*_sampling > 24000)) {
        ui->comboBox_content_type->setCurrentIndex(5);
    };
    if ((*_sampling <= 44100) && (*_sampling > 32000)) {
        ui->comboBox_content_type->setCurrentIndex(6);
    };
    if (optimize_for == "Listening in the car on high-quality equipment") {
        if (*_sampling > 44100) {
            ui->comboBox_content_type->setCurrentIndex(6);
        };
        ui->comboBox_bit_depth->setCurrentIndex(0);
    };
    if (optimize_for == "Listening at home on standard equipment") {
        if (*_sampling > 44100) {
            if (ui->checkBox_preference->isChecked()) {
                ui->comboBox_content_type->setCurrentIndex(7);
            } else {
                ui->comboBox_content_type->setCurrentIndex(6);
            };
        };
        ui->comboBox_bit_depth->setCurrentIndex(0);
    };
    if (optimize_for == "Listening at home on high-quality equipment") {
        if (*_sampling > 44100) {
            ui->comboBox_content_type->setCurrentIndex(7);
            if (ui->checkBox_preference->isChecked()) {
                if (*_bit_depth == 16) {
                    ui->comboBox_bit_depth->setCurrentIndex(0);
                };
                if (*_bit_depth >= 24) {
                    ui->comboBox_bit_depth->setCurrentIndex(1);
                };
            } else {
                ui->comboBox_bit_depth->setCurrentIndex(0);
            };
        };
    };
    if (optimize_for == "Studio quality") {
        if ((*_sampling <= 48000) && (*_sampling > 44100)) {
            ui->comboBox_content_type->setCurrentIndex(7);
        };
        if ((*_sampling <= 88200) && (*_sampling > 48000)) {
            ui->comboBox_content_type->setCurrentIndex(8);
        };
        if ((*_sampling <= 96000) && (*_sampling > 88200)) {
            ui->comboBox_content_type->setCurrentIndex(9);
        };
        if ((*_sampling <= 176400) && (*_sampling > 96000)) {
            ui->comboBox_content_type->setCurrentIndex(10);
        };
        if ((*_sampling <= 192000) && (*_sampling > 176400)) {
            ui->comboBox_content_type->setCurrentIndex(11);
        };
        if (*_bit_depth == 16) {
            ui->comboBox_bit_depth->setCurrentIndex(0);
        };
        if (*_bit_depth >= 24) {
            ui->comboBox_bit_depth->setCurrentIndex(1);
        };
    };
}


