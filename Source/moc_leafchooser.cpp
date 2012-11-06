/****************************************************************************
** Meta object code from reading C++ file 'leafchooser.h'
**
** Created: Tue Nov 6 19:09:08 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "LeafGenerator/leafchooser.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'leafchooser.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_LeafChooser[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      27,   13,   12,   12, 0x05,
      59,   54,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      94,   83,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_LeafChooser[] = {
    "LeafChooser\0\0leaf,selected\0"
    "leafSelected(QString,bool)\0name\0"
    "leafMousedOver(QString)\0isSelected\0"
    "leafSelectionClicked(bool)\0"
};

void LeafChooser::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        LeafChooser *_t = static_cast<LeafChooser *>(_o);
        switch (_id) {
        case 0: _t->leafSelected((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 1: _t->leafMousedOver((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->leafSelectionClicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData LeafChooser::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject LeafChooser::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_LeafChooser,
      qt_meta_data_LeafChooser, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &LeafChooser::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *LeafChooser::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *LeafChooser::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_LeafChooser))
        return static_cast<void*>(const_cast< LeafChooser*>(this));
    return QWidget::qt_metacast(_clname);
}

int LeafChooser::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void LeafChooser::leafSelected(QString _t1, bool _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void LeafChooser::leafMousedOver(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
