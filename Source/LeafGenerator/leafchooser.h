#ifndef LEAFCHOOSER_H
#define LEAFCHOOSER_H

#include <QWidget>
#include <QtGui>
#include <QList>
#include <QTableWidget>
#include <QString>

class LeafChooser : public QWidget
{
    Q_OBJECT
public:
    explicit LeafChooser(QString name, QWidget *parent = 0);
    bool isSelected();
    QString getName();
    int getRatio();
    QSize sizeHint();
    void setSelected(bool selected);
    
signals:
    //Sends the leaf name, adds to list is true, removes if false
    void leafSelected(QString leaf, bool selected);
    void leafMousedOver(QString name);
    
private slots:
    //connects to tick box
    void leafSelectionClicked(bool isSelected);

private:
    void createWidgets();
    void enterEvent(QEvent *);

    QString leafName;

    QLabel* picture;
    QPixmap pix;
    QCheckBox* isUsed;
    QSpinBox* ratio;
};

#endif // LEAFCHOOSER_H
