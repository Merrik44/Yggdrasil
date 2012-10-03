#ifndef TEXTURESYNTHESISDIALOG_H
#define TEXTURESYNTHESISDIALOG_H

#include <QDialog>

namespace Ui {
class TextureSynthesisDialog;
}

class TextureSynthesisDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit TextureSynthesisDialog(QWidget *parent = 0);
    ~TextureSynthesisDialog();
    
private:
    Ui::TextureSynthesisDialog *ui;
};

#endif // TEXTURESYNTHESISDIALOG_H
