#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "texturesynthesisdialog.h"
using namespace std;
bool t = true;



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    dialog = new TextureSynthesisDialog();
    
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_released()
{
    dialog->show();
}
