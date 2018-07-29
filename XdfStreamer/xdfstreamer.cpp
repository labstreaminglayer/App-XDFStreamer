#include "xdfstreamer.h"
#include "ui_xdfstreamer.h"
#include "lsl_cpp.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <ctime>

XdfStreamer::XdfStreamer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::XdfStreamer)
{
    ui->setupUi(this);
    ui->spinBox->setMaximum(100000);
    ui->spinBox->setValue(10000);
    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    ui->spinBox_2->setValue(32);
    ui->label_3->hide();
    ui->lineEdit_2->hide();
    ui->label_4->hide();
    ui->spinBox_2->hide();
    ui->label_5->hide();
    ui->lineEdit_3->setText("EEG");
    ui->lineEdit_3->hide();
    ui->groupBox_2->hide();
    setWindowTitle("XDF Streamer");

    QObject::connect(ui->checkBox, SIGNAL(stateChanged(int)), this, SLOT(on_checkBox_stateChanged(int)));
    QObject::connect(ui->toolButton, SIGNAL(clicked()), this, SLOT(openFilePicker()));
    QObject::connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(handleXdfFile()));
}

XdfStreamer::~XdfStreamer()
{
    delete ui;
}

void XdfStreamer::pushRandomSamples()
{
    QString streamName = ui->lineEdit_2->text();
    const int samplingRate = ui->spinBox->value();
    int channelCount = ui->spinBox_2->value();
    lsl::stream_info info(streamName.toStdString(), "EEG", channelCount, (double)samplingRate, lsl::cf_double64, "RT_Sender_SimulationPC");
    lsl::stream_outlet outlet(info);

    const double dSamplingInterval = 1.0 / samplingRate;
    std::vector<double> sample(channelCount);

    double starttime = ((double)clock()) / CLOCKS_PER_SEC;

    for (unsigned t = 0;; t++) {
        {
            std::lock_guard<std::mutex> guard(this->mutex_stop_thread);
            if (this->stop_thread) {
                return;
            }
        }

        while (((double)clock()) / CLOCKS_PER_SEC < starttime + t * dSamplingInterval);

        for (int c = 0; c < channelCount; c++) {
            sample[c] = (rand() % 1500) / 500.0 - 1.5;
        }

        outlet.push_sample(sample);
    }
}

void XdfStreamer::on_checkBox_stateChanged(int status)
{
    bool enabled = status == Qt::Checked ? false : true;
    ui->label->setEnabled(enabled);
    ui->lineEdit->setEnabled(enabled);
    ui->toolButton->setEnabled(enabled);
    ui->pushButton->setEnabled(!enabled);
    bool loadButtonEnabled = ui->lineEdit->text().isEmpty() ? false : enabled;
    ui->pushButton_2->setEnabled(loadButtonEnabled);

    if (status == Qt::Checked) {
        ui->label_3->show();
        ui->lineEdit_2->show();
        ui->lineEdit_2->setText("ActiChamp-0");
        ui->label_4->show();
        ui->spinBox_2->show();
        ui->label_5->show();
        ui->lineEdit_3->show();
        ui->groupBox_2->show();
    }
    else {
        ui->label_3->hide();
        ui->lineEdit_2->hide();
        ui->label_4->hide();
        ui->spinBox_2->hide();
        ui->label_5->hide();
        ui->lineEdit_3->hide();
        ui->groupBox_2->hide();
    }
}

void XdfStreamer::openFilePicker()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open XDF File"), "", tr("XDF Files (*.xdf)"));

    if (!fileName.isEmpty()) {
        ui->lineEdit->setText(fileName);
    }
}

void XdfStreamer::handleXdfFile()
{
    if (ui->pushButton_2->text().compare("Load") == 0) {
        this->xdf = QSharedPointer<Xdf>(new Xdf);
        this->xdf->load_xdf(ui->lineEdit->text().toStdString());

        if (this->xdf->streams.empty()) {
            QString msg = "Unable to find " + ui->lineEdit->text() + "\nPlease check your path";
            QMessageBox::warning(this, tr("XDF Streamer"), msg, QMessageBox::Ok);
        }
        else {
            ui->pushButton->setEnabled(true);
            ui->pushButton_2->setText("Unload");
        }
    }
    else {
        this->xdf.clear();
        ui->pushButton->setEnabled(false);
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

void XdfStreamer::on_pushButton_clicked()
{
    if (ui->pushButton->text().compare("Stream") == 0) {
        ui->pushButton->setText("Stop");

        if (ui->checkBox->isChecked()) {
            qDebug() << "Generating synthetic signals";

            this->pushThread = new std::thread(&XdfStreamer::pushRandomSamples, this);
        }
        else {
            qDebug() << "Load XDF";

//            QString streamName = ui->lineEdit_2->text();
//            const int samplingRate = ui->spinBox->value();
//            const int channelCount = 32;
//            lsl::stream_info info(streamName.toStdString(), "EEG", channelCount, (double)samplingRate, lsl::cf_double64, "RT_Sender_SimulationPC");
//            lsl::stream_outlet outlet(info);

//            const double dSamplingInterval = 1.0 / samplingRate;
//            double sample[channelCount];

//            //===============================================================================================================
//            // Get Raw Stream Index
//            //===============================================================================================================
//            int streamIdx = -1;

//            for (size_t k = 0; k < this->xdf->streams.size(); k++) {
//                if (this->xdf->streams[k].info.name.compare("ActiChamp-0") == 0) {
//                    streamIdx = k;
//                    break;
//                }
//            }

//            //===============================================================================================================
//            // Push samples to LSL
//            //===============================================================================================================
//            if (streamIdx == -1) {
//                qDebug() << "Didn't find the data stream.";
//            }
//            else {
//                QMessageBox::information(this, tr("Click to start streaming"), tr("Loading XDF file successful"), QMessageBox::Ok);

//                double starttime = ((double)clock()) / CLOCKS_PER_SEC;

//                for (unsigned t = 0; t < xdf->streams[streamIdx].time_series.front().size(); t++) {
//                    while (((double)clock()) / CLOCKS_PER_SEC < starttime + t * dSamplingInterval);

//                    for (int c = 0; c < channelCount; c++) {
//                        sample[c] = xdf->streams[streamIdx].time_series[c][t];
//                    }

//                    outlet.push_sample(sample);
//                }
//            }
        }
    }
    else {
        this->mutex_stop_thread.lock();
        this->stop_thread = true;
        this->mutex_stop_thread.unlock();

        ui->pushButton->setText("Stream");
        this->pushThread->join();
        delete this->pushThread;
        this->pushThread = nullptr;
    }
}
