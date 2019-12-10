#include <unistd.h>
#include <arpa/inet.h>

#include <qt5/QtCore/QByteArray>
#include <qt5/QtCore/QDataStream>
#include <qt5/QtNetwork/QTcpSocket>
#include <qt5/QtNetwork/QHostAddress>
#include <qt5/QtWidgets/QFileDialog>
#include <qt5/QtWidgets/QMessageBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"


struct __attribute__((packed)) hello
{
    unsigned int magic;
    char user[32];
    char file[256];
};


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    sock(new QTcpSocket),
    addr(new QHostAddress),
    file(new QString)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete file;
    delete addr;
    delete sock;
    delete ui;
}

void MainWindow::on_lineEditAddress_textEdited(const QString &newAddr)
{
    addr->setAddress(newAddr);
    ui->pushButtonSelect->setEnabled(newAddr.length());
}

void MainWindow::on_pushButtonSelect_clicked()
{
    QFileInfo info = QFileDialog::getOpenFileName(this, "Select the file...", "~");

    if (!info.fileName().length())
        return;

    if (!info.isReadable()) {
        QMessageBox box;
        box.setIcon(QMessageBox::Critical);
        box.setInformativeText("Permission denied");
        box.setText("Could not open the file");
        box.exec();
        return;
    }

    *file = info.absoluteFilePath();
    ui->pushButtonSend->setEnabled(true);
    ui->pushButtonSelect->setText(info.fileName());
    ui->progressBarUpload->setValue(ui->progressBarUpload->minimum());
}

void MainWindow::on_pushButtonSend_clicked()
{
    QByteArray name = file->toLocal8Bit();

    struct hello hello;
    hello.magic = htonl(0xdeadbeef);
    getlogin_r(hello.user, sizeof(hello.user)-1);
    strncpy(hello.file, name.data(), sizeof(hello.file));
    hello.user[sizeof(hello.user)-1] = '\0';
    hello.file[sizeof(hello.file)-1] = '\0';

    QFile f(*file);
    if (!f.open(QFile::ReadOnly))
        return;

    sock->connectToHost(*addr, 2137);
    sock->write((const char *)&hello, sizeof(hello));

    qint64 chunk = 4096, size = f.size();
    ui->progressBarUpload->setMaximum(size);
    QByteArray data = f.read(chunk);

    while (data.length()) {
        sock->write(data);
        data = f.read(chunk);
        ui->progressBarUpload->setValue(f.pos());
    }

    sock->disconnectFromHost();
    sock->close();
    f.close();
}
