/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created: Sun Nov 4 17:12:09 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "TreeDrawGUI/mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      35,   14, // methods
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
     275,  250,   11,   11, 0x08,
     324,  316,   11,   11, 0x08,
     357,   11,   11,   11, 0x08,
     379,   11,   11,   11, 0x08,
     397,   11,   11,   11, 0x08,
     417,   11,   11,   11, 0x08,
     437,   11,   11,   11, 0x08,
     463,  457,   11,   11, 0x08,
     487,   11,   11,   11, 0x08,
     507,   11,   11,   11, 0x08,
     525,   11,   11,   11, 0x08,
     543,   11,   11,   11, 0x08,
     557,   11,   11,   11, 0x08,
     573,   11,   11,   11, 0x08,
     594,  589,   11,   11, 0x08,
     616,   11,   11,   11, 0x08,
     635,   11,   11,   11, 0x08,
     685,   11,   11,   11, 0x08,
     728,   11,   11,   11, 0x08,
     763,   11,   11,   11, 0x08,
     800,   11,   11,   11, 0x08,

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
    "v1,v2,v3,v4,v5,subdValue\0"
    "optionsAccepted(int,int,int,int,int,int)\0"
    "lstfile\0generateMeshFromLST(std::string)\0"
    "exportCylindesAsOBJ()\0exportMeshAsOBJ()\0"
    "displayAsMesh(bool)\0toggleFoliage(bool)\0"
    "toggleTexture(bool)\0value\0"
    "SubdivSliderChange(int)\0SynthesizeTexture()\0"
    "blackBackground()\0whiteBackground()\0"
    "setLineMode()\0setSelectMode()\0"
    "setPencilMode()\0text\0setBrushSize(QString)\0"
    "cancelGeneration()\0"
    "on_actionExport_Cylinder_Model_to_OBJ_triggered()\0"
    "on_Export_to_Mesh_Model_to_OBJ_triggered()\0"
    "on_actionCreate_Leaves_triggered()\0"
    "on_actionFoliage_Options_triggered()\0"
    "foliageChanged()\0"
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MainWindow *_t = static_cast<MainWindow *>(_o);
        switch (_id) {
        case 0: _t->sketchChanged(); break;
        case 1: _t->sketchNonEmpty(); break;
        case 2: _t->sketchEmpty(); break;
        case 3: _t->openTreeFromXML(); break;
        case 4: _t->saveTreeToXML(); break;
        case 5: _t->clearSketch(); break;
        case 6: _t->undoAction(); break;
        case 7: _t->redoAction(); break;
        case 8: _t->displayXMLFile(); break;
        case 9: _t->displayLSTFile(); break;
        case 10: _t->generateFromXML(); break;
        case 11: _t->generateFromCurrent(); break;
        case 12: _t->generateNewVariation(); break;
        case 13: _t->generationOptions(); break;
        case 14: _t->optionsAccepted((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< int(*)>(_a[5])),(*reinterpret_cast< int(*)>(_a[6]))); break;
        case 15: _t->generateMeshFromLST((*reinterpret_cast< std::string(*)>(_a[1]))); break;
        case 16: _t->exportCylindesAsOBJ(); break;
        case 17: _t->exportMeshAsOBJ(); break;
        case 18: _t->displayAsMesh((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 19: _t->toggleFoliage((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 20: _t->toggleTexture((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 21: _t->SubdivSliderChange((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 22: _t->SynthesizeTexture(); break;
        case 23: _t->blackBackground(); break;
        case 24: _t->whiteBackground(); break;
        case 25: _t->setLineMode(); break;
        case 26: _t->setSelectMode(); break;
        case 27: _t->setPencilMode(); break;
        case 28: _t->setBrushSize((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 29: _t->cancelGeneration(); break;
        case 30: _t->on_actionExport_Cylinder_Model_to_OBJ_triggered(); break;
        case 31: _t->on_Export_to_Mesh_Model_to_OBJ_triggered(); break;
        case 32: _t->on_actionCreate_Leaves_triggered(); break;
        case 33: _t->on_actionFoliage_Options_triggered(); break;
        case 34: _t->foliageChanged(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MainWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, &staticMetaObjectExtraData }
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
        if (_id < 35)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 35;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
