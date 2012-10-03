#ifndef DISPLAYLABELTHREAD_H
#define DISPLAYLABELTHREAD_H
#include "QThread"
class QLabel;
class Texture;
class Vector2;
class QImage;
class DisplayLabelThread : public QThread
{
    Q_OBJECT
  public:
    DisplayLabelThread();
  public slots:
    void slotSetLabel(Vector2 & sizeScaledPatched,  QImage & texture, QLabel * label);
//  signals:
//    void finished();
//    void error(QString err);
};
#endif // DISPLAYLABELTHREAD_H
