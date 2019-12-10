#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QHostAddress>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_lineEditAddress_textEdited(const QString &newAddr);
    void on_pushButtonSelect_clicked();
    void on_pushButtonSend_clicked();

private:
    Ui::MainWindow 	*ui;
    QTcpSocket 		*sock;
    QHostAddress 	*addr;
    QString 		*file;
};

#endif // MAINWINDOW_H
