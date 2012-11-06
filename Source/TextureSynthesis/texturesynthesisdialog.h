#ifndef TEXTURESYNTHESISDIALOG_H
#define TEXTURESYNTHESISDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QImage>
class TextureSynthesis;
class Vector2;

class DisplayLabelThread;
namespace Ui {
class TextureSynthesisDialog;
}

class TextureSynthesisDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit TextureSynthesisDialog(QWidget *parent = 0);
    ~TextureSynthesisDialog();
    DisplayLabelThread* worker;
    
    void scaleAndDisplayImage(Vector2& sizeScaledPatched, QImage & texture, QLabel* label);
    void setPreviewImage(QImage & image);
    
private slots:
    void on_pushButton_released();
    
    void on_pushButton_4_released();
    
    void on_pushButton_2_released();
    
    void on_spinBox_4_valueChanged(int arg1);
    
private:
    Ui::TextureSynthesisDialog *ui;
    TextureSynthesis * synthesis;
};

#endif // TEXTURESYNTHESISDIALOG_H
