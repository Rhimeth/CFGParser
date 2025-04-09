/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionOpen;
    QAction *actionExit;
    QAction *actionAbout;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *filePathEdit;
    QPushButton *browseButton;
    QPushButton *analyzeButton;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QLineEdit *search;
    QPushButton *searchButton;
    QPushButton *toggleFunctionGraph;
    QSplitter *splitter;
    QListWidget *fileList;
    QSplitter *splitter_2;
    QGraphicsView *graphicsView;
    QTextEdit *reportTextEdit;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *extractAstButton;
    QPushButton *openFilesButton;
    QPushButton *loadJsonButton;
    QPushButton *mergeCfgsButton;
    QSpacerItem *horizontalSpacer;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuHelp;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(800, 600);
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        filePathEdit = new QLineEdit(centralwidget);
        filePathEdit->setObjectName(QString::fromUtf8("filePathEdit"));

        horizontalLayout->addWidget(filePathEdit);

        browseButton = new QPushButton(centralwidget);
        browseButton->setObjectName(QString::fromUtf8("browseButton"));

        horizontalLayout->addWidget(browseButton);

        analyzeButton = new QPushButton(centralwidget);
        analyzeButton->setObjectName(QString::fromUtf8("analyzeButton"));

        horizontalLayout->addWidget(analyzeButton);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout_2->addWidget(label_2);

        search = new QLineEdit(centralwidget);
        search->setObjectName(QString::fromUtf8("search"));

        horizontalLayout_2->addWidget(search);

        searchButton = new QPushButton(centralwidget);
        searchButton->setObjectName(QString::fromUtf8("searchButton"));

        horizontalLayout_2->addWidget(searchButton);

        toggleFunctionGraph = new QPushButton(centralwidget);
        toggleFunctionGraph->setObjectName(QString::fromUtf8("toggleFunctionGraph"));

        horizontalLayout_2->addWidget(toggleFunctionGraph);


        verticalLayout->addLayout(horizontalLayout_2);

        splitter = new QSplitter(centralwidget);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        fileList = new QListWidget(splitter);
        fileList->setObjectName(QString::fromUtf8("fileList"));
        splitter->addWidget(fileList);
        splitter_2 = new QSplitter(splitter);
        splitter_2->setObjectName(QString::fromUtf8("splitter_2"));
        splitter_2->setOrientation(Qt::Vertical);
        graphicsView = new QGraphicsView(splitter_2);
        graphicsView->setObjectName(QString::fromUtf8("graphicsView"));
        splitter_2->addWidget(graphicsView);
        reportTextEdit = new QTextEdit(splitter_2);
        reportTextEdit->setObjectName(QString::fromUtf8("reportTextEdit"));
        splitter_2->addWidget(reportTextEdit);
        splitter->addWidget(splitter_2);

        verticalLayout->addWidget(splitter);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        extractAstButton = new QPushButton(centralwidget);
        extractAstButton->setObjectName(QString::fromUtf8("extractAstButton"));

        horizontalLayout_3->addWidget(extractAstButton);

        openFilesButton = new QPushButton(centralwidget);
        openFilesButton->setObjectName(QString::fromUtf8("openFilesButton"));

        horizontalLayout_3->addWidget(openFilesButton);

        loadJsonButton = new QPushButton(centralwidget);
        loadJsonButton->setObjectName(QString::fromUtf8("loadJsonButton"));

        horizontalLayout_3->addWidget(loadJsonButton);

        mergeCfgsButton = new QPushButton(centralwidget);
        mergeCfgsButton->setObjectName(QString::fromUtf8("mergeCfgsButton"));

        horizontalLayout_3->addWidget(mergeCfgsButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout_3);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 22));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionExit);
        menuHelp->addAction(actionAbout);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "CFG Parser", nullptr));
        actionOpen->setText(QCoreApplication::translate("MainWindow", "Open", nullptr));
        actionExit->setText(QCoreApplication::translate("MainWindow", "Exit", nullptr));
        actionAbout->setText(QCoreApplication::translate("MainWindow", "About", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "File:", nullptr));
        browseButton->setText(QCoreApplication::translate("MainWindow", "Browse...", nullptr));
        analyzeButton->setText(QCoreApplication::translate("MainWindow", "Analyze", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "Search:", nullptr));
        searchButton->setText(QCoreApplication::translate("MainWindow", "Find", nullptr));
        toggleFunctionGraph->setText(QCoreApplication::translate("MainWindow", "Toggle View", nullptr));
        extractAstButton->setText(QCoreApplication::translate("MainWindow", "Extract AST", nullptr));
        openFilesButton->setText(QCoreApplication::translate("MainWindow", "Open Files...", nullptr));
        loadJsonButton->setText(QCoreApplication::translate("MainWindow", "Load JSON", nullptr));
        mergeCfgsButton->setText(QCoreApplication::translate("MainWindow", "Merge CFGs", nullptr));
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "File", nullptr));
        menuHelp->setTitle(QCoreApplication::translate("MainWindow", "Help", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
