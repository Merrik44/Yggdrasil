/****************************************************************************
** Meta object code from reading C++ file 'displaylabelthread.h'
**
** Created: Tue Nov 6 17:20:47 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../displaylabelthread.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'displaylabelthread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DisplayLabelThread[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      52,   20,   19,   19, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_DisplayLabelThread[] = {
    "DisplayLabelThread\0\0sizeScaledPatched,texture,label\0"
    "slotSetLabel(Vector2&,QImage&,QLabel*)\0"
};

const QMetaObject DisplayLabelThread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_DisplayLabelThread,
      qt_meta_data_DisplayLabelThread, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DisplayLabelThread::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DisplayLabelThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DisplayLabelThread::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DisplayLabelThread))
        return static_cast<void*>(const_cast< DisplayLabelThread*>(this));
    return QThread::qt_metacast(_clname);
}

int DisplayLabelThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slotSetLabel((*reinterpret_cast< Vector2(*)>(_a[1])),(*reinterpret_cast< QImage(*)>(_a[2])),(*reinterpret_cast< QLabel*(*)>(_a[3]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
