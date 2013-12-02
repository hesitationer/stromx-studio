#include "delegate/InputDelegate.h"

#include <QPainter>
#include <QApplication>

#include "Common.h"
#include "delegate/InputEditWidget.h"
#include "delegate/InputPaintWidget.h"

const int InputDelegate::ROW_HEIGHT = 100;
const int InputDelegate::BORDER_OFFSET = 5;

InputDelegate::InputDelegate(QObject* parent)
  : QStyledItemDelegate(parent)
{
}

QWidget* InputDelegate::createEditor(QWidget* parent,
                                     const QStyleOptionViewItem& /*option*/,
                                     const QModelIndex& index) const
{
    InputEditWidget* editor = new InputEditWidget(index, parent);
    connect(editor, SIGNAL(dataChanged()), this, SLOT(emitCommitData()));
    return editor;
}

void InputDelegate::setEditorData(QWidget* editor, const QModelIndex& /*index*/) const
{
    InputEditWidget* widget = qobject_cast<InputEditWidget*>(editor);
    widget->updateFromModel();
}

void InputDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    InputEditWidget* widget = qobject_cast<InputEditWidget*>(editor);
    
    VisualizationState state;
    state.setIsActive(widget->inputActive());
    state.setCurrentVisualization(""); // TODO: set real visualization
    model->setData(index, QVariant::fromValue(state), VisualizationStateRole);
}

void InputDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (option.state & QStyle::State_Selected)
    {
        painter->fillRect(option.rect, option.palette.background());
        return;
    }
    
    InputPaintWidget box;
    setInputWidgetData(&box, index);
    QRect widgetRect(0, 0, option.rect.width(), option.rect.height());
    box.setGeometry(widgetRect);
    box.setInputTitle(index.data(Qt::DisplayRole).toString());
    QPixmap pixmap(option.rect.size());
    QPainter widgetPainter;
 
    widgetPainter.begin(&pixmap);
    box.render(&widgetPainter);
    widgetPainter.end();
    
    painter->drawPixmap(option.rect, pixmap, widgetRect);
}

QSize InputDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight(ROW_HEIGHT);
    
    return size;
}

void InputDelegate::setInputWidgetData(InputWidget* widget, const QModelIndex& index)
{
    widget->setInputTitle(index.data(Qt::DisplayRole).toString());
    
    QVariant stateVariant = index.data(VisualizationStateRole);
        
    if (stateVariant.canConvert<VisualizationState>())
    {
        VisualizationState state = stateVariant.value<VisualizationState>();
        
        widget->setInputActive(state.isActive());
        widget->setVisualizationType(0); // TODO: convert visualization string to item number
    }
}

void InputDelegate::emitCommitData()
{
    QWidget* widget = qobject_cast<QWidget* >(sender());
    emit commitData(widget);
}
