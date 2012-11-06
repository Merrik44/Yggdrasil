/****************************************************************************
** Meta object code from reading C++ file 'foliagechooser.h'
**
** Created: Tue Nov 6 17:47:00 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "LeafGenerator/foliagechooser.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'foliagechooser.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_FoliageChooser[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
     100,   16,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
     174,   15,   15,   15, 0x08,
     191,  186,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_FoliageChooser[] = {
    "FoliageChooser\0\0"
    "leaves,ratios,numberLeavesMin,numberLeavesMax,spreadMin,spreadMax,scal"
    "eMin,scaleMax\0"
    "returnValues(QList<QString>*,QList<int>*,int,int,float,float,float,flo"
    "at)\0"
    "okClicked()\0name\0leafMouseOver(QString)\0"
};

const QMetaObject FoliageChooser::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_FoliageChooser,
      qt_meta_data_FoliageChooser, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FoliageChooser::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FoliageChooser::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FoliageChooser::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FoliageChooser))
        return static_cast<void*>(const_cast< FoliageChooser*>(this));
    return QWidget::qt_metacast(_clname);
}

int FoliageChooser::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: returnValues((*reinterpret_cast< QList<QString>*(*)>(_a[1])),(*reinterpret_cast< QList<int>*(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< float(*)>(_a[5])),(*reinterpret_cast< float(*)>(_a[6])),(*reinterpret_cast< float(*)>(_a[7])),(*reinterpret_cast< float(*)>(_a[8]))); break;
        case 1: okClicked(); break;
        case 2: leafMouseOver((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void FoliageChooser::returnValues(QList<QString> * _t1, QList<int> * _t2, int _t3, int _t4, float _t5, float _t6, float _t7, float _t8)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)), const_cast<void*>(reinterpret_cast<const void*>(&_t7)), const_cast<void*>(reinterpret_cast<const void*>(&_t8)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
