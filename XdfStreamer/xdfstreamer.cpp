#include "xdfstreamer.h"
#include "ui_xdfstreamer.h"
#include <QFileDialog>
#include <QMessageBox>

XdfStreamer::XdfStreamer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::XdfStreamer)
{
    ui->setupUi(this);
    ui->spinBox->setMaximum(100000);
    ui->spinBox->setValue(10000);
    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    setWindowTitle("XDF Streamer");

    QObject::connect(ui->checkBox, SIGNAL(stateChanged(int)), this, SLOT(enableFilePicker(int)));
    QObject::connect(ui->toolButton, SIGNAL(clicked()), this, SLOT(openFilePicker()));
    QObject::connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(handleXdfFile()));
    QObject::connect(ui->lineEdit, SIGNAL(textChanged(QString)), this, SLOT(on_lineEdit_textChanged(QString)));
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
}

void XdfStreamer::openFilePicker()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open XDF File"), "", tr("XDF Files (*.xdf)"));

    if (fileName.compare("") != 0) {
        ui->lineEdit->setText(fileName);
    }
}

void XdfStreamer::handleXdfFile()
{
    if (ui->pushButton_2->text().compare("Load") == 0) {
        this->xdf = QSharedPointer<Xdf>(new Xdf);
        this->xdf->load_xdf(ui->lineEdit->text().toStdString());

        if (this->xdf.isNull()) {
            QString msg = "Unable to find " + ui->lineEdit->text() + "\nPlease check your path";
            QMessageBox::warning(this, tr("XDF Streamer"), msg, QMessageBox::Ok);
        }
        else {
            ui->pushButton_2->setText("Unload");
        }
    }
    else {
        this->xdf.clear();
        ui->pushButton_2->setText("Load");
    }
}

void XdfStreamer::on_lineEdit_textChanged(const QString &arg1)
{
    if (arg1.isEmpty()) {
        ui->pushButton_2->setEnabled(false);
    }
    else {
        ui->pushButton_2->setEnabled(true);
    }
}
