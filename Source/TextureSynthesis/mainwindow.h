#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}
class TextureSynthesisDialog;
class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
    
    
private slots:
    void on_pushButton_released();
    
private:
    Ui::MainWindow *ui;
    TextureSynthesisDialog * dialog;
    
};

#endif // MAINWINDOW_H
