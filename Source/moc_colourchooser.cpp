/****************************************************************************
** Meta object code from reading C++ file 'colourchooser.h'
**
** Created: Tue Nov 6 20:01:02 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "LeafGenerator/colourchooser.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'colourchooser.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ColourChooser[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      26,   15,   14,   14, 0x0a,
      52,   14,   14,   14, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_ColourChooser[] = {
    "ColourChooser\0\0value,type\0"
    "setConstant(bool,QString)\0btnClicked()\0"
};

void ColourChooser::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ColourChooser *_t = static_cast<ColourChooser *>(_o);
        switch (_id) {
        case 0: _t->setConstant((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 1: _t->btnClicked(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ColourChooser::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ColourChooser::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ColourChooser,
      qt_meta_data_ColourChooser, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ColourChooser::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ColourChooser::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ColourChooser::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ColourChooser))
        return static_cast<void*>(const_cast< ColourChooser*>(this));
    return QWidget::qt_metacast(_clname);
}

int ColourChooser::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
