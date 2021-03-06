#include "event/ConnectorOccupyEvent.h"

ConnectorOccupyEvent::ConnectorOccupyEvent(OperatorModel::ConnectorType type, unsigned int id, bool isOccupied)
  : QEvent(Type(TYPE)),
    m_type(type),
    m_id(id),
    m_occupied(isOccupied)
{
}
