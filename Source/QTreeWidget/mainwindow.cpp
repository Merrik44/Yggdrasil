#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    display = new QTreeDisplayWidget(this->centralWidget());
    display->setGeometry(10,40,this->width()-20,this->height()-50);
    display->show();
    display->setMouseTracking(true);
    display->updateGL();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_released()
{
    // Get file path to open
    QString path = QFileDialog::getOpenFileName(
            this,
            "LST File",
            QDir::currentPath(),
            "LST file (*.lst);;All Files (*.*)");

    if (!path.isNull())
    {
        display->setSourceFile(path.toStdString().c_str());
        display->repaint();
    }
}

void MainWindow::resizeEvent(QResizeEvent *i_pEvent)
{
    display->setGeometry(10,40,this->width()-20,this->height()-50);
}
