/****************************************************************************
** Meta object code from reading C++ file 'texturesynthesisdialog.h'
**
** Created: Tue Nov 6 20:00:49 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "TextureSynthesis/texturesynthesisdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'texturesynthesisdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_TextureSynthesisDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      24,   23,   23,   23, 0x08,
      49,   23,   23,   23, 0x08,
      76,   23,   23,   23, 0x08,
     108,  103,   23,   23, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_TextureSynthesisDialog[] = {
    "TextureSynthesisDialog\0\0"
    "on_pushButton_released()\0"
    "on_pushButton_4_released()\0"
    "on_pushButton_2_released()\0arg1\0"
    "on_spinBox_4_valueChanged(int)\0"
};

void TextureSynthesisDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        TextureSynthesisDialog *_t = static_cast<TextureSynthesisDialog *>(_o);
        switch (_id) {
        case 0: _t->on_pushButton_released(); break;
        case 1: _t->on_pushButton_4_released(); break;
        case 2: _t->on_pushButton_2_released(); break;
        case 3: _t->on_spinBox_4_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData TextureSynthesisDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject TextureSynthesisDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_TextureSynthesisDialog,
      qt_meta_data_TextureSynthesisDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &TextureSynthesisDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *TextureSynthesisDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *TextureSynthesisDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TextureSynthesisDialog))
        return static_cast<void*>(const_cast< TextureSynthesisDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int TextureSynthesisDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
