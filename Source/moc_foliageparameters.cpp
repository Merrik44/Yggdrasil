/****************************************************************************
** Meta object code from reading C++ file 'foliageparameters.h'
**
** Created: Sun Nov 4 17:12:13 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "LeafGenerator/foliageparameters.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'foliageparameters.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_FoliageParameters[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x05,

 // slots: signature, parameters, type, tag, flags
     120,   36,   18,   18, 0x0a,
     201,   18,  196,   18, 0x0a,
     229,  225,  213,   18, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_FoliageParameters[] = {
    "FoliageParameters\0\0foliageChanged()\0"
    "leaves,ratios,numberLeavesMin,numberLeavesMax,spreadMin,spreadMax,scal"
    "eMin,scaleMax\0"
    "valuesReturned(QList<QString>*,QList<int>*,int,int,float,float,float,f"
    "loat)\0"
    "bool\0setValues()\0std::string\0lst\0"
    "createMesh(QString)\0"
};

void FoliageParameters::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        FoliageParameters *_t = static_cast<FoliageParameters *>(_o);
        switch (_id) {
        case 0: _t->foliageChanged(); break;
        case 1: _t->valuesReturned((*reinterpret_cast< QList<QString>*(*)>(_a[1])),(*reinterpret_cast< QList<int>*(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< float(*)>(_a[5])),(*reinterpret_cast< float(*)>(_a[6])),(*reinterpret_cast< float(*)>(_a[7])),(*reinterpret_cast< float(*)>(_a[8]))); break;
        case 2: { bool _r = _t->setValues();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 3: { std::string _r = _t->createMesh((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< std::string*>(_a[0]) = _r; }  break;
        default: ;
        }
    }
}

const QMetaObjectExtraData FoliageParameters::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FoliageParameters::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_FoliageParameters,
      qt_meta_data_FoliageParameters, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FoliageParameters::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FoliageParameters::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FoliageParameters::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FoliageParameters))
        return static_cast<void*>(const_cast< FoliageParameters*>(this));
    return QObject::qt_metacast(_clname);
}

int FoliageParameters::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void FoliageParameters::foliageChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
