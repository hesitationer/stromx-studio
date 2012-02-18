#include "InputData.h"

#include <QStringList>
#include <stromx/core/Operator.h>
#include "OperatorModel.h"

InputData::InputData(unsigned int id, OperatorModel* op)
  : m_id(id),
    m_op(op)
{
    setText(QString("%1/%2").arg(QString::fromStdString(m_op->op()->name()), m_id));
}


QStringList InputData::formats () const
{
    return QStringList("stromx/input");
}