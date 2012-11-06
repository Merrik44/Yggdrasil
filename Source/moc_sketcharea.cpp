/****************************************************************************
** Meta object code from reading C++ file 'sketcharea.h'
**
** Created: Tue Nov 6 19:09:01 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "LeafGenerator/sketcharea.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sketcharea.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SketchArea[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,   12,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      40,   11,   11,   11, 0x0a,
      53,   11,   11,   11, 0x0a,
      69,   61,   11,   11, 0x0a,
      94,   11,   89,   11, 0x0a,
     107,  105,   11,   11, 0x0a,
     362,  125,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_SketchArea[] = {
    "SketchArea\0\0text\0setStatusText(QString)\0"
    "clearImage()\0print()\0modeStr\0"
    "changeMode(QString)\0bool\0validate()\0"
    "i\0createLeaves(int)\0"
    "growthType,hGrowthVar,vGrowthVar,scaleVar,colourType,fCol_MMin,fCol_MM"
    "ax,fCol_AMin,fCol_AMax,bCol_MMin,bCol_MMax,bCol_AMin,bCol_AMax,specVar"
    ",bumpVar,spawnRateVar,bsVar,bvVar,kdVar,intensityVar,rootColour_min,ro"
    "otColour_max,startScaleVar\0"
    "setValues(int,QPointF*,QPointF*,QPointF*,int,QColor*,QColor*,QColor*,Q"
    "Color*,QColor*,QColor*,QColor*,QColor*,QPointF*,QPointF*,QPointF*,QPoi"
    "ntF*,QPointF*,QPointF*,QPointF*,QColor*,QColor*,QPointF*)\0"
};

void SketchArea::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SketchArea *_t = static_cast<SketchArea *>(_o);
        switch (_id) {
        case 0: _t->setStatusText((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->clearImage(); break;
        case 2: _t->print(); break;
        case 3: _t->changeMode((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: { bool _r = _t->validate();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 5: _t->createLeaves((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 6: _t->setValues((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QPointF*(*)>(_a[2])),(*reinterpret_cast< QPointF*(*)>(_a[3])),(*reinterpret_cast< QPointF*(*)>(_a[4])),(*reinterpret_cast< int(*)>(_a[5])),(*reinterpret_cast< QColor*(*)>(_a[6])),(*reinterpret_cast< QColor*(*)>(_a[7])),(*reinterpret_cast< QColor*(*)>(_a[8])),(*reinterpret_cast< QColor*(*)>(_a[9])),(*reinterpret_cast< QColor*(*)>(_a[10])),(*reinterpret_cast< QColor*(*)>(_a[11])),(*reinterpret_cast< QColor*(*)>(_a[12])),(*reinterpret_cast< QColor*(*)>(_a[13])),(*reinterpret_cast< QPointF*(*)>(_a[14])),(*reinterpret_cast< QPointF*(*)>(_a[15])),(*reinterpret_cast< QPointF*(*)>(_a[16])),(*reinterpret_cast< QPointF*(*)>(_a[17])),(*reinterpret_cast< QPointF*(*)>(_a[18])),(*reinterpret_cast< QPointF*(*)>(_a[19])),(*reinterpret_cast< QPointF*(*)>(_a[20])),(*reinterpret_cast< QColor*(*)>(_a[21])),(*reinterpret_cast< QColor*(*)>(_a[22])),(*reinterpret_cast< QPointF*(*)>(_a[23]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData SketchArea::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SketchArea::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_SketchArea,
      qt_meta_data_SketchArea, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SketchArea::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SketchArea::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SketchArea::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SketchArea))
        return static_cast<void*>(const_cast< SketchArea*>(this));
    return QWidget::qt_metacast(_clname);
}

int SketchArea::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void SketchArea::setStatusText(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
