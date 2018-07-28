#include "xdfstreamer.h"
#include "ui_xdfstreamer.h"
#include <QFileDialog>

XdfStreamer::XdfStreamer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::XdfStreamer)
{
    ui->setupUi(this);
    ui->spinBox->setMaximum(10000);
    ui->spinBox->setValue(10000);
    ui->pushButton->setEnabled(false);

    QObject::connect(ui->checkBox, SIGNAL(stateChanged(int)), this, SLOT(enableFilePicker(int)));
    QObject::connect(ui->toolButton, SIGNAL(clicked()), this, SLOT(openFilePicker()));
}

XdfStreamer::~XdfStreamer()
{
    delete ui;
}

void XdfStreamer::enableFilePicker(int status)
{
    bool enabled = status == Qt::Checked ? false : true;

    ui->label->setEnabled(enabled);
    ui->lineEdit->setEnabled(enabled);
    ui->toolButton->setEnabled(enabled);
    ui->pushButton_2->setEnabled(enabled);
}

void XdfStreamer::openFilePicker()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open XDF File"), "", tr("XDF Files (*.xdf)"));

    if (fileName.compare("") != 0) {
        ui->lineEdit->setText(fileName);
    }
}
