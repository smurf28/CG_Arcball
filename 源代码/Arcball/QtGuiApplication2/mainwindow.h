#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void openFile();
public slots:
     void openclicked();//¶¨Òå²Ûº¯Êý
signals:
    void func( QString );
private:
    Ui::MainWindow *ui;
    QString fileName;
};

#endif // MAINWINDOW_H