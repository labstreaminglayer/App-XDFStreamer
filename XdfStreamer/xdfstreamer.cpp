#include "xdfstreamer.h"
#include "ui_xdfstreamer.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <ctime>
#include <math.h>
#include <iostream>


XdfStreamer::XdfStreamer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::XdfStreamer)
{
    ui->setupUi(this);
    ui->spinBox->setMaximum(100000);
    ui->spinBox->setValue(10000);
    ui->pushButtonStream->setEnabled(false);
    ui->pushButtonLoad->setEnabled(false);
    ui->spinBox_2->setValue(32);
    ui->formatComboBox->setCurrentIndex((int)(lsl::cf_double64-1));
    ui->label_3->hide();
    ui->lineEdit_2->hide();
    ui->label_4->hide();
    ui->spinBox_2->hide();
    ui->label_5->hide();
    ui->lineEdit_3->setText("EEG");
    ui->lineEdit_3->hide();
    ui->groupBox_2->hide();

    QStringList header = {"Property", "Value"};
    ui->treeWidgetXDF->setHeaderLabels(header);
    ui->treeWidgetXDF->header()->hide();
    ui->treeWidgetXDF->setColumnCount(2);

    ui->treeWidgetRandomSignal->setHeaderLabels(header);
    ui->treeWidgetRandomSignal->header()->hide();
    ui->treeWidgetRandomSignal->setColumnCount(2);
    ui->treeWidgetRandomSignal->hide();

    setWindowTitle("XDF Streamer");
}

XdfStreamer::~XdfStreamer()
{
    delete ui;
}

void XdfStreamer::pushRandomData(QSharedPointer<lsl::stream_outlet> outlet_ptr, const int samplingRate, const int channelCount)
{
    const double dSamplingInterval = 1.0 / samplingRate;
    std::vector<double> sample(channelCount);

    double starttime = ((double)clock()) / CLOCKS_PER_SEC;

    for (unsigned t = 0;; t++) {
        {
            std::lock_guard<std::mutex> guard(this->mutex_stop_thread);
            if (this->stop_thread) {
                outlet_ptr.clear();
                return;
            }
        }

        while (((double)clock()) / CLOCKS_PER_SEC < starttime + t * dSamplingInterval);

        for (int c = 0; c < channelCount; c++) {
            /* Generate a sine wave
            if (c != 20) {
                sample[c] = 2 * sin(2 * 3.1415926 * 10 * (t * 1.0 / 10000) - 0.2) + 0.5;
            }
            */

            sample[c] = (rand() % 1500) / 500.0 - 1.5;
        }

        outlet_ptr->push_sample(sample);
    }
}

void XdfStreamer::pushXdfData(const int stream_id, QSharedPointer<lsl::stream_outlet> outlet_ptr, const int samplingRate, const int channelCount)
{
    const double dSamplingInterval = 1.0 / samplingRate;
    std::vector<double> sample(channelCount);

    double starttime = ((double)clock()) / CLOCKS_PER_SEC;

    for (unsigned t = 0; t < xdf->streams[stream_id].time_series.front().size(); t++) {
        {
            std::lock_guard<std::mutex> guard(this->mutex_stop_thread);
            if (this->stop_thread) {
                return;
            }
        }

        while (((double)clock()) / CLOCKS_PER_SEC < starttime + t * dSamplingInterval);

        for (int c = 0; c < channelCount; c++) {
            sample[c] = this->xdf->streams[stream_id].time_series[c][t];
        }

        outlet_ptr->push_sample(sample);
    }

    outlet_ptr.clear();
    emit this->ui->pushButtonStream->clicked();
}

void XdfStreamer::clearCache()
{
    this->xdf.clear();
    ui->pushButtonStream->setEnabled(false);
    ui->pushButtonLoad->setText("Load");
    ui->treeWidgetXDF->header()->hide();
    ui->treeWidgetXDF->clear();
    this->stream_ready = false;
}

void XdfStreamer::enableControlPanel(bool enabled)
{
    if (ui->checkBoxRandomSignal->checkState() == Qt::Unchecked) {
        ui->label->setEnabled(enabled);
        ui->lineEdit->setEnabled(enabled);
        ui->toolButtonBrowse->setEnabled(enabled);
        ui->pushButtonLoad->setEnabled(enabled);
    }

    ui->label_2->setEnabled(enabled);
    ui->label_3->setEnabled(enabled);
    ui->label_4->setEnabled(enabled);
    ui->label_5->setEnabled(enabled);
    ui->label_6->setEnabled(enabled);
    ui->lineEdit_2->setEnabled(enabled);
    ui->lineEdit_3->setEnabled(enabled);
    ui->checkBoxRandomSignal->setEnabled(enabled);
    ui->spinBox->setEnabled(enabled);
    ui->spinBox_2->setEnabled(enabled);
    ui->formatComboBox->setEnabled(enabled);
}

lsl::stream_info XdfStreamer::initializeLslStreamsForRandomData(const int samplingRate, const int channelCount)
{
    QString streamName = ui->lineEdit_2->text();
    std::string streamType = ui->lineEdit_3->text().toStdString();
    lsl::channel_format_t channelFormat = (lsl::channel_format_t)(ui->formatComboBox->currentIndex() + 1);
    lsl::stream_info info(streamName.toStdString(), streamType, channelCount, (double)samplingRate, channelFormat, "RT_Sender_SimulationPC");

    return info;
}

lsl::stream_info XdfStreamer::initializeLslStreamsForXdfData(const int stream_id, const int samplingRate, const int channelCount)
{
    std::string streamName = this->xdf->streams[stream_id].info.name;
    std::string streamType = this->xdf->streams[stream_id].info.type;

    lsl::channel_format_t channelFormat;
    if (this->xdf->streams[stream_id].info.channel_format.compare("float32") == 0) {
        channelFormat = lsl::cf_float32;
    }
    else if (this->xdf->streams[stream_id].info.channel_format.compare("double64") == 0) {
        channelFormat = lsl::cf_double64;
    }
    else if (this->xdf->streams[stream_id].info.channel_format.compare("int8") == 0) {
        channelFormat = lsl::cf_int8;
    }
    else if (this->xdf->streams[stream_id].info.channel_format.compare("int16") == 0) {
        channelFormat = lsl::cf_int16;
    }
    else if (this->xdf->streams[stream_id].info.channel_format.compare("int32") == 0) {
        channelFormat = lsl::cf_int32;
    }
    else if (this->xdf->streams[stream_id].info.channel_format.compare("int64") == 0) {
        channelFormat = lsl::cf_int64;
    }
    else if (this->xdf->streams[stream_id].info.channel_format.compare("string") == 0) {
        channelFormat = lsl::cf_string;
    }
    else {
        channelFormat = lsl::cf_undefined;
    }

    lsl::stream_info info(streamName, streamType, channelCount, (double)samplingRate, channelFormat, "RT_Sender_SimulationPC_" + streamName);

    // Reading channel names from the xdf file and setting for the stream!
    std::vector<std::map<std::string, std::string> > channels = this->xdf->streams[stream_id].info.channels;

    lsl::xml_element chns = info.desc().append_child("channels");
    for(const std::map<std::string, std::string> &item : channels) {
        lsl::xml_element chn = chns.append_child("channel");
        for(auto &[key, val] : item) {
            chn.append_child_value(key, val);
        }
    }

    return info;
}

void XdfStreamer::on_checkBoxRandomSignal_stateChanged(int status)
{
    if (status == Qt::Checked) {
        ui->pushButtonStream->setEnabled(true);
        ui->label_3->show();
        ui->lineEdit_2->show();
        ui->lineEdit_2->setText("ActiChamp-0");
        ui->label_4->show();
        ui->spinBox_2->show();
        ui->label_5->show();
        ui->lineEdit_3->show();
        ui->groupBox_2->show();
        ui->treeWidgetXDF->hide();
        ui->treeWidgetRandomSignal->show();
        ui->treeWidgetRandomSignal->header()->show();
        ui->treeWidgetRandomSignal->setColumnWidth(0, std::round(0.5 * ui->treeWidgetRandomSignal->width()));

        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidgetRandomSignal);
        item->setText(0, "Stream-" + QString::number(1));

        QTreeWidgetItem *subItem = new QTreeWidgetItem(item);
        subItem->setText(0, "Stream Name");
        subItem->setText(1, ui->lineEdit_2->text());
        item->addChild(subItem);

        subItem = new QTreeWidgetItem(item);
        subItem->setText(0, "Channel Format");
        subItem->setText(1, "Double");
        item->addChild(subItem);

        subItem = new QTreeWidgetItem(item);
        subItem->setText(0, "Samplign Rate");
        subItem->setText(1, QString::number(ui->spinBox->value()));
        item->addChild(subItem);

        subItem = new QTreeWidgetItem(item);
        subItem->setText(0, "Channel Count");
        subItem->setText(1, QString::number(ui->spinBox_2->value()));
        item->addChild(subItem);

        subItem = new QTreeWidgetItem(item);
        subItem->setText(0, "Stream Type");
        subItem->setText(1, ui->lineEdit_3->text());
        item->addChild(subItem);

        ui->treeWidgetRandomSignal->addTopLevelItem(item);
        ui->treeWidgetRandomSignal->expandAll();
    }
    else {
        ui->pushButtonStream->setEnabled(this->stream_ready ? true : false);
        ui->label_3->hide();
        ui->lineEdit_2->hide();
        ui->label_4->hide();
        ui->spinBox_2->hide();
        ui->label_5->hide();
        ui->lineEdit_3->hide();
        ui->groupBox_2->hide();
        ui->treeWidgetRandomSignal->clear();
        ui->treeWidgetRandomSignal->hide();
        ui->treeWidgetRandomSignal->header()->hide();
        ui->treeWidgetXDF->show();
    }

    bool enabled = status == Qt::Checked ? false : true;
    ui->label->setEnabled(enabled);
    ui->lineEdit->setEnabled(enabled);
    ui->toolButtonBrowse->setEnabled(enabled);
    bool loadButtonEnabled = ui->lineEdit->text().isEmpty() ? false : enabled;
    ui->pushButtonLoad->setEnabled(loadButtonEnabled);
}

void XdfStreamer::on_toolButtonBrowse_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open XDF File"), "", tr("XDF Files (*.xdf)"));

    if (!fileName.isEmpty()) {
        this->clearCache();
        ui->lineEdit->setText(fileName);
        on_pushButtonLoad_clicked();
    }
}

void XdfStreamer::on_pushButtonLoad_clicked()
{
    if (ui->pushButtonLoad->text().compare("Load") == 0) {
        this->xdf = QSharedPointer<Xdf>(new Xdf);
        this->xdf->load_xdf(ui->lineEdit->text().toStdString());

        if (this->xdf->streams.empty()) {
            QString msg = "Unable to find " + ui->lineEdit->text() + "\nPlease check your path";
            QMessageBox::warning(this, tr("XDF Streamer"), msg, QMessageBox::Ok);
        }
        else {
            ui->pushButtonStream->setEnabled(true);
            ui->pushButtonLoad->setText("Unload");
            ui->treeWidgetXDF->header()->show();
            ui->treeWidgetXDF->setColumnWidth(0, std::round(0.5 * ui->treeWidgetXDF->width()));

            /* Get first stream that is not a string stream */
            for (size_t k = 0; k < this->xdf->streams.size(); k++) {
                if (this->xdf->streams[k].info.channel_format.compare("string") != 0) {
                    ui->spinBox->setValue((int)this->xdf->streams[k].info.nominal_srate);
                    break;
                }
            }

            for (size_t k = 0; k < this->xdf->streams.size(); k++) {
                QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidgetXDF);
                item->setText(0, "Stream-" + QString::number(k+1));
                item->setCheckState(0, Qt::Unchecked);
                item->setDisabled(this->xdf->streams[k].info.channel_format.compare("string") == 0 ? true : false);

                QTreeWidgetItem *subItem = new QTreeWidgetItem(item);
                subItem->setText(0, "Stream Name");
                subItem->setText(1, QString::fromStdString(this->xdf->streams[k].info.name));
                subItem->setDisabled(this->xdf->streams[k].info.channel_format.compare("string") == 0 ? true : false);
                item->addChild(subItem);

                subItem = new QTreeWidgetItem(item);
                subItem->setText(0, "Channel Format");
                subItem->setText(1, QString::fromStdString(this->xdf->streams[k].info.channel_format));
                subItem->setDisabled(this->xdf->streams[k].info.channel_format.compare("string") == 0 ? true : false);
                item->addChild(subItem);

                subItem = new QTreeWidgetItem(item);
                subItem->setText(0, "Sampling Rate");
                subItem->setText(1, QString::number(this->xdf->streams[k].info.nominal_srate));
                subItem->setDisabled(this->xdf->streams[k].info.channel_format.compare("string") == 0 ? true : false);
                item->addChild(subItem);

                subItem = new QTreeWidgetItem(item);
                subItem->setText(0, "Channel Count");
                subItem->setText(1, QString::number(this->xdf->streams[k].info.channel_count));
                subItem->setDisabled(this->xdf->streams[k].info.channel_format.compare("string") == 0 ? true : false);
                item->addChild(subItem);

                subItem = new QTreeWidgetItem(item);
                subItem->setText(0, "Stream Type");
                subItem->setText(1, QString::fromStdString(this->xdf->streams[k].info.type));
                subItem->setDisabled(this->xdf->streams[k].info.channel_format.compare("string") == 0 ? true : false);
                item->addChild(subItem);

                ui->treeWidgetXDF->addTopLevelItem(item);
            }
            ui->treeWidgetXDF->expandAll();
        }
    }
    else {
        this->clearCache();
    }
}

void XdfStreamer::on_lineEdit_textChanged(const QString &path)
{
    if (path.isEmpty()) {
        ui->pushButtonLoad->setEnabled(false);
    }
    else {
        ui->pushButtonLoad->setEnabled(true);
    }
}

void XdfStreamer::on_pushButtonStream_clicked()
{
    if (ui->pushButtonStream->text().compare("Stream") == 0) {
        this->stop_thread = false;

        if (ui->checkBoxRandomSignal->isChecked()) {
            qDebug() << "Generating synthetic signals";

            const int samplingRate = ui->spinBox->value();
            const int channelCount = ui->spinBox_2->value();
            lsl::stream_info info = this->initializeLslStreamsForRandomData(samplingRate, channelCount);
            QSharedPointer<lsl::stream_outlet> outlet_ptr = QSharedPointer<lsl::stream_outlet>(new lsl::stream_outlet(info));

            QMessageBox::information(this, tr("Status"), tr("Lab Streaming Layer stream initialized.\n"
                                                      "Please start LabRecorder and refresh the streams\n"
                                                      "and/or RT_Receiver_GUI to run simulations."),
                                     QMessageBox::Ok, QMessageBox::Ok);

            pushThreads.push_back(new std::thread(&XdfStreamer::pushRandomData, this, outlet_ptr, samplingRate, channelCount));
        }
        else {
            if(!this->stream_ready) {
                QMessageBox::information(this, tr("Status"), tr("Please select a stream!\n"
                                                                "At least one stream needs to be selected...\n"),
                                         QMessageBox::Ok, QMessageBox::Ok);
                this->stop_thread = true;
                return;
            }

            qDebug() << "Load XDF";

            // Find the checked streams
            for (size_t i = 0; i < ui->treeWidgetXDF->topLevelItemCount(); ++i) {
                if (ui->treeWidgetXDF->topLevelItem((int)i)->checkState(0)) {
                    std::cout << "Selected " << i << std::endl;

                    const int samplingRate = this->xdf->streams[i].info.nominal_srate;
                    const int channelCount = this->xdf->streams[i].info.channel_count;

                    lsl::stream_info info = this->initializeLslStreamsForXdfData(i, samplingRate, channelCount);
                    QSharedPointer<lsl::stream_outlet> outlet_ptr = QSharedPointer<lsl::stream_outlet>(new lsl::stream_outlet(info));
                    // add thread to the list
                    pushThreads.push_back(new std::thread(&XdfStreamer::pushXdfData, this, i, outlet_ptr, samplingRate, channelCount));
                }
            }

            QMessageBox::information(this, tr("Status"), tr("Lab Streaming Layer stream initialized.\n"
                                                            "Please start LabRecorder and refresh the streams\n"
                                                            "and/or RT_Receiver_GUI to run simulations."),
                                     QMessageBox::Ok, QMessageBox::Ok);
        }

        ui->pushButtonStream->setText("Stop");
        this->enableControlPanel(false);

        ui->treeWidgetXDF->setEnabled(false);
        ui->treeWidgetRandomSignal->setEnabled(false);
    }
    else {
        this->mutex_stop_thread.lock();
        this->stop_thread = true;
        this->mutex_stop_thread.unlock();

        ui->treeWidgetXDF->setEnabled(true);
        ui->treeWidgetRandomSignal->setEnabled(true);
        ui->pushButtonStream->setText("Stream");

        for (int i = 0; i < pushThreads.size(); ++i) {
            std::cout << "Stopping thread..." << " id " << pushThreads[i]->get_id() << std::endl;
            pushThreads[i]->join();
            delete this->pushThreads[i];
        }
        pushThreads.clear();

        this->enableControlPanel(true);
    }
}

void XdfStreamer::on_treeWidgetXDF_itemClicked(QTreeWidgetItem *item)
{
    this->stream_ready = false;
    for (size_t i = 0; i < ui->treeWidgetXDF->topLevelItemCount(); ++i) {
        if (ui->treeWidgetXDF->topLevelItem((int)i)->checkState(0)) {

            // IRREGULAR_RATE = 0.0, ignoring now
            if(this->xdf->streams[i].info.nominal_srate < 1.0){
                QMessageBox::information(this, tr("Status"), tr("You select a stream that has irregular rate!\n"
                                                                "The app will ignore that stream!\n"
                                                                "This app does not suport irregular rate at the moment...\n"),
                                         QMessageBox::Ok, QMessageBox::Ok);


                // uncheck
                ui->treeWidgetXDF->topLevelItem((int)i)->setCheckState(0, Qt::Unchecked);
                continue;
            }

            this->stream_ready = true;
        }
    }
}
