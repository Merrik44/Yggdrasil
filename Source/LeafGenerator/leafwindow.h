#ifndef LEAFWINDOW_H
#define LEAFWINDOW_H

#include <QMainWindow>
#include <QList>
#include "foliagechooser.h"

class SketchArea;
class SideBar;

class LeafWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    LeafWindow();
    ~LeafWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void open();
    void save();
    void penColor();
    void about();
    void setStatusText(QString text);
    void foliage();

private:
    void createActions();
    void createMenus();
    bool saveOnExit();
    bool saveFile(const QByteArray &fileFormat);

    SketchArea *sketchArea;
    SideBar *sideBar;

    QMenu *saveAsMenu;
    QMenu *fileMenu;
    QMenu *optionMenu;
    QMenu *helpMenu;

    QAction *newAct;
    QAction *openAct;
    QList<QAction *> saveAsActs;
    QAction *exitAct;
    QAction *penColorAct;
    QAction *printAct;
    QAction *clearScreenAct;
    QAction *aboutAct;
    QAction *foliageAct;
};

#endif // LEAFWINDOW_H
