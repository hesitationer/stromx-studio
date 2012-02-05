/* 
*  Copyright 2011 Matthias Fuchs
*
*  This file is part of stromx-studio.
*
*  Stromx-studio is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  Stromx-studio is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with stromx-studio.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QSplitter>
#include <QToolBar>
#include <QUndoStack>
#include <QDir>
#include <QFileDialog>
#include <QSettings>
#include <iostream>
#include <stromx/core/DirectoryFileInput.h>
#include <stromx/core/DirectoryFileOutput.h>
#include <stromx/core/ZipFileInput.h>
#include <stromx/core/ZipFileOutput.h>
#include "Exception.h"
#include "MainWindow.h"
#include "ObserverEditor.h"
#include "OperatorLibrary.h"
#include "OperatorLibraryModel.h"
#include "PropertyEditor.h"
#include "StreamEditor.h"
#include "StreamEditorScene.h"
#include "StreamModel.h"
#include "ThreadEditor.h"

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
{
    m_undoStack = new QUndoStack(this);
    
    QSplitter* splitter = new QSplitter(Qt::Vertical);
    m_streamEditor = new StreamEditor;
    m_threadEditor = new ThreadEditor;
    m_observerEditor = new ObserverEditor;
    m_operatorLibrary = new OperatorLibrary();
    m_propertyEditor = new PropertyEditor();
    
    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    createDockWindows();
    
    m_streamEditor->scene()->setUndoStack(m_undoStack);
    StreamModel* streamModel = new StreamModel(m_undoStack, m_operatorLibrary->model(), this);
    m_streamEditor->scene()->setModel(streamModel);
    m_threadEditor->setModel(streamModel);
    
    splitter->addWidget(m_streamEditor);
    splitter->addWidget(m_threadEditor);
     
    m_operatorLibrary->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    
    addDockWidget(Qt::LeftDockWidgetArea, m_operatorLibrary);
    addDockWidget(Qt::RightDockWidgetArea, m_propertyEditor);
    addDockWidget(Qt::RightDockWidgetArea, m_observerEditor);
    setCentralWidget(splitter);
    
    updateCurrentFile("");
    updateWindowTitle(true);
    
    readSettings();
    
    connect(m_streamEditor->scene(), SIGNAL(selectedModelChanged(QAbstractTableModel*)),
            m_propertyEditor, SLOT(setModel(QAbstractTableModel*)));
    connect(m_undoStack, SIGNAL(cleanChanged(bool)), this, SLOT(updateWindowTitle(bool)));
    connect(m_undoStack, SIGNAL(cleanChanged(bool)), m_saveAct, SLOT(setDisabled(bool)));
}

MainWindow::~MainWindow()
{
}

void MainWindow::createActions()
{
    m_undoAct = m_undoStack->createUndoAction(this);
    m_undoAct->setShortcuts(QKeySequence::Undo);
    m_redoAct = m_undoStack->createRedoAction(this);
    m_redoAct->setShortcuts(QKeySequence::Redo);
    m_initializeAct = m_streamEditor->scene()->createInitializeAction(this);
    m_deinitializeAct = m_streamEditor->scene()->createDeinitializeAction(this);
    m_addThreadAct = m_threadEditor->createAddThreadAction(this);
    m_removeThreadAct = m_threadEditor->createRemoveThreadAction(this);
    m_removeSelectedItemsAct = m_streamEditor->scene()->createRemoveAction(this);

    m_saveAct = new QAction(tr("&Save"), this);
    m_saveAct->setShortcuts(QKeySequence::Save);
    m_saveAct->setStatusTip(tr("Save the current stream"));
    m_saveAct->setEnabled(false);
    connect(m_saveAct, SIGNAL(triggered()), this, SLOT(save()));

    m_saveAsAct = new QAction(tr("Save &As..."), this);
    m_saveAsAct->setShortcuts(QKeySequence::SaveAs);
    m_saveAsAct->setStatusTip(tr("Save the current stream as"));
    connect(m_saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    m_openAct = new QAction(tr("&Open..."), this);
    m_openAct->setShortcuts(QKeySequence::Open);
    m_openAct->setStatusTip(tr("Open a stream"));
    connect(m_openAct, SIGNAL(triggered()), this, SLOT(open()));

    m_closeAct = new QAction(tr("&Close"), this);
    m_closeAct->setShortcuts(QKeySequence::Close);
    m_closeAct->setStatusTip(tr("Close the current stream"));
    connect(m_closeAct, SIGNAL(triggered()), this, SLOT(closeStream()));

    m_loadLibrariesAct = new QAction(tr("&Load Libraries..."), this);
    m_loadLibrariesAct->setStatusTip(tr("Load operator libraries"));
    connect(m_loadLibrariesAct, SIGNAL(triggered()), this, SLOT(loadLibraries()));

    m_resetLibrariesAct = new QAction(tr("&Reset Libraries..."), this);
    m_resetLibrariesAct->setStatusTip(tr("Reset operator libraries"));
    connect(m_resetLibrariesAct, SIGNAL(triggered()), this, SLOT(resetLibraries()));

    m_quitAct = new QAction(tr("&Quit"), this);
    m_quitAct->setShortcuts(QKeySequence::Quit);
    m_quitAct->setStatusTip(tr("Quit the application"));
    connect(m_quitAct, SIGNAL(triggered()), this, SLOT(close()));

    m_startAct = new QAction(QIcon(":/images/start.png"), tr("Start"), this);
    m_startAct->setStatusTip(tr("Start the stream"));
    connect(m_startAct, SIGNAL(triggered()), this, SLOT(start()));

    m_stopAct = new QAction(QIcon(":/images/stop.png"), tr("Stop"), this);
    m_stopAct->setStatusTip(tr("Stop the stream"));
    connect(m_stopAct, SIGNAL(triggered()), this, SLOT(stop()));

    m_aboutAct = new QAction(tr("&About"), this);
    m_aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(m_aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    m_aboutQtAct = new QAction(tr("About &Qt"), this);
    m_aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(m_aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createDockWindows()
{
}

void MainWindow::createMenus()
{
     m_fileMenu = menuBar()->addMenu(tr("&File"));
     m_fileMenu->addAction(m_openAct);
     m_fileMenu->addAction(m_saveAct);
     m_fileMenu->addAction(m_saveAsAct);
     m_fileMenu->addAction(m_closeAct);
     m_fileMenu->addSeparator();
     m_fileMenu->addAction(m_loadLibrariesAct);
     m_fileMenu->addAction(m_resetLibrariesAct);
     m_fileMenu->addSeparator();
     m_fileMenu->addAction(m_quitAct);

     m_editMenu = menuBar()->addMenu(tr("&Edit"));
     m_editMenu->addAction(m_undoAct);
     m_editMenu->addAction(m_redoAct);
     
     m_streamMenu = menuBar()->addMenu(tr("&Stream"));
     m_streamMenu->addAction(m_removeSelectedItemsAct);
     m_streamMenu->addAction(m_initializeAct);
     m_streamMenu->addAction(m_deinitializeAct);
     m_streamMenu->addSeparator();
     m_streamMenu->addAction(m_addThreadAct);
     m_streamMenu->addAction(m_removeThreadAct);

     m_viewMenu = menuBar()->addMenu(tr("&View"));

     menuBar()->addSeparator();

     m_helpMenu = menuBar()->addMenu(tr("&Help"));
     m_helpMenu->addAction(m_aboutAct);
     m_helpMenu->addAction(m_aboutQtAct);
}

void MainWindow::createStatusBar()
{
}

void MainWindow::createToolBars()
{    
     m_streamToolBar = addToolBar(tr("Stream"));
     m_streamToolBar->setObjectName("StreamToolbar");
     m_streamToolBar->addAction(m_startAct);
     m_streamToolBar->addAction(m_stopAct);
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About stromx-studio"),
                       tr("The <b>stromx-studio</b> is an editor for stromx streams."));
}

bool MainWindow::save()
{
    if(m_currentFile.isEmpty())
    {
        // return false if the saving dialog was cancelled
        if(! saveAs())
            return false;
    }
    
    writeFile(m_currentFile);
    
    return true;
}

void MainWindow::start()
{
}

void MainWindow::stop()
{
}

bool MainWindow::open()
{
    if(! saveBeforeClosing())
        return false;
    
    QSettings settings("stromx", "stromx-studio");
    QString lastDir = settings.value("lastStreamOpened", QDir::home().absolutePath()).toString();
    
    QString file = QFileDialog::getOpenFileName(this, tr("Select a stream to open"),
                                                lastDir, tr("Stromx archive (*.zip);;Stream file (*.xml)")); 
    
    if(file.isNull())
        return false;
    
    readFile(file);
    
    return true;
}

void MainWindow::readFile(const QString& filepath)
{ 
    QString basename = QFileInfo(filepath).baseName();
    QString extension = QFileInfo(filepath).suffix();
    if(! (extension == "xml" || extension == "zip"))
    {
        QMessageBox::critical(this, tr("stromx-studio error"), tr("Invalid file extension"),
                              QMessageBox::Ok, QMessageBox::Ok);
    }
    
    // load the stream
    try
    {
        if(extension == "xml")
        {
            QString directory = QFileInfo(filepath).absoluteDir().absolutePath();
            stromx::core::DirectoryFileInput input(directory.toStdString());
            m_streamEditor->scene()->model()->read(input, basename);
        }
        else if(extension == "zip")
        {
            stromx::core::ZipFileInput input(filepath.toStdString());
            m_streamEditor->scene()->model()->read(input, basename);
        }
    }
    catch(ReadStudioDataFailed&)
    {
        QMessageBox::warning(this, tr("stromx-studio error"), tr("Failed to read studio data."),
                             QMessageBox::Ok, QMessageBox::Ok);
    }
    catch(ReadStreamFailed&)
    {
        QMessageBox::critical(this, tr("stromx-studio error"), tr("Failed to open stream."),
                              QMessageBox::Ok, QMessageBox::Ok);
    }
    
    updateCurrentFile(filepath);
    
    // remember the last file
    QSettings settings("stromx", "stromx-studio");
    settings.setValue("lastStreamOpened", filepath);
}

void MainWindow::updateWindowTitle(bool undoStackIsClean)
{
    QString cleanMarker = undoStackIsClean ? "" : tr("*");
    QString file = m_currentFile.isEmpty() ? tr("Untitled") : QFileInfo(m_currentFile).fileName();
    QString title = file + cleanMarker + " - " + tr("stromx-studio");
    setWindowTitle(title);
}

void MainWindow::updateCurrentFile(const QString& filepath)
{
    m_currentFile = filepath;
    m_undoStack->setClean();
    updateWindowTitle(true);
}

bool MainWindow::saveBeforeClosing()
{
    if(! m_undoStack->isClean())
    { 
        QMessageBox msgBox;
        msgBox.setText(tr("The stream has been modified."));
        msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();
        
        switch (ret)
        {
        case QMessageBox::Save:
            // cancel closing if saving was cancelled
            if(! save())
                return false;
            break;
        case QMessageBox::Discard:
            // forget the changes
            break;
        case QMessageBox::Cancel:
            // cancel closing
            return false;
        default:
            Q_ASSERT(false);
            break;
        }
    }
    
    return true;
}



bool MainWindow::closeStream()
{
    if(! saveBeforeClosing())
        return false;
    
    // replace the current model with a new one
    StreamModel* currentModel = m_streamEditor->scene()->model();
    StreamModel* newModel = new StreamModel(m_undoStack, m_operatorLibrary->model(), this);
    m_streamEditor->scene()->setModel(newModel);
    m_undoStack->clear();
    delete currentModel;
    
    updateCurrentFile("");
    
    return true;
}

bool MainWindow::saveAs()
{
    QSettings settings("stromx", "stromx-studio");
    QString lastDir = settings.value("lastStreamSavedDir", QDir::home().absolutePath()).toString();
    
    QString file = QFileDialog::getSaveFileName(this, tr("Save stromx-studio project"),
                                                lastDir, tr("Stromx archive (*.zip);;Stream file (*.xml)")); 
    
    if(file.isNull())
        return false;
    
    writeFile(file);
    
    return true;
}

void MainWindow::writeFile(const QString& filepath)
{
    QString basename = QFileInfo(filepath).baseName();
    QString extension = QFileInfo(filepath).suffix();
    if(! (extension == "xml" || extension == "zip"))
    {
        QMessageBox::critical(this, tr("stromx-studio error"), tr("Invalid file extension"),
                              QMessageBox::Ok, QMessageBox::Ok);
    }
        
    // write the stream
    try
    {
        if(extension == "xml")
        {
            QString directory = QFileInfo(filepath).absoluteDir().absolutePath();
            stromx::core::DirectoryFileOutput output(directory.toStdString());
            m_streamEditor->scene()->model()->write(output, basename);
        }
        else if(extension == "zip")
        {
            stromx::core::ZipFileOutput output(filepath.toStdString());
            m_streamEditor->scene()->model()->write(output, basename);
        }
    }
    catch(WriteStreamFailed&)
    {
        QMessageBox::critical(this, tr("stromx-studio error"), tr("Failed to save stream"),
                              QMessageBox::Ok, QMessageBox::Ok);
    }
    
    updateCurrentFile(filepath);
    
    // remember the last dir
    QSettings settings("stromx", "stromx-studio");
    settings.setValue("lastStreamSavedDir", QFileInfo(filepath).dir().absolutePath());
}

void MainWindow::readSettings()
{
    QSettings settings("stromx", "stromx-studio");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
}

void MainWindow::writeSettings()
{
    QSettings settings("stromx", "stromx-studio");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
}

void MainWindow::loadLibraries()
{
    QSettings settings("stromx", "stromx-studio");
    QString lastDir = settings.value("lastLibrary", QDir::home().absolutePath()).toString();
    
    QStringList files = QFileDialog::getOpenFileNames(
                            this,
                            tr("Select one or more stromx libraries to open"),
                            lastDir,
                            tr("Libraries (*.so)")); 
    
    // load each library
    // Qt documentation recommends to iterate over a copy of the file list
    QStringList fileList = files;
    foreach(QString file, fileList)
    {
        try
        {
            m_operatorLibrary->model()->loadLibrary(file);
        }
        catch(LoadLibraryFailed&)
        {
            std::cout << "Failed to load '" << file.toStdString() << "'" << std::endl;
        }
    }
    
    // remember the last library
    if(files.size())
    {
        QString lastDir = files.back();
        settings.setValue("lastLibrary", lastDir);
    }
}

void MainWindow::resetLibraries()
{
    QMessageBox msgBox;
    msgBox.setText(tr("Do you want to reset the operator libraries?"));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    
    if(ret == QMessageBox::Ok)
        m_operatorLibrary->model()->resetLibraries();
}

void MainWindow::closeEvent(QCloseEvent* event)
{ 
    if(closeStream())
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
    
    writeSettings();
}





