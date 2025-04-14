/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../include/mainwindow.h"
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
    QByteArrayData data[69];
    char stringdata0[1125];
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
QT_MOC_LITERAL(10, 169, 13), // "onNodeClicked"
QT_MOC_LITERAL(11, 183, 6), // "nodeId"
QT_MOC_LITERAL(12, 190, 13), // "onEdgeHovered"
QT_MOC_LITERAL(13, 204, 4), // "from"
QT_MOC_LITERAL(14, 209, 2), // "to"
QT_MOC_LITERAL(15, 212, 20), // "onVisualizationError"
QT_MOC_LITERAL(16, 233, 28), // "showVisualizationContextMenu"
QT_MOC_LITERAL(17, 262, 3), // "pos"
QT_MOC_LITERAL(18, 266, 19), // "showEdgeContextMenu"
QT_MOC_LITERAL(19, 286, 21), // "onDisplayGraphClicked"
QT_MOC_LITERAL(20, 308, 23), // "on_exportButton_clicked"
QT_MOC_LITERAL(21, 332, 23), // "on_browseButton_clicked"
QT_MOC_LITERAL(22, 356, 24), // "on_analyzeButton_clicked"
QT_MOC_LITERAL(23, 381, 26), // "on_openFilesButton_clicked"
QT_MOC_LITERAL(24, 408, 23), // "on_searchButton_clicked"
QT_MOC_LITERAL(25, 432, 30), // "on_toggleFunctionGraph_clicked"
QT_MOC_LITERAL(26, 463, 23), // "on_fileList_itemClicked"
QT_MOC_LITERAL(27, 487, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(28, 504, 4), // "item"
QT_MOC_LITERAL(29, 509, 19), // "displayFunctionInfo"
QT_MOC_LITERAL(30, 529, 12), // "functionName"
QT_MOC_LITERAL(31, 542, 20), // "onParseButtonClicked"
QT_MOC_LITERAL(32, 563, 17), // "onParsingFinished"
QT_MOC_LITERAL(33, 581, 7), // "success"
QT_MOC_LITERAL(34, 589, 17), // "onLoadJsonClicked"
QT_MOC_LITERAL(35, 607, 18), // "onMergeCfgsClicked"
QT_MOC_LITERAL(36, 626, 27), // "on_extractAstButton_clicked"
QT_MOC_LITERAL(37, 654, 11), // "exportGraph"
QT_MOC_LITERAL(38, 666, 13), // "defaultFormat"
QT_MOC_LITERAL(39, 680, 6), // "zoomIn"
QT_MOC_LITERAL(40, 687, 7), // "zoomOut"
QT_MOC_LITERAL(41, 695, 9), // "resetZoom"
QT_MOC_LITERAL(42, 705, 13), // "setGraphTheme"
QT_MOC_LITERAL(43, 719, 5), // "theme"
QT_MOC_LITERAL(44, 725, 16), // "toggleNodeLabels"
QT_MOC_LITERAL(45, 742, 7), // "visible"
QT_MOC_LITERAL(46, 750, 16), // "toggleEdgeLabels"
QT_MOC_LITERAL(47, 767, 20), // "connectNodesWithEdge"
QT_MOC_LITERAL(48, 788, 21), // "QGraphicsEllipseItem*"
QT_MOC_LITERAL(49, 810, 13), // "dumpSceneInfo"
QT_MOC_LITERAL(50, 824, 11), // "verifyScene"
QT_MOC_LITERAL(51, 836, 14), // "addItemToScene"
QT_MOC_LITERAL(52, 851, 14), // "QGraphicsItem*"
QT_MOC_LITERAL(53, 866, 21), // "switchLayoutAlgorithm"
QT_MOC_LITERAL(54, 888, 5), // "index"
QT_MOC_LITERAL(55, 894, 15), // "onErrorOccurred"
QT_MOC_LITERAL(56, 910, 7), // "message"
QT_MOC_LITERAL(57, 918, 19), // "showNodeContextMenu"
QT_MOC_LITERAL(58, 938, 22), // "loadEmptyVisualization"
QT_MOC_LITERAL(59, 961, 21), // "visualizeCurrentGraph"
QT_MOC_LITERAL(60, 983, 14), // "connectSignals"
QT_MOC_LITERAL(61, 998, 13), // "highlightNode"
QT_MOC_LITERAL(62, 1012, 5), // "color"
QT_MOC_LITERAL(63, 1018, 21), // "highlightInCodeEditor"
QT_MOC_LITERAL(64, 1040, 23), // "toggleVisualizationMode"
QT_MOC_LITERAL(65, 1064, 22), // "highlightSearchResults"
QT_MOC_LITERAL(66, 1087, 13), // "verifyDotFile"
QT_MOC_LITERAL(67, 1101, 8), // "filePath"
QT_MOC_LITERAL(68, 1110, 14) // "analyzeDotFile"

    },
    "MainWindow\0analysisComplete\0\0"
    "CFGAnalyzer::AnalysisResult\0result\0"
    "handleVisualizationResult\0"
    "std::shared_ptr<GraphGenerator::CFGGraph>\0"
    "graph\0handleVisualizationError\0error\0"
    "onNodeClicked\0nodeId\0onEdgeHovered\0"
    "from\0to\0onVisualizationError\0"
    "showVisualizationContextMenu\0pos\0"
    "showEdgeContextMenu\0onDisplayGraphClicked\0"
    "on_exportButton_clicked\0on_browseButton_clicked\0"
    "on_analyzeButton_clicked\0"
    "on_openFilesButton_clicked\0"
    "on_searchButton_clicked\0"
    "on_toggleFunctionGraph_clicked\0"
    "on_fileList_itemClicked\0QListWidgetItem*\0"
    "item\0displayFunctionInfo\0functionName\0"
    "onParseButtonClicked\0onParsingFinished\0"
    "success\0onLoadJsonClicked\0onMergeCfgsClicked\0"
    "on_extractAstButton_clicked\0exportGraph\0"
    "defaultFormat\0zoomIn\0zoomOut\0resetZoom\0"
    "setGraphTheme\0theme\0toggleNodeLabels\0"
    "visible\0toggleEdgeLabels\0connectNodesWithEdge\0"
    "QGraphicsEllipseItem*\0dumpSceneInfo\0"
    "verifyScene\0addItemToScene\0QGraphicsItem*\0"
    "switchLayoutAlgorithm\0index\0onErrorOccurred\0"
    "message\0showNodeContextMenu\0"
    "loadEmptyVisualization\0visualizeCurrentGraph\0"
    "connectSignals\0highlightNode\0color\0"
    "highlightInCodeEditor\0toggleVisualizationMode\0"
    "highlightSearchResults\0verifyDotFile\0"
    "filePath\0analyzeDotFile"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      46,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  244,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,  247,    2, 0x0a /* Public */,
       8,    1,  250,    2, 0x0a /* Public */,
      10,    1,  253,    2, 0x0a /* Public */,
      12,    2,  256,    2, 0x0a /* Public */,
      15,    1,  261,    2, 0x0a /* Public */,
      16,    1,  264,    2, 0x0a /* Public */,
      18,    1,  267,    2, 0x0a /* Public */,
      19,    0,  270,    2, 0x08 /* Private */,
      20,    0,  271,    2, 0x08 /* Private */,
      21,    0,  272,    2, 0x08 /* Private */,
      22,    0,  273,    2, 0x08 /* Private */,
      23,    0,  274,    2, 0x08 /* Private */,
      24,    0,  275,    2, 0x08 /* Private */,
      25,    0,  276,    2, 0x08 /* Private */,
      26,    1,  277,    2, 0x08 /* Private */,
      29,    1,  280,    2, 0x08 /* Private */,
      31,    0,  283,    2, 0x08 /* Private */,
      32,    1,  284,    2, 0x08 /* Private */,
      34,    0,  287,    2, 0x08 /* Private */,
      35,    0,  288,    2, 0x08 /* Private */,
      36,    0,  289,    2, 0x08 /* Private */,
      37,    1,  290,    2, 0x08 /* Private */,
      37,    0,  293,    2, 0x28 /* Private | MethodCloned */,
      39,    0,  294,    2, 0x08 /* Private */,
      40,    0,  295,    2, 0x08 /* Private */,
      41,    0,  296,    2, 0x08 /* Private */,
      42,    1,  297,    2, 0x08 /* Private */,
      44,    1,  300,    2, 0x08 /* Private */,
      46,    1,  303,    2, 0x08 /* Private */,
      47,    2,  306,    2, 0x08 /* Private */,
      49,    0,  311,    2, 0x08 /* Private */,
      50,    0,  312,    2, 0x08 /* Private */,
      51,    1,  313,    2, 0x08 /* Private */,
      53,    1,  316,    2, 0x08 /* Private */,
      55,    1,  319,    2, 0x08 /* Private */,
      57,    1,  322,    2, 0x08 /* Private */,
      58,    0,  325,    2, 0x08 /* Private */,
      59,    0,  326,    2, 0x08 /* Private */,
      60,    0,  327,    2, 0x08 /* Private */,
      61,    2,  328,    2, 0x08 /* Private */,
      63,    1,  333,    2, 0x08 /* Private */,
      64,    0,  336,    2, 0x08 /* Private */,
      65,    0,  337,    2, 0x08 /* Private */,
      66,    1,  338,    2, 0x08 /* Private */,
      68,    1,  341,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   13,   14,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void, QMetaType::QPoint,   17,
    QMetaType::Void, QMetaType::QPoint,   17,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 27,   28,
    QMetaType::Void, QMetaType::QString,   30,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   33,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   38,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   43,
    QMetaType::Void, QMetaType::Bool,   45,
    QMetaType::Void, QMetaType::Bool,   45,
    QMetaType::Void, 0x80000000 | 48, 0x80000000 | 48,   13,   14,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 52,   28,
    QMetaType::Void, QMetaType::Int,   54,
    QMetaType::Void, QMetaType::QString,   56,
    QMetaType::Void, QMetaType::QPoint,   17,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::QColor,   11,   62,
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Bool, QMetaType::QString,   67,
    QMetaType::Void, QMetaType::QString,   67,

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
        case 3: _t->onNodeClicked((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->onEdgeHovered((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 5: _t->onVisualizationError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->showVisualizationContextMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 7: _t->showEdgeContextMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 8: _t->onDisplayGraphClicked(); break;
        case 9: _t->on_exportButton_clicked(); break;
        case 10: _t->on_browseButton_clicked(); break;
        case 11: _t->on_analyzeButton_clicked(); break;
        case 12: _t->on_openFilesButton_clicked(); break;
        case 13: _t->on_searchButton_clicked(); break;
        case 14: _t->on_toggleFunctionGraph_clicked(); break;
        case 15: _t->on_fileList_itemClicked((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 16: _t->displayFunctionInfo((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 17: _t->onParseButtonClicked(); break;
        case 18: _t->onParsingFinished((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 19: _t->onLoadJsonClicked(); break;
        case 20: _t->onMergeCfgsClicked(); break;
        case 21: _t->on_extractAstButton_clicked(); break;
        case 22: _t->exportGraph((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 23: _t->exportGraph(); break;
        case 24: _t->zoomIn(); break;
        case 25: _t->zoomOut(); break;
        case 26: _t->resetZoom(); break;
        case 27: _t->setGraphTheme((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 28: _t->toggleNodeLabels((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 29: _t->toggleEdgeLabels((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 30: _t->connectNodesWithEdge((*reinterpret_cast< QGraphicsEllipseItem*(*)>(_a[1])),(*reinterpret_cast< QGraphicsEllipseItem*(*)>(_a[2]))); break;
        case 31: _t->dumpSceneInfo(); break;
        case 32: _t->verifyScene(); break;
        case 33: _t->addItemToScene((*reinterpret_cast< QGraphicsItem*(*)>(_a[1]))); break;
        case 34: _t->switchLayoutAlgorithm((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 35: _t->onErrorOccurred((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 36: _t->showNodeContextMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 37: _t->loadEmptyVisualization(); break;
        case 38: _t->visualizeCurrentGraph(); break;
        case 39: _t->connectSignals(); break;
        case 40: _t->highlightNode((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QColor(*)>(_a[2]))); break;
        case 41: _t->highlightInCodeEditor((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 42: _t->toggleVisualizationMode(); break;
        case 43: _t->highlightSearchResults(); break;
        case 44: { bool _r = _t->verifyDotFile((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 45: _t->analyzeDotFile((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 33:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QGraphicsItem* >(); break;
            }
            break;
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
        if (_id < 46)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 46;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 46)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 46;
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
