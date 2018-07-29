#ifndef XDFSTREAMER_H
#define XDFSTREAMER_H

#include "libxdf/xdf.h"
#include <QMainWindow>
#include <thread>
#include <mutex>

namespace Ui {
class XdfStreamer;
}

class XdfStreamer : public QMainWindow
{
    Q_OBJECT

public:
    explicit XdfStreamer(QWidget *parent = nullptr);
    ~XdfStreamer();

private:
    Ui::XdfStreamer *ui;
    QSharedPointer<Xdf> xdf;
    std::thread *pushThread = nullptr;
    bool stop_thread = false;
    std::mutex mutex_stop_thread;

    void pushRandomSamples();

private slots:
    void on_checkBox_stateChanged(int status);
    void openFilePicker();
    void handleXdfFile();
    void on_lineEdit_textChanged(const QString &arg1);
    void on_pushButton_clicked();
};

#endif // XDFSTREAMER_H
