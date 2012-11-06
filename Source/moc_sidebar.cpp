/****************************************************************************
** Meta object code from reading C++ file 'sidebar.h'
**
** Created: Mon Nov 5 21:27:19 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "LeafGenerator/sidebar.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sidebar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SideBar[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      14,    8,    9,    8, 0x05,
      32,   24,    8,    8, 0x05,

 // slots: signature, parameters, type, tag, flags
      52,    8,    8,    8, 0x0a,
      60,    8,    8,    8, 0x0a,
      71,    8,    8,    8, 0x0a,
      83,    8,    8,    8, 0x0a,
      94,    8,    8,    8, 0x0a,
     113,    8,    8,    8, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_SideBar[] = {
    "SideBar\0\0bool\0isValid()\0modeStr\0"
    "changeMode(QString)\0reset()\0validate()\0"
    "paramsSet()\0paramsOk()\0create_okClicked()\0"
    "drawClicked()\0"
};

void SideBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SideBar *_t = static_cast<SideBar *>(_o);
        switch (_id) {
        case 0: { bool _r = _t->isValid();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 1: _t->changeMode((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->reset(); break;
        case 3: _t->validate(); break;
        case 4: _t->paramsSet(); break;
        case 5: _t->paramsOk(); break;
        case 6: _t->create_okClicked(); break;
        case 7: _t->drawClicked(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData SideBar::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SideBar::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_SideBar,
      qt_meta_data_SideBar, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SideBar::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SideBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SideBar::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SideBar))
        return static_cast<void*>(const_cast< SideBar*>(this));
    return QWidget::qt_metacast(_clname);
}

int SideBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
bool SideBar::isValid()
{
    bool _t0;
    void *_a[] = { const_cast<void*>(reinterpret_cast<const void*>(&_t0)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
    return _t0;
}

// SIGNAL 1
void SideBar::changeMode(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
