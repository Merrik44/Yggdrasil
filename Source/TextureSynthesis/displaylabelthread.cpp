#include "displaylabelthread.h"
#include "QLabel"
#include "texture.h"
#include "vector2.h"

DisplayLabelThread::DisplayLabelThread()
{
    
}

//DisplayLabelThread::DisplayLabelThread()
void DisplayLabelThread::slotSetLabel(Vector2 & sizeScaledPatched, QImage & texture, QLabel * label) 
{
   //==============================Display Logic=========================================
   
   QSize sizeScaledPatchedTemp = QSize(sizeScaledPatched.x,sizeScaledPatched.y);
   
   Qt::AspectRatioMode aspectRatioModePatched = Qt::IgnoreAspectRatio;
   Qt::TransformationMode transformModePatched = Qt::FastTransformation;
   //cout<<"here1"<<endl;

   QImage scaledPatched = texture.scaled(sizeScaledPatchedTemp, aspectRatioModePatched,transformModePatched);
   //cout<<"here2"<<endl;

   label->setFixedWidth(scaledPatched.size().width());
   label->setFixedHeight(scaledPatched.size().height());

    //cout<<"here3"<<endl;
   label->setPixmap(QPixmap::fromImage(scaledPatched));
   
   label->repaint();
   emit finished();
}
