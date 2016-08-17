#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QFileDialog>
#include <QDebug>
QT_USE_NAMESPACE

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    defaultSettings.IPaddress = ui->IPaddress->text();
    defaultSettings.picturePath = ui->picturePath->text();

    updateSettings();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

SettingsDialog::Settings SettingsDialog::settings() const
{
    return currentSettings;
}


void SettingsDialog::on_applyButton_clicked()
{
    updateSettings();
    hide();
}

void SettingsDialog::updateSettings()
{
    currentSettings.IPaddress = ui->IPaddress->text();
    currentSettings.picturePath = ui->picturePath->text();
    defaultSettings.picturePath = currentSettings.picturePath;
}

void SettingsDialog::updatePath(QString path)
{
    ui->picturePath->setText(path)  ;
    updateSettings();
 //   qDebug() << "pp is: "<< currentSettings.picturePath <<endl;
}


void SettingsDialog::on_defultButton_clicked()
{
    ui->IPaddress->setText(defaultSettings.IPaddress);
    ui->picturePath->setText(defaultSettings.picturePath);
}

void SettingsDialog::on_setPath_clicked()
{
    QString Path;
    Path = QFileDialog::getExistingDirectory(this, tr("Save images"),
                                                    currentSettings.picturePath,
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    ui->picturePath->setText(Path)  ;
    updateSettings();
}
