#ifndef XDFSTREAMER_H
#define XDFSTREAMER_H

#include "libxdf/xdf.h"
#include <QMainWindow>

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

private slots:
    void enableFilePicker(int status);
    void openFilePicker();
    void handleXdfFile();
    void on_lineEdit_textChanged(const QString &arg1);
    void on_pushButton_clicked();
};

#endif // XDFSTREAMER_H
