#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>
#include <QtGui>

class OptionsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit OptionsDialog(QWidget *parent = 0);
    ~OptionsDialog();
    void setValues(int value1, int value2, int value3, int value4, int value5);

signals:
    void valuesAccepted(int value1, int value2, int value3, int value4, int value5);

public slots:
    void checked1(int state);
    void checked2(int state);
    void checked3(int state);
    void updated1(int value);
    void updated2(int value);
    void updated3(int value);

    void set();
    void close();



private:
    QLabel label1,label2,label3,label4;
    QCheckBox enabled1, enabled2, enabled3, enabled4;
    QSlider option1, option2, option3;
    QLabel value1,value2,value3, value4;
    QGridLayout *frameLayout;
    QGridLayout *layout;

    QPushButton ok,cancel;


    QScrollArea texArea;
    QGridLayout *texLayout;
    QButtonGroup texGroup;
    QStringList textureList;
    QLabel textureHeading;
    QLabel settingsHeading;

    int textureIndex;




};

#endif // OPTIONSDIALOG_H
