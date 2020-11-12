#ifndef XDFSTREAMER_H
#define XDFSTREAMER_H

#include "xdf.h"
#include "lsl_cpp.h"
#include <QMainWindow>
#include <QTreeWidget>
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
    std::thread *pushThread = Q_NULLPTR;
    bool stop_thread = false;
    std::mutex mutex_stop_thread;
    bool stream_ready = false;
    size_t stream_idx = -1;

    void pushRandomData(QSharedPointer<lsl::stream_outlet> outlet_ptr, const int samplingRate, const int channelCount);
    void pushXdfData(QSharedPointer<lsl::stream_outlet> outlet_ptr, const int samplingRate, const int channelCount);
    void clearCache();
    void enableControlPanel(bool enabled);
    lsl::stream_info initializeLslStreamsForRandomData(const int samplingRate, const int channelCount);
    lsl::stream_info initializeLslStreamsForXdfData(const int samplingRate, const int channelCount);

private slots:
    void on_checkBox_stateChanged(int status);
    void on_toolButton_clicked();
    void on_pushButton_2_clicked();
    void on_lineEdit_textChanged(const QString &arg1);
    void on_pushButton_clicked();
    void on_treeWidget_itemClicked(QTreeWidgetItem *item);
};

#endif // XDFSTREAMER_H
