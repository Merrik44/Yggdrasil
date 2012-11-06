/****************************************************************************
** Meta object code from reading C++ file 'foliageparameters.h'
**
** Created: Tue Nov 6 17:46:58 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "LeafGenerator/foliageparameters.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'foliageparameters.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_FoliageParameters[] = {

 // content:
       5,       // revision
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

const QMetaObject FoliageParameters::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_FoliageParameters,
      qt_meta_data_FoliageParameters, 0 }
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
        switch (_id) {
        case 0: foliageChanged(); break;
        case 1: valuesReturned((*reinterpret_cast< QList<QString>*(*)>(_a[1])),(*reinterpret_cast< QList<int>*(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< float(*)>(_a[5])),(*reinterpret_cast< float(*)>(_a[6])),(*reinterpret_cast< float(*)>(_a[7])),(*reinterpret_cast< float(*)>(_a[8]))); break;
        case 2: { bool _r = setValues();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 3: { std::string _r = createMesh((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< std::string*>(_a[0]) = _r; }  break;
        default: ;
        }
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
