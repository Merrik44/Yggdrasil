/****************************************************************************
** Meta object code from reading C++ file 'sketchwidget.h'
**
** Created: Tue Nov 6 20:00:46 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "TreeDrawGUI/sketchwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sketchwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SketchWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x05,
      30,   13,   13,   13, 0x05,
      44,   13,   13,   13, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_SketchWidget[] = {
    "SketchWidget\0\0sketchChanged()\0"
    "sketchEmpty()\0sketchNonEmpty()\0"
};

void SketchWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SketchWidget *_t = static_cast<SketchWidget *>(_o);
        switch (_id) {
        case 0: _t->sketchChanged(); break;
        case 1: _t->sketchEmpty(); break;
        case 2: _t->sketchNonEmpty(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData SketchWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SketchWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_SketchWidget,
      qt_meta_data_SketchWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SketchWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SketchWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SketchWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SketchWidget))
        return static_cast<void*>(const_cast< SketchWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int SketchWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void SketchWidget::sketchChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void SketchWidget::sketchEmpty()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void SketchWidget::sketchNonEmpty()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
QT_END_MOC_NAMESPACE
