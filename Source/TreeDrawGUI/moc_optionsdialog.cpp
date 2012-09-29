/****************************************************************************
** Meta object code from reading C++ file 'optionsdialog.h'
**
** Created: Mon Aug 27 22:11:00 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "optionsdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'optionsdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_OptionsDialog[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      50,   15,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      92,   86,   14,   14, 0x0a,
     106,   86,   14,   14, 0x0a,
     120,   86,   14,   14, 0x0a,
     140,  134,   14,   14, 0x0a,
     154,  134,   14,   14, 0x0a,
     168,  134,   14,   14, 0x0a,
     182,   14,   14,   14, 0x0a,
     188,   14,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_OptionsDialog[] = {
    "OptionsDialog\0\0value1,value2,value3,value4,value5\0"
    "valuesAccepted(int,int,int,int,int)\0"
    "state\0checked1(int)\0checked2(int)\0"
    "checked3(int)\0value\0updated1(int)\0"
    "updated2(int)\0updated3(int)\0set()\0"
    "close()\0"
};

const QMetaObject OptionsDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_OptionsDialog,
      qt_meta_data_OptionsDialog, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &OptionsDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *OptionsDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *OptionsDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_OptionsDialog))
        return static_cast<void*>(const_cast< OptionsDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int OptionsDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: valuesAccepted((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< int(*)>(_a[5]))); break;
        case 1: checked1((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: checked2((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: checked3((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: updated1((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: updated2((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: updated3((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: set(); break;
        case 8: close(); break;
        default: ;
        }
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void OptionsDialog::valuesAccepted(int _t1, int _t2, int _t3, int _t4, int _t5)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
