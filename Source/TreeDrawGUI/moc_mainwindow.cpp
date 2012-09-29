/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created: Mon Aug 27 22:10:57 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindow[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      23,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x0a,
      28,   11,   11,   11, 0x0a,
      45,   11,   11,   11, 0x0a,
      59,   11,   11,   11, 0x08,
      77,   11,   11,   11, 0x08,
      93,   11,   11,   11, 0x08,
     107,   11,   11,   11, 0x08,
     120,   11,   11,   11, 0x08,
     133,   11,   11,   11, 0x08,
     150,   11,   11,   11, 0x08,
     167,   11,   11,   11, 0x08,
     185,   11,   11,   11, 0x08,
     207,   11,   11,   11, 0x08,
     230,   11,   11,   11, 0x08,
     265,  250,   11,   11, 0x08,
     302,   11,   11,   11, 0x08,
     316,   11,   11,   11, 0x08,
     334,   11,   11,   11, 0x08,
     352,   11,   11,   11, 0x08,
     366,   11,   11,   11, 0x08,
     382,   11,   11,   11, 0x08,
     403,  398,   11,   11, 0x08,
     425,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0sketchChanged()\0"
    "sketchNonEmpty()\0sketchEmpty()\0"
    "openTreeFromXML()\0saveTreeToXML()\0"
    "clearSketch()\0undoAction()\0redoAction()\0"
    "displayXMLFile()\0displayLSTFile()\0"
    "generateFromXML()\0generateFromCurrent()\0"
    "generateNewVariation()\0generationOptions()\0"
    "v1,v2,v3,v4,v5\0optionsAccepted(int,int,int,int,int)\0"
    "exportToOBJ()\0blackBackground()\0"
    "whiteBackground()\0setLineMode()\0"
    "setSelectMode()\0setPencilMode()\0text\0"
    "setBrushSize(QString)\0cancelGeneration()\0"
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MainWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: sketchChanged(); break;
        case 1: sketchNonEmpty(); break;
        case 2: sketchEmpty(); break;
        case 3: openTreeFromXML(); break;
        case 4: saveTreeToXML(); break;
        case 5: clearSketch(); break;
        case 6: undoAction(); break;
        case 7: redoAction(); break;
        case 8: displayXMLFile(); break;
        case 9: displayLSTFile(); break;
        case 10: generateFromXML(); break;
        case 11: generateFromCurrent(); break;
        case 12: generateNewVariation(); break;
        case 13: generationOptions(); break;
        case 14: optionsAccepted((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< int(*)>(_a[5]))); break;
        case 15: exportToOBJ(); break;
        case 16: blackBackground(); break;
        case 17: whiteBackground(); break;
        case 18: setLineMode(); break;
        case 19: setSelectMode(); break;
        case 20: setPencilMode(); break;
        case 21: setBrushSize((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 22: cancelGeneration(); break;
        default: ;
        }
        _id -= 23;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
