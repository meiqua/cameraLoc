

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

QT_USE_NAMESPACE

QT_BEGIN_NAMESPACE

namespace Ui {
class SettingsDialog;
}
QT_END_NAMESPACE

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    struct Settings {
       QString IPaddress;
       QString picturePath;
    };

    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

    Settings settings() const;

    void updateSettings();
    void updatePath(QString path);

private slots:

    void on_applyButton_clicked();

    void on_defultButton_clicked();

    void on_setPath_clicked();

private:


private:
    Ui::SettingsDialog *ui;
    Settings currentSettings;
    Settings defaultSettings;
};

#endif // SETTINGSDIALOG_H
