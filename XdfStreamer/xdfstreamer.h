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
};

#endif // XDFSTREAMER_H
