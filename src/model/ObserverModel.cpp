#include "model/ObserverModel.h"

#include "cmd/RenameObserverCmd.h"
#include "data/InputData.h"
#include "model/InputModel.h"
#include "model/ObserverTreeModel.h"

ObserverModel::ObserverModel(QUndoStack* undoStack, ObserverTreeModel* parent)
  : QAbstractTableModel(parent),
    m_undoStack(undoStack),
    m_parent(parent)
{
    m_name = "New observer";
    
    connect(m_parent, SIGNAL(rowsAboutToBeInserted(QModelIndex, int, int)),
            this, SLOT(handleRowsAboutToBeInserted(QModelIndex, int, int)));
    connect(m_parent, SIGNAL(rowsInserted(QModelIndex, int, int)),
            this, SLOT(handleRowsInserted(QModelIndex, int, int)));
    
    connect(m_parent, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)),
            this, SLOT(handleRowsAboutToBeRemoved(QModelIndex, int, int)));
    connect(m_parent, SIGNAL(rowsRemoved(QModelIndex, int, int)),
            this, SLOT(handleRowsRemoved(QModelIndex, int, int)));
    
    connect(m_parent, SIGNAL(inputAdded(InputModel*,ObserverModel*,int)),
            this, SLOT(handleInputAdded(InputModel*,ObserverModel*,int)));
    connect(m_parent, SIGNAL(inputMoved(InputModel*,ObserverModel*,int,ObserverModel*,int)),
            this, SLOT(handleInputMoved(InputModel*,ObserverModel*,int,ObserverModel*,int)));
    connect(m_parent, SIGNAL(inputRemoved(InputModel*,ObserverModel*,int)),
            this, SLOT(handleInputRemoved(InputModel*,ObserverModel*,int)));
}

void ObserverModel::setName(const QString& name)
{
    if(name != m_name)
    {
        QUndoCommand* cmd = new RenameObserverCmd(this, name);
        m_undoStack->push(cmd);
    }
}

void ObserverModel::doSetName(const QString& name)
{
    m_name = name;
    emit nameChanged(name);
    emit changed(this);
}

void ObserverModel::insertInput(int position, InputModel* input)
{
    m_inputs.insert(position, input);
    input->setParentModel(this);
}

void ObserverModel::removeInput(int position)
{
    m_inputs[position]->setParentModel(0);
    m_inputs.removeAt(position);
}

InputModel* ObserverModel::input(int position) const
{
    if(position < 0 || position >= m_inputs.size())
        return 0;
    
    return m_inputs[position];
}

int ObserverModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 1;
}

int ObserverModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;
    else
        return numInputs();
}

QModelIndex ObserverModel::index(int row, int column, const QModelIndex& /*parent*/) const
{
    return createIndex(row, column, m_inputs[row]);
}

QVariant ObserverModel::data(const QModelIndex& index, int role) const
{
    QModelIndex parentIndex = m_parent->inputIndex(this, index.row());
    return m_parent->data(parentIndex, role);
}

bool ObserverModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    QModelIndex parentIndex = m_parent->inputIndex(this, index.row());
    return m_parent->setData(parentIndex, value, role);
}

void ObserverModel::handleRowsAboutToBeInserted(const QModelIndex& parent, int start, int end)
{
    if(concernsThisObserver(parent))
        beginInsertRows(QModelIndex(), start, end);
}

void ObserverModel::handleRowsInserted(const QModelIndex& parent, int /*start*/, int /*end*/)
{
    if(concernsThisObserver(parent))
        endInsertRows();
}

void ObserverModel::handleRowsAboutToBeRemoved(const QModelIndex& parent, int start, int end)
{
    if(concernsThisObserver(parent))
        beginRemoveRows(QModelIndex(), start, end);
}

void ObserverModel::handleRowsRemoved(const QModelIndex& parent, int /*start*/, int /*end*/)
{
    if(concernsThisObserver(parent))
        endRemoveRows();
}

void ObserverModel::emitDataChanged(const int topRow, const int bottomRow)
{
    QModelIndex topLeft = createIndex(topRow, 0, m_inputs[topRow]);
    QModelIndex bottomRight = createIndex(bottomRow, 0, m_inputs[bottomRow]);
    emit dataChanged(topLeft, bottomRight);
}

void ObserverModel::handleInputAdded(InputModel* input, ObserverModel* observer, int pos)
{
    if(observer == this)
        emit inputAdded(input, pos);
}

void ObserverModel::handleInputMoved(InputModel* input, ObserverModel* srcObserver, int srcPos, ObserverModel* destObserver, int destPos)
{
    if(srcObserver == this && destObserver == this)
        emit inputMoved(input, srcPos, destPos);
    else if(srcObserver == this)
        emit inputRemoved(input, srcPos);
    else if(destObserver == this)
        emit inputAdded(input, destPos);
}

void ObserverModel::handleInputRemoved(InputModel* input, ObserverModel* observer, int pos)
{
    if(observer == this)
        emit inputRemoved(input, pos);
}

bool ObserverModel::concernsThisObserver(const QModelIndex & parentModelIndex) const
{
    return this == parentModelIndex.internalPointer();
}

QStringList ObserverModel::mimeTypes() const
{
    return m_parent->mimeTypes();
}

Qt::DropActions ObserverModel::supportedDropActions() const
{
    return Qt::MoveAction | Qt::CopyAction;
}

Qt::DropActions ObserverModel::supportedDragActions() const
{
    return Qt::MoveAction;
}

bool ObserverModel::dropMimeData(const QMimeData* data, Qt::DropAction action,
                                 int /*row*/, int /*column*/, const QModelIndex& parent)
{
    int observerPos = m_parent->observers().indexOf(const_cast<ObserverModel*>(this));
    if(observerPos >= 0)
    {
        int insertAt = parent.row() >= 0 ? parent.row() : numInputs();
        QModelIndex observerIndex = m_parent->observerIndex(observerPos);
        return m_parent->dropMimeData(data, action, insertAt, 0, observerIndex);
    }
    
    return false;
}

QMimeData* ObserverModel::mimeData(const QModelIndexList& indexes) const
{
    QMimeData* data = m_parent->mimeData(indexes);
    
    if(InputData* inputData = qobject_cast<InputData*>(data))
    {
        inputData->setDragSource(InputData::TABLE_MODEL);
        return inputData;
    }
    
    return data;
}

Qt::ItemFlags ObserverModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);
    
    flags |= (Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
    
    return flags;
}



