#include "OperatorModel.h"
#include <stromx/core/Operator.h>

OperatorModel::OperatorModel(stromx::core::Operator* op, QObject* parent)
  : QAbstractTableModel(parent),
    m_op(op)
{

}

int OperatorModel::rowCount(const QModelIndex& index) const
{
    return 2;
}

int OperatorModel::columnCount(const QModelIndex& index) const
{
    return 2;
}

QVariant OperatorModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        if(section == 0)
            return tr("Property");
        else
            return tr("Value");
    }
    
    return QVariant();
}

QVariant OperatorModel::data(const QModelIndex& index, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();
    
    if(m_op)
    {
        if(index.row() == 0)
        {
            if(index.column() == 0)
                return tr("Type");
            else
                return QVariant(QString::fromStdString(m_op->info().type()));
        }
        else
        {
            if(index.column() == 0)
            {
                return tr("Status");
            }
            else
            {
                switch(m_op->status())
                {
                case stromx::core::Operator::NONE:
                    return QVariant(tr("None"));
                default:
                    return QVariant(tr("Initialized"));
                }
            }
        }
    }
    
    return QVariant();
}

void OperatorModel::setPos(const QPointF& pos)
{
    if(m_pos != pos)
    {
        m_pos = pos;
        emit posChanged(m_pos);
    }
}

bool OperatorModel::isInitialized() const
{
    return m_op->status() != stromx::core::Operator::NONE;
}

void OperatorModel::setInitialized(bool status)
{
    if(isInitialized() == status)
        return;
   
    beginResetModel();
    
    if(status == true)
        m_op->initialize();
    
    endResetModel();
    
    emit initializedChanged(isInitialized());
}

