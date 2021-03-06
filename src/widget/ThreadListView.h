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

#ifndef THREADLISTVIEW_H
#define THREADLISTVIEW_H

#include <QTableView>

class QAbstractTableModel;
class QModelIndex;
class ThreadModel;
class StreamModel;

class ThreadListView : public QTableView
{
    Q_OBJECT

public:
    explicit ThreadListView(QWidget *parent = 0);
    
    QAction* createAddThreadAction(QObject* parent);
    QAction* createRemoveThreadAction(QObject* parent);
    
    void setStreamModel(StreamModel* model);
    
public slots:
    void addThread() const;
    void removeThread() const;
    
signals:
    void removeThreadActiveChanged(bool selected);
    void addThreadActiveChanged(bool selected);
    
protected:
    void contextMenuEvent(QContextMenuEvent* event);
    
private slots:
    void updateThreadSelected( const QModelIndex & current, const QModelIndex & previous );
    void updateStreamActive();
    
private:
    
    ThreadModel* selectedThread() const;
    
    StreamModel* m_model;
    QAction* m_addThreadAct;
    QAction* m_removeThreadAct;
};

#endif // THREADLISTVIEW_H
