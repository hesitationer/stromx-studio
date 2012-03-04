#include "StreamEditor.h"

#include <QApplication>
#include <QGraphicsView>
#include <QMouseEvent>
#include "ConnectionItem.h"
#include "ConnectionModel.h"
#include "ConnectorItem.h"
#include "InputData.h"
#include "StreamEditorScene.h"
#include "StreamModel.h"

StreamEditor::StreamEditor(QWidget* parent)
  : QGraphicsView(parent),
    m_targetOp(0),
    m_inputId(0)
{
    m_scene = new StreamEditorScene;
    
    m_scene->setSceneRect(0, 0, 512, 512);
    setScene(m_scene);
    
    setAcceptDrops(true);
    setDragMode(QGraphicsView::RubberBandDrag);
}

void StreamEditor::mouseMoveEvent(QMouseEvent* event)
{
    if(event->buttons() & Qt::LeftButton && m_targetOp)
    {
        int distance = (event->pos() - m_startPos).manhattanLength();
        
        if(distance >= QApplication::startDragDistance())
            startDrag();
        
        return;
    }
        
    QGraphicsView::mouseMoveEvent(event);
}

void StreamEditor::mousePressEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton)
    {
        QPointF scenePos = mapToScene(event->pos());
        QGraphicsItem* item = scene()->itemAt(scenePos);
        
        if(ConnectionItem* connection = qgraphicsitem_cast<ConnectionItem*>(item))
        {
            m_startPos = event->pos();
            m_targetOp = connection->model()->targetOp();
            m_inputId = connection->model()->inputId();
            return;
        }
        else if(ConnectorItem* connector = qgraphicsitem_cast<ConnectorItem*>(item))
        {
            if(connector->connectorType() == ConnectorItem::INPUT
               && connector->numConnections())
            {
                m_startPos = event->pos();
                m_targetOp = connector->op();
                m_inputId = connector->id();
                return;
            }
        }
    }
    
    m_targetOp = 0;
    m_inputId = 0;
    QGraphicsView::mousePressEvent(event);
}

void StreamEditor::startDrag()
{    
    // allocate a input data object
    InputData* data = new InputData(m_targetOp, m_inputId);
    
    QList<QGraphicsView*> views = scene()->views();
    if(views.count() != 1)
        return;
    QWidget* currentWidget = views[0];
    
    QDrag* drag = new QDrag(currentWidget);
    drag->setMimeData(data);
    drag->exec(Qt::CopyAction, Qt::CopyAction);
    
    m_targetOp = 0;
    m_inputId = 0;
}

