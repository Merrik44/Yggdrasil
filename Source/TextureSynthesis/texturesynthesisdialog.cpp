#include "texturesynthesisdialog.h"
#include "ui_texturesynthesisdialog.h"

TextureSynthesisDialog::TextureSynthesisDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TextureSynthesisDialog)
{
    ui->setupUi(this);
}

TextureSynthesisDialog::~TextureSynthesisDialog()
{
    delete ui;
}
