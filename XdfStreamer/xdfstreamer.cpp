#include "xdfstreamer.h"
#include "ui_xdfstreamer.h"

XdfStreamer::XdfStreamer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::XdfStreamer)
{
    ui->setupUi(this);
}

XdfStreamer::~XdfStreamer()
{
    delete ui;
}
