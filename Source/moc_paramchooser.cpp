/****************************************************************************
** Meta object code from reading C++ file 'paramchooser.h'
**
** Created: Mon Nov 5 21:27:20 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "LeafGenerator/paramchooser.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'paramchooser.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ParamChooser[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      30,   14,   13,   13, 0x05,
     296,   59,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
     497,   13,   13,   13, 0x08,
     518,  509,   13,   13, 0x08,
     545,  509,   13,   13, 0x08,
     572,   13,   13,   13, 0x08,
     583,   13,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ParamChooser[] = {
    "ParamChooser\0\0isConstant,type\0"
    "colourConstant(bool,QString)\0"
    "growthType,hGrowthVar,vGrowthVar,scaleVar,colourType,fCol_MMin,fCol_MM"
    "ax,fCol_AMin,fCol_AMax,bCol_MMin,bCol_MMax,bCol_AMin,bCol_AMax,specVar"
    ",bumpVar,spawnRateVar,bsVar,bvVar,kdVar,intensityVar,rootColour_min,ro"
    "otColour_max,startScaleVar\0"
    "returnValues(int,QPointF*,QPointF*,QPointF*,int,QColor*,QColor*,QColor"
    "*,QColor*,QColor*,QColor*,QColor*,QColor*,QPointF*,QPointF*,QPointF*,Q"
    "PointF*,QPointF*,QPointF*,QPointF*,QColor*,QColor*,QPointF*)\0"
    "okClicked()\0typeName\0colourTypeChanged(QString)\0"
    "growthTypeChanged(QString)\0saveFile()\0"
    "loadFile()\0"
};

void ParamChooser::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ParamChooser *_t = static_cast<ParamChooser *>(_o);
        switch (_id) {
        case 0: _t->colourConstant((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 1: _t->returnValues((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QPointF*(*)>(_a[2])),(*reinterpret_cast< QPointF*(*)>(_a[3])),(*reinterpret_cast< QPointF*(*)>(_a[4])),(*reinterpret_cast< int(*)>(_a[5])),(*reinterpret_cast< QColor*(*)>(_a[6])),(*reinterpret_cast< QColor*(*)>(_a[7])),(*reinterpret_cast< QColor*(*)>(_a[8])),(*reinterpret_cast< QColor*(*)>(_a[9])),(*reinterpret_cast< QColor*(*)>(_a[10])),(*reinterpret_cast< QColor*(*)>(_a[11])),(*reinterpret_cast< QColor*(*)>(_a[12])),(*reinterpret_cast< QColor*(*)>(_a[13])),(*reinterpret_cast< QPointF*(*)>(_a[14])),(*reinterpret_cast< QPointF*(*)>(_a[15])),(*reinterpret_cast< QPointF*(*)>(_a[16])),(*reinterpret_cast< QPointF*(*)>(_a[17])),(*reinterpret_cast< QPointF*(*)>(_a[18])),(*reinterpret_cast< QPointF*(*)>(_a[19])),(*reinterpret_cast< QPointF*(*)>(_a[20])),(*reinterpret_cast< QColor*(*)>(_a[21])),(*reinterpret_cast< QColor*(*)>(_a[22])),(*reinterpret_cast< QPointF*(*)>(_a[23]))); break;
        case 2: _t->okClicked(); break;
        case 3: _t->colourTypeChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: _t->growthTypeChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: _t->saveFile(); break;
        case 6: _t->loadFile(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ParamChooser::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ParamChooser::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ParamChooser,
      qt_meta_data_ParamChooser, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ParamChooser::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ParamChooser::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ParamChooser::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ParamChooser))
        return static_cast<void*>(const_cast< ParamChooser*>(this));
    return QDialog::qt_metacast(_clname);
}

int ParamChooser::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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
void ParamChooser::colourConstant(bool _t1, QString _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ParamChooser::returnValues(int _t1, QPointF * _t2, QPointF * _t3, QPointF * _t4, int _t5, QColor * _t6, QColor * _t7, QColor * _t8, QColor * _t9, QColor * _t10, QColor * _t11, QColor * _t12, QColor * _t13, QPointF * _t14, QPointF * _t15, QPointF * _t16, QPointF * _t17, QPointF * _t18, QPointF * _t19, QPointF * _t20, QColor * _t21, QColor * _t22, QPointF * _t23)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)), const_cast<void*>(reinterpret_cast<const void*>(&_t7)), const_cast<void*>(reinterpret_cast<const void*>(&_t8)), const_cast<void*>(reinterpret_cast<const void*>(&_t9)), const_cast<void*>(reinterpret_cast<const void*>(&_t10)), const_cast<void*>(reinterpret_cast<const void*>(&_t11)), const_cast<void*>(reinterpret_cast<const void*>(&_t12)), const_cast<void*>(reinterpret_cast<const void*>(&_t13)), const_cast<void*>(reinterpret_cast<const void*>(&_t14)), const_cast<void*>(reinterpret_cast<const void*>(&_t15)), const_cast<void*>(reinterpret_cast<const void*>(&_t16)), const_cast<void*>(reinterpret_cast<const void*>(&_t17)), const_cast<void*>(reinterpret_cast<const void*>(&_t18)), const_cast<void*>(reinterpret_cast<const void*>(&_t19)), const_cast<void*>(reinterpret_cast<const void*>(&_t20)), const_cast<void*>(reinterpret_cast<const void*>(&_t21)), const_cast<void*>(reinterpret_cast<const void*>(&_t22)), const_cast<void*>(reinterpret_cast<const void*>(&_t23)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
