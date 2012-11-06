/****************************************************************************
** Meta object code from reading C++ file 'leafwindow.h'
**
** Created: Tue Nov 6 19:09:06 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "LeafGenerator/leafwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'leafwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_LeafWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x08,
      19,   11,   11,   11, 0x08,
      26,   11,   11,   11, 0x08,
      37,   11,   11,   11, 0x08,
      50,   45,   11,   11, 0x08,
      73,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_LeafWindow[] = {
    "LeafWindow\0\0open()\0save()\0penColor()\0"
    "about()\0text\0setStatusText(QString)\0"
    "foliage()\0"
};

void LeafWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        LeafWindow *_t = static_cast<LeafWindow *>(_o);
        switch (_id) {
        case 0: _t->open(); break;
        case 1: _t->save(); break;
        case 2: _t->penColor(); break;
        case 3: _t->about(); break;
        case 4: _t->setStatusText((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: _t->foliage(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData LeafWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject LeafWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_LeafWindow,
      qt_meta_data_LeafWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &LeafWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *LeafWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *LeafWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_LeafWindow))
        return static_cast<void*>(const_cast< LeafWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int LeafWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
