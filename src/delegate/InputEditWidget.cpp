#include "delegate/InputEditWidget.h"

#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QLabel>

#include "Common.h"
#include "visualization/Visualization.h"
#include "visualization/VisualizationRegistry.h"
#include "visualization/VisualizationState.h"
#include "visualization/VisualizationWidget.h"

InputEditWidget::InputEditWidget(QWidget* parent)
  : QGroupBox(parent),
    m_widget(0),
    m_activeCheckBox(0),
    m_layout(0),
    m_propertiesGroupBox(0),
    m_isSettingState(false)
{
    m_layout = new QFormLayout();

    m_activeCheckBox = new QCheckBox(tr("Active"));
    connect(m_activeCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateState()));
    m_layout->addRow(m_activeCheckBox);
    
    m_visualizationMenu = new QComboBox();
    m_visualizationMenu->addItems(VisualizationRegistry::visualizationNameList());
    m_visualizationMenu->setCurrentIndex(-1);
    connect(m_visualizationMenu, SIGNAL(currentIndexChanged(int)), this, SLOT(updateState()));
    connect(m_visualizationMenu, SIGNAL(currentIndexChanged(int)), this, SLOT(updateWidget()));
    m_layout->addRow(tr("Visualization"), m_visualizationMenu);
    
    setLayout(m_layout);
}

void InputEditWidget::setInputTitle(const QString& title)
{
    setTitle(title);
}

void InputEditWidget::updateState()
{
    // if the state is currently being set by the model its change must not
    // be propagated back to model
    if (m_isSettingState)
        return; 
    
    m_state.setIsActive(m_activeCheckBox->isChecked());
    if (m_widget)
        m_state.currentProperties() = m_widget->getProperties();
    
    const Visualization* visualization = VisualizationRegistry::visualization(m_visualizationMenu->currentIndex());
    QString identifier = visualization ? visualization->visualization() : "";
    m_state.setCurrentVisualization(identifier);
    
    emit dataChanged();
}

void InputEditWidget::setState(const VisualizationState& state)
{
    m_isSettingState = true;
    
    m_state = state;
    m_activeCheckBox->setChecked(state.isActive());
    QString identifier = state.currentVisualization();
    int index = VisualizationRegistry::identifierToIndex(identifier);
    m_visualizationMenu->setCurrentIndex(index);
    if (m_widget)
        m_widget->setProperties(state.currentProperties());
    
    m_isSettingState = false;
}

void InputEditWidget::updateWidget()
{
    const Visualization* visualization = VisualizationRegistry::visualization(m_state.currentVisualization());
    if (visualization)
    {
        delete m_propertiesGroupBox;
        m_propertiesGroupBox = 0;
        m_widget = visualization->createEditor();
        if (m_widget)
        {
            QVBoxLayout* layout = new QVBoxLayout();
            layout->addWidget(m_widget);
            m_propertiesGroupBox = new QGroupBox(tr("Properties"));
            m_propertiesGroupBox->setLayout(layout);
            m_layout->addRow(m_propertiesGroupBox);
            
            m_widget->setProperties(m_state.currentProperties());
            connect(m_widget, SIGNAL(valueChanged()), this, SLOT(updateState()));
        }
    }
}



