#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QWidget>
#include <QtGui>
#include "sketcharea.h"
#include "paramchooser.h"

class SideBar : public QWidget
{
    Q_OBJECT
public:
    explicit SideBar(SketchArea *s, QWidget *parent = 0);
    bool setStage(const int stage);
    
public slots:
    void reset();
    void validate();
    void paramsSet();
    void paramsOk();
    void create_okClicked();
    void drawClicked();

signals:
    bool isValid();
    void changeMode(QString modeStr);

private:
    void createActions();

    SketchArea *sketchArea;

    //Boxes
    QGroupBox *draw;
    QGroupBox *params;
    QGroupBox *create;
    QVBoxLayout *layout;

    //Draw buttons
    QPushButton *draw_outline;
    QPushButton *draw_root;
    QPushButton *draw_eraser;
    QPushButton *draw_validate;

    //Param buttons
    QPushButton *params_set;
    ParamChooser *paramChooser;

    //Create buttons
    QPushButton *create_preview; //creates one sample top leaf texture
    QSpinBox *create_number;
    QPushButton *create_ok;

    //Actions
    QAction *draw_outline_Act;
    QAction *draw_root_Act;
    QAction *draw_eraser_Act;

};

#endif // SIDEBAR_H
