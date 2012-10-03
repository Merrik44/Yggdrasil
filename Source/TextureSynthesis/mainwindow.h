#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QImage>
class DisplayLabelThread;
namespace Ui {
class MainWindow;
}
class TextureSynthesis;
class Vector2;
class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    DisplayLabelThread* worker;
    
    void scaleAndDisplayImage(Vector2& sizeScaledPatched, QImage & texture, QLabel* label);
    
    
private slots:
    void on_pushButton_released();

    void on_pushButton_2_clicked();

    void on_pushButton_2_released();

    void on_horizontalSlider_valueChanged(int value);
    
    void on_pushButton_4_released();
    
private:
    Ui::MainWindow *ui;
    TextureSynthesis * synthesis;
    
};

#endif // MAINWINDOW_H
