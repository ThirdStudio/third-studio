#ifndef SMARTSETTINGS_H
#define SMARTSETTINGS_H

#include <QDialog>
#include <iostream>

namespace Ui {
    class SmartSettings;
}

class SmartSettings : public QDialog
{
    Q_OBJECT

public:

    explicit SmartSettings(QWidget *parent = nullptr);

    ~SmartSettings();

    void setParameters(const QString &comboBox_codec_text, int *comboBox_content_type_index,
                       int *comboBox_compression_index, int *comboBox_bit_depth_index,
                       int *sampling, int *bit_depth, bool *flag_no_smart);
private slots:

    void on_buttonCancel_clicked();

    void on_buttonContinue_clicked();

    void on_comboBox_optimize_for_currentTextChanged();

    void on_checkBox_preference_stateChanged();

    void change_optimize_for();

private:

    Ui::SmartSettings *ui;

    bool *_flag_no_smart;

    QString _comboBox_codec_text;

    int *_comboBox_content_type_index;

    int *_comboBox_bit_depth_index;

    int *_comboBox_compression_index;

    int *_sampling;

    int *_bit_depth;
};


#endif // SMARTSETTINGS_H
