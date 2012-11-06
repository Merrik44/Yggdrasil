/********************************************************************************
** Form generated from reading UI file 'texturesynthesisdialog.ui'
**
** Created: Tue Nov 6 17:20:40 2012
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TEXTURESYNTHESISDIALOG_H
#define UI_TEXTURESYNTHESISDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_TextureSynthesisDialog
{
public:
    QPushButton *pushButton_2;
    QLabel *label_3;
    QSpinBox *spinBox_3;
    QPushButton *pushButton;
    QSpinBox *spinBox_2;
    QLabel *label_9;
    QPushButton *pushButton_4;
    QLabel *label_2;
    QSpinBox *spinBox;
    QLabel *label;
    QLabel *label_4;
    QFrame *line;
    QFrame *line_2;
    QLabel *label_5;
    QProgressBar *progressBar;
    QSpinBox *spinBox_4;
    QFrame *line_3;

    void setupUi(QDialog *TextureSynthesisDialog)
    {
        if (TextureSynthesisDialog->objectName().isEmpty())
            TextureSynthesisDialog->setObjectName(QString::fromUtf8("TextureSynthesisDialog"));
        TextureSynthesisDialog->resize(641, 451);
        pushButton_2 = new QPushButton(TextureSynthesisDialog);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(10, 170, 201, 27));
        label_3 = new QLabel(TextureSynthesisDialog);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 90, 121, 31));
        spinBox_3 = new QSpinBox(TextureSynthesisDialog);
        spinBox_3->setObjectName(QString::fromUtf8("spinBox_3"));
        spinBox_3->setGeometry(QRect(100, 60, 81, 27));
        spinBox_3->setMaximum(10000);
        spinBox_3->setValue(4);
        pushButton = new QPushButton(TextureSynthesisDialog);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(10, 10, 98, 27));
        spinBox_2 = new QSpinBox(TextureSynthesisDialog);
        spinBox_2->setObjectName(QString::fromUtf8("spinBox_2"));
        spinBox_2->setGeometry(QRect(100, 140, 81, 27));
        spinBox_2->setMaximum(10000);
        spinBox_2->setValue(30);
        label_9 = new QLabel(TextureSynthesisDialog);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(10, 120, 181, 20));
        pushButton_4 = new QPushButton(TextureSynthesisDialog);
        pushButton_4->setObjectName(QString::fromUtf8("pushButton_4"));
        pushButton_4->setGeometry(QRect(110, 10, 98, 27));
        label_2 = new QLabel(TextureSynthesisDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(230, 10, 400, 400));
        spinBox = new QSpinBox(TextureSynthesisDialog);
        spinBox->setObjectName(QString::fromUtf8("spinBox"));
        spinBox->setGeometry(QRect(10, 140, 81, 27));
        spinBox->setMaximum(10000);
        spinBox->setValue(30);
        label = new QLabel(TextureSynthesisDialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(30, 250, 160, 160));
        label_4 = new QLabel(TextureSynthesisDialog);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(10, 210, 201, 31));
        label_4->setAlignment(Qt::AlignCenter);
        line = new QFrame(TextureSynthesisDialog);
        line->setObjectName(QString::fromUtf8("line"));
        line->setGeometry(QRect(0, 200, 221, 16));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        line_2 = new QFrame(TextureSynthesisDialog);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setGeometry(QRect(210, 0, 20, 421));
        line_2->setFrameShape(QFrame::VLine);
        line_2->setFrameShadow(QFrame::Sunken);
        label_5 = new QLabel(TextureSynthesisDialog);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(10, 60, 121, 31));
        progressBar = new QProgressBar(TextureSynthesisDialog);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setGeometry(QRect(10, 420, 621, 23));
        progressBar->setValue(24);
        spinBox_4 = new QSpinBox(TextureSynthesisDialog);
        spinBox_4->setObjectName(QString::fromUtf8("spinBox_4"));
        spinBox_4->setGeometry(QRect(100, 90, 81, 27));
        spinBox_4->setMaximum(10000);
        spinBox_4->setValue(11);
        line_3 = new QFrame(TextureSynthesisDialog);
        line_3->setObjectName(QString::fromUtf8("line_3"));
        line_3->setGeometry(QRect(0, 40, 221, 16));
        line_3->setFrameShape(QFrame::HLine);
        line_3->setFrameShadow(QFrame::Sunken);

        retranslateUi(TextureSynthesisDialog);

        QMetaObject::connectSlotsByName(TextureSynthesisDialog);
    } // setupUi

    void retranslateUi(QDialog *TextureSynthesisDialog)
    {
        TextureSynthesisDialog->setWindowTitle(QApplication::translate("TextureSynthesisDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("TextureSynthesisDialog", "Synthesise new image", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("TextureSynthesisDialog", "Quality", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("TextureSynthesisDialog", "Load Image", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("TextureSynthesisDialog", " Width               Height", 0, QApplication::UnicodeUTF8));
        pushButton_4->setText(QApplication::translate("TextureSynthesisDialog", "Save Image", 0, QApplication::UnicodeUTF8));
        label_2->setText(QString());
        label->setText(QString());
        label_4->setText(QApplication::translate("TextureSynthesisDialog", "Sample Image", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("TextureSynthesisDialog", "Iterations", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class TextureSynthesisDialog: public Ui_TextureSynthesisDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEXTURESYNTHESISDIALOG_H
