/********************************************************************************
** Form generated from reading UI file 'texturesynthesisdialog.ui'
**
** Created: Mon Nov 5 21:25:53 2012
**      by: Qt User Interface Compiler version 4.8.1
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
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>

QT_BEGIN_NAMESPACE

class Ui_TextureSynthesisDialog
{
public:
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *TextureSynthesisDialog)
    {
        if (TextureSynthesisDialog->objectName().isEmpty())
            TextureSynthesisDialog->setObjectName(QString::fromUtf8("TextureSynthesisDialog"));
        TextureSynthesisDialog->resize(400, 300);
        buttonBox = new QDialogButtonBox(TextureSynthesisDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(30, 240, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        retranslateUi(TextureSynthesisDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), TextureSynthesisDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), TextureSynthesisDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(TextureSynthesisDialog);
    } // setupUi

    void retranslateUi(QDialog *TextureSynthesisDialog)
    {
        TextureSynthesisDialog->setWindowTitle(QApplication::translate("TextureSynthesisDialog", "Dialog", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class TextureSynthesisDialog: public Ui_TextureSynthesisDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEXTURESYNTHESISDIALOG_H
