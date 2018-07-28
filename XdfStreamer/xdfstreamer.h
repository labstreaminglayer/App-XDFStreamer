#ifndef XDFSTREAMER_H
#define XDFSTREAMER_H

#include <QMainWindow>

namespace Ui {
class XdfStreamer;
}

class XdfStreamer : public QMainWindow
{
    Q_OBJECT

public:
    explicit XdfStreamer(QWidget *parent = 0);
    ~XdfStreamer();

private:
    Ui::XdfStreamer *ui;

private slots:
    void enableFilePicker(int status);
    void openFilePicker();
};

#endif // XDFSTREAMER_H
