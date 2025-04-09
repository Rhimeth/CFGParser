/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../include/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[47];
    char stringdata0[749];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 16), // "analysisComplete"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 27), // "CFGAnalyzer::AnalysisResult"
QT_MOC_LITERAL(4, 57, 6), // "result"
QT_MOC_LITERAL(5, 64, 25), // "handleVisualizationResult"
QT_MOC_LITERAL(6, 90, 41), // "std::shared_ptr<GraphGenerato..."
QT_MOC_LITERAL(7, 132, 5), // "graph"
QT_MOC_LITERAL(8, 138, 24), // "handleVisualizationError"
QT_MOC_LITERAL(9, 163, 5), // "error"
QT_MOC_LITERAL(10, 169, 23), // "on_browseButton_clicked"
QT_MOC_LITERAL(11, 193, 24), // "on_analyzeButton_clicked"
QT_MOC_LITERAL(12, 218, 26), // "on_openFilesButton_clicked"
QT_MOC_LITERAL(13, 245, 23), // "on_searchButton_clicked"
QT_MOC_LITERAL(14, 269, 30), // "on_toggleFunctionGraph_clicked"
QT_MOC_LITERAL(15, 300, 23), // "on_fileList_itemClicked"
QT_MOC_LITERAL(16, 324, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(17, 341, 4), // "item"
QT_MOC_LITERAL(18, 346, 19), // "displayFunctionInfo"
QT_MOC_LITERAL(19, 366, 12), // "functionName"
QT_MOC_LITERAL(20, 379, 20), // "onParseButtonClicked"
QT_MOC_LITERAL(21, 400, 17), // "onParsingFinished"
QT_MOC_LITERAL(22, 418, 7), // "success"
QT_MOC_LITERAL(23, 426, 17), // "onLoadJsonClicked"
QT_MOC_LITERAL(24, 444, 18), // "onMergeCfgsClicked"
QT_MOC_LITERAL(25, 463, 27), // "on_extractAstButton_clicked"
QT_MOC_LITERAL(26, 491, 11), // "exportGraph"
QT_MOC_LITERAL(27, 503, 6), // "zoomIn"
QT_MOC_LITERAL(28, 510, 7), // "zoomOut"
QT_MOC_LITERAL(29, 518, 9), // "resetZoom"
QT_MOC_LITERAL(30, 528, 13), // "setGraphTheme"
QT_MOC_LITERAL(31, 542, 5), // "theme"
QT_MOC_LITERAL(32, 548, 16), // "toggleNodeLabels"
QT_MOC_LITERAL(33, 565, 7), // "visible"
QT_MOC_LITERAL(34, 573, 16), // "toggleEdgeLabels"
QT_MOC_LITERAL(35, 590, 20), // "connectNodesWithEdge"
QT_MOC_LITERAL(36, 611, 21), // "QGraphicsEllipseItem*"
QT_MOC_LITERAL(37, 633, 4), // "from"
QT_MOC_LITERAL(38, 638, 2), // "to"
QT_MOC_LITERAL(39, 641, 13), // "dumpSceneInfo"
QT_MOC_LITERAL(40, 655, 11), // "verifyScene"
QT_MOC_LITERAL(41, 667, 14), // "addItemToScene"
QT_MOC_LITERAL(42, 682, 14), // "QGraphicsItem*"
QT_MOC_LITERAL(43, 697, 21), // "switchLayoutAlgorithm"
QT_MOC_LITERAL(44, 719, 5), // "index"
QT_MOC_LITERAL(45, 725, 15), // "onErrorOccurred"
QT_MOC_LITERAL(46, 741, 7) // "message"

    },
    "MainWindow\0analysisComplete\0\0"
    "CFGAnalyzer::AnalysisResult\0result\0"
    "handleVisualizationResult\0"
    "std::shared_ptr<GraphGenerator::CFGGraph>\0"
    "graph\0handleVisualizationError\0error\0"
    "on_browseButton_clicked\0"
    "on_analyzeButton_clicked\0"
    "on_openFilesButton_clicked\0"
    "on_searchButton_clicked\0"
    "on_toggleFunctionGraph_clicked\0"
    "on_fileList_itemClicked\0QListWidgetItem*\0"
    "item\0displayFunctionInfo\0functionName\0"
    "onParseButtonClicked\0onParsingFinished\0"
    "success\0onLoadJsonClicked\0onMergeCfgsClicked\0"
    "on_extractAstButton_clicked\0exportGraph\0"
    "zoomIn\0zoomOut\0resetZoom\0setGraphTheme\0"
    "theme\0toggleNodeLabels\0visible\0"
    "toggleEdgeLabels\0connectNodesWithEdge\0"
    "QGraphicsEllipseItem*\0from\0to\0"
    "dumpSceneInfo\0verifyScene\0addItemToScene\0"
    "QGraphicsItem*\0switchLayoutAlgorithm\0"
    "index\0onErrorOccurred\0message"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      28,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  154,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,  157,    2, 0x0a /* Public */,
       8,    1,  160,    2, 0x0a /* Public */,
      10,    0,  163,    2, 0x08 /* Private */,
      11,    0,  164,    2, 0x08 /* Private */,
      12,    0,  165,    2, 0x08 /* Private */,
      13,    0,  166,    2, 0x08 /* Private */,
      14,    0,  167,    2, 0x08 /* Private */,
      15,    1,  168,    2, 0x08 /* Private */,
      18,    1,  171,    2, 0x08 /* Private */,
      20,    0,  174,    2, 0x08 /* Private */,
      21,    1,  175,    2, 0x08 /* Private */,
      23,    0,  178,    2, 0x08 /* Private */,
      24,    0,  179,    2, 0x08 /* Private */,
      25,    0,  180,    2, 0x08 /* Private */,
      26,    0,  181,    2, 0x08 /* Private */,
      27,    0,  182,    2, 0x08 /* Private */,
      28,    0,  183,    2, 0x08 /* Private */,
      29,    0,  184,    2, 0x08 /* Private */,
      30,    1,  185,    2, 0x08 /* Private */,
      32,    1,  188,    2, 0x08 /* Private */,
      34,    1,  191,    2, 0x08 /* Private */,
      35,    2,  194,    2, 0x08 /* Private */,
      39,    0,  199,    2, 0x08 /* Private */,
      40,    0,  200,    2, 0x08 /* Private */,
      41,    1,  201,    2, 0x08 /* Private */,
      43,    1,  204,    2, 0x08 /* Private */,
      45,    1,  207,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void, QMetaType::QString,   19,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   22,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   31,
    QMetaType::Void, QMetaType::Bool,   33,
    QMetaType::Void, QMetaType::Bool,   33,
    QMetaType::Void, 0x80000000 | 36, 0x80000000 | 36,   37,   38,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 42,   17,
    QMetaType::Void, QMetaType::Int,   44,
    QMetaType::Void, QMetaType::QString,   46,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->analysisComplete((*reinterpret_cast< const CFGAnalyzer::AnalysisResult(*)>(_a[1]))); break;
        case 1: _t->handleVisualizationResult((*reinterpret_cast< std::shared_ptr<GraphGenerator::CFGGraph>(*)>(_a[1]))); break;
        case 2: _t->handleVisualizationError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->on_browseButton_clicked(); break;
        case 4: _t->on_analyzeButton_clicked(); break;
        case 5: _t->on_openFilesButton_clicked(); break;
        case 6: _t->on_searchButton_clicked(); break;
        case 7: _t->on_toggleFunctionGraph_clicked(); break;
        case 8: _t->on_fileList_itemClicked((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 9: _t->displayFunctionInfo((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: _t->onParseButtonClicked(); break;
        case 11: _t->onParsingFinished((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 12: _t->onLoadJsonClicked(); break;
        case 13: _t->onMergeCfgsClicked(); break;
        case 14: _t->on_extractAstButton_clicked(); break;
        case 15: _t->exportGraph(); break;
        case 16: _t->zoomIn(); break;
        case 17: _t->zoomOut(); break;
        case 18: _t->resetZoom(); break;
        case 19: _t->setGraphTheme((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 20: _t->toggleNodeLabels((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 21: _t->toggleEdgeLabels((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 22: _t->connectNodesWithEdge((*reinterpret_cast< QGraphicsEllipseItem*(*)>(_a[1])),(*reinterpret_cast< QGraphicsEllipseItem*(*)>(_a[2]))); break;
        case 23: _t->dumpSceneInfo(); break;
        case 24: _t->verifyScene(); break;
        case 25: _t->addItemToScene((*reinterpret_cast< QGraphicsItem*(*)>(_a[1]))); break;
        case 26: _t->switchLayoutAlgorithm((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 27: _t->onErrorOccurred((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MainWindow::*)(const CFGAnalyzer::AnalysisResult & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainWindow::analysisComplete)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindow.data,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 28)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 28;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 28)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 28;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::analysisComplete(const CFGAnalyzer::AnalysisResult & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
