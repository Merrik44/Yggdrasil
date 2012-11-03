/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Fri Oct 26 15:25:32 2012
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionOpen_XML_file;
    QAction *actionSave_XML_file;
    QAction *actionExit;
    QAction *actionDisplay_XML_file;
    QAction *actionDisplay_LST_file;
    QAction *actionGenerate_from_current;
    QAction *actionGenerate_output_for_XML_file;
    QAction *actionUndo;
    QAction *actionRedo;
    QAction *actionExport_Cylinder_Model_to_OBJ;
    QAction *actionBlack_background;
    QAction *actionWhite_background;
    QAction *actionOpen_XML_file_2;
    QAction *Export_to_Mesh_Model_to_OBJ;
    QAction *actionCreate_Leaves;
    QAction *actionCreate_Foliage;
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuDisplay;
    QMenu *menuEdit;
    QMenu *menuSynthesise_Texture;
    QMenu *menuGenerate_Leaves;
    QMenu *menuDafasdf;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(633, 300);
        actionOpen_XML_file = new QAction(MainWindow);
        actionOpen_XML_file->setObjectName(QString::fromUtf8("actionOpen_XML_file"));
        actionSave_XML_file = new QAction(MainWindow);
        actionSave_XML_file->setObjectName(QString::fromUtf8("actionSave_XML_file"));
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionDisplay_XML_file = new QAction(MainWindow);
        actionDisplay_XML_file->setObjectName(QString::fromUtf8("actionDisplay_XML_file"));
        actionDisplay_LST_file = new QAction(MainWindow);
        actionDisplay_LST_file->setObjectName(QString::fromUtf8("actionDisplay_LST_file"));
        actionGenerate_from_current = new QAction(MainWindow);
        actionGenerate_from_current->setObjectName(QString::fromUtf8("actionGenerate_from_current"));
        actionGenerate_output_for_XML_file = new QAction(MainWindow);
        actionGenerate_output_for_XML_file->setObjectName(QString::fromUtf8("actionGenerate_output_for_XML_file"));
        actionUndo = new QAction(MainWindow);
        actionUndo->setObjectName(QString::fromUtf8("actionUndo"));
        actionRedo = new QAction(MainWindow);
        actionRedo->setObjectName(QString::fromUtf8("actionRedo"));
        actionExport_Cylinder_Model_to_OBJ = new QAction(MainWindow);
        actionExport_Cylinder_Model_to_OBJ->setObjectName(QString::fromUtf8("actionExport_Cylinder_Model_to_OBJ"));
        actionBlack_background = new QAction(MainWindow);
        actionBlack_background->setObjectName(QString::fromUtf8("actionBlack_background"));
        actionBlack_background->setCheckable(true);
        actionBlack_background->setChecked(true);
        actionWhite_background = new QAction(MainWindow);
        actionWhite_background->setObjectName(QString::fromUtf8("actionWhite_background"));
        actionWhite_background->setCheckable(true);
        actionOpen_XML_file_2 = new QAction(MainWindow);
        actionOpen_XML_file_2->setObjectName(QString::fromUtf8("actionOpen_XML_file_2"));
        Export_to_Mesh_Model_to_OBJ = new QAction(MainWindow);
        Export_to_Mesh_Model_to_OBJ->setObjectName(QString::fromUtf8("Export_to_Mesh_Model_to_OBJ"));
        actionCreate_Leaves = new QAction(MainWindow);
        actionCreate_Leaves->setObjectName(QString::fromUtf8("actionCreate_Leaves"));
        actionCreate_Foliage = new QAction(MainWindow);
        actionCreate_Foliage->setObjectName(QString::fromUtf8("actionCreate_Foliage"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 633, 25));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuDisplay = new QMenu(menuBar);
        menuDisplay->setObjectName(QString::fromUtf8("menuDisplay"));
        menuEdit = new QMenu(menuBar);
        menuEdit->setObjectName(QString::fromUtf8("menuEdit"));
        menuSynthesise_Texture = new QMenu(menuBar);
        menuSynthesise_Texture->setObjectName(QString::fromUtf8("menuSynthesise_Texture"));
        menuGenerate_Leaves = new QMenu(menuBar);
        menuGenerate_Leaves->setObjectName(QString::fromUtf8("menuGenerate_Leaves"));
        menuDafasdf = new QMenu(menuBar);
        menuDafasdf->setObjectName(QString::fromUtf8("menuDafasdf"));
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuEdit->menuAction());
        menuBar->addAction(menuDisplay->menuAction());
        menuBar->addAction(menuSynthesise_Texture->menuAction());
        menuBar->addAction(menuGenerate_Leaves->menuAction());
        menuBar->addAction(menuDafasdf->menuAction());
        menuFile->addAction(actionOpen_XML_file);
        menuFile->addAction(actionSave_XML_file);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuDisplay->addAction(actionDisplay_XML_file);
        menuDisplay->addAction(actionDisplay_LST_file);
        menuDisplay->addSeparator();
        menuDisplay->addAction(actionGenerate_from_current);
        menuDisplay->addAction(actionExport_Cylinder_Model_to_OBJ);
        menuDisplay->addSeparator();
        menuDisplay->addAction(actionBlack_background);
        menuDisplay->addAction(actionWhite_background);
        menuEdit->addAction(actionUndo);
        menuEdit->addAction(actionRedo);
        menuGenerate_Leaves->addAction(actionCreate_Leaves);
        menuGenerate_Leaves->addAction(actionCreate_Foliage);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        actionOpen_XML_file->setText(QApplication::translate("MainWindow", "Open XML file", 0, QApplication::UnicodeUTF8));
        actionSave_XML_file->setText(QApplication::translate("MainWindow", "Save XML file", 0, QApplication::UnicodeUTF8));
        actionExit->setText(QApplication::translate("MainWindow", "Exit", 0, QApplication::UnicodeUTF8));
        actionDisplay_XML_file->setText(QApplication::translate("MainWindow", "Display XML file", 0, QApplication::UnicodeUTF8));
        actionDisplay_LST_file->setText(QApplication::translate("MainWindow", "Display LST file", 0, QApplication::UnicodeUTF8));
        actionGenerate_from_current->setText(QApplication::translate("MainWindow", "Generate from current sketch", 0, QApplication::UnicodeUTF8));
        actionGenerate_output_for_XML_file->setText(QApplication::translate("MainWindow", "Generate output for XML file", 0, QApplication::UnicodeUTF8));
        actionUndo->setText(QApplication::translate("MainWindow", "Undo", 0, QApplication::UnicodeUTF8));
        actionRedo->setText(QApplication::translate("MainWindow", "Redo", 0, QApplication::UnicodeUTF8));
        actionExport_Cylinder_Model_to_OBJ->setText(QApplication::translate("MainWindow", "Export model in view as OBJ", 0, QApplication::UnicodeUTF8));
        actionBlack_background->setText(QApplication::translate("MainWindow", "Black background", 0, QApplication::UnicodeUTF8));
        actionWhite_background->setText(QApplication::translate("MainWindow", "White background", 0, QApplication::UnicodeUTF8));
        actionOpen_XML_file_2->setText(QApplication::translate("MainWindow", "Open XML file", 0, QApplication::UnicodeUTF8));
        Export_to_Mesh_Model_to_OBJ->setText(QApplication::translate("MainWindow", "Export to Mesh Model to OBJ", 0, QApplication::UnicodeUTF8));
        actionCreate_Leaves->setText(QApplication::translate("MainWindow", "Create Leaves", 0, QApplication::UnicodeUTF8));
        actionCreate_Foliage->setText(QApplication::translate("MainWindow", "Create Foliage", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));
        menuDisplay->setTitle(QApplication::translate("MainWindow", "Display", 0, QApplication::UnicodeUTF8));
        menuEdit->setTitle(QApplication::translate("MainWindow", "Edit", 0, QApplication::UnicodeUTF8));
        menuSynthesise_Texture->setTitle(QApplication::translate("MainWindow", "Synthesise Texture", 0, QApplication::UnicodeUTF8));
        menuGenerate_Leaves->setTitle(QApplication::translate("MainWindow", "Generate Leaves", 0, QApplication::UnicodeUTF8));
        menuDafasdf->setTitle(QApplication::translate("MainWindow", "dafasdf", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
