/* 
*  Copyright 2012 Matthias Fuchs
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


#ifndef COMMON_H
#define COMMON_H

#include <Qt>
#include <QColor>
#include <QMap>

/** Custom roles of stromx-studio. */
enum Roles
{
    /** 
     * Describes data which represents one item of a list of possible choices.
     * Upon edit the user should be presented the list of choices and select one
     * one of them.
     */
    ChoicesRole = Qt::UserRole,
    
    /**
     * Describes data which represents a color. Upon edit the user should be presented 
     * an editor to choose a color such as a list of predefined colors or a
     * color picker.
     */
    ColorRole,
    
    /**
     * Describes trigger data, i.e. dummy data objects which trigger an action
     * if they are passed to an object. Trigger data does not have a real representation.
     * The user should be presented a push button which triggers setting the data.
     */
    TriggerRole,
    
    /**
     * Describes image data. Upon edit the user should be presented an editor to
     * open or generate new image data.
     */
    ImageRole,
    
    /**
     * The minimal value of numeric data. It should be provided to the user 
     * as an assistance when editing the data.
     */
    MinRole,
    
    /**
     * The maximal value of numeric data. It should be provided to the user 
     * as an assistance when editing the data.
     */
    MaxRole,
    
    /**
     * The step size of numeric data. It should be provided to the user 
     * as an assistance when editing the data.
     */
    StepRole,
    
    /**
     * A matrix data which can be edited by the user in a separate editor.
     */
    MatrixRole,
    
    /** The visualization properties of an observed input. */
    VisualizationStateRole,
    
    /** The number of rows of a matrix parameter. */
    NumRowsRole,
    
    /** The number of columns of a matrix parameter. */
    NumColsRole
};

/** Custom event types of stromx-studio. */
enum EventTypes
{
    /** Type offset of the class ConnectorOccupyEvent. */
    ConnectorOccupy,
    
    /** Type offset of the class ConnectorDataEvent. */
    ConnectorData,
    
    /** Type offset of the class ErrorDataEvent. */
    Error,
    
    /** Type offset for timeout events. */
    Timeout
};

/** 
 * Constant colors which can be chosen by the user for the visualization of
 * inputs and threads. 
 */
class Colors
{
public:
    static const QColor DEFAULT;
    static const QColor RED;
    static const QColor GREEN;
    static const QColor BLUE;
};

/** 
 * Returns the table which maps (localized) names to the colors in the class
 * Colors.
 */
const QMap<QString, QColor> & colorTable();
    
/**
* Returns the appropriate label for the input \c visualization. If an entry at the 
* position \c visualization in the visualization label list exists it is returned.
* Otherwise \c visualization is converted to a string an returned.
*/
QStringList & visualizationLabels();

/** 
 * Stromx title members (title of parameters, connectors, etc.) are encoded as
 * unicode. This function converts \c str from UTF-8 to QString.
 */
QString fromStromxTitle(const std::string & str);

#endif // COMMON_H