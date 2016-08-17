#include "testparamset.h"
#include "ui_testparamset.h"

testparamset::testparamset(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::testparamset)
{
    ui->setupUi(this);


    //connect(ui->verticalSlider,SIGNAL(valueChanged(int)),this,SLOT(valueChanged(int)));
    steps = 0;
    do1 = 0;
    do2 = 0;
}

testparamset::~testparamset()
{
    delete ui;
}

void testparamset::stepReverse()
{
    steps = 5 - steps;
}

void testparamset::on_do1_clicked()
{
    do1 = ui->do1->isChecked();
        emit refresh();
}

void testparamset::on_do2_clicked()
{
    do2 = ui->do2->isChecked();
        emit refresh();
}

void testparamset::valueChanged(int position)
{
    steps = position;
    stepReverse();
    emit refresh();
}
