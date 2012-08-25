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

#ifndef DATAVISUALIZER_H
#define DATAVISUALIZER_H

#include <QMap>

#include <stromx/core/Data.h>

#include "AbstractDataVisualizer.h"
#include "GraphicsView.h"

/** 
 * \brief Data visualizer based on QGraphicsView
 * 
 * This implementation of an AbstractDataVisualizer is derived from QGraphicsView
 * and uses QGraphicsItem objects to display data. The position of a layer translates
 * into the z-value of the graphic items.
 */
class DataVisualizer : public GraphicsView, public AbstractDataVisualizer
{
public:
    DataVisualizer(QWidget* parent = 0);
    
    virtual void addLayer(int layer);
    virtual void moveLayer(int src, int dest);
    virtual void removeLayer(int layer);
    virtual void setColor(int layer, const QColor & color);
    virtual void setData(int layer, const stromx::core::Data& data, Visualization visualization);
    virtual void setActive(int layer, bool active);
    
private:
    /** Casts \c data to an stromx image and returns an image item. */
    static QList<QGraphicsItem*> createImageItems(const stromx::core::Data & data);
    
    /** 
     * Casts \c data to an stromx matrix and returns the line segments defined
     * by the matrix rows.
     */
    static QList<QGraphicsItem*> createLineSegmentItems(const stromx::core::Data & data);
    
    /** 
     * Casts \c data to an stromx primitive and returns its text representation as a
     * text item. 
     */
    template <class data_t>
    static QList<QGraphicsItem*> createPrimitiveItems(const stromx::core::Data & data)
    {
        QList<QGraphicsItem*> items;
        
        try
        {
            const data_t & number = stromx::core::data_cast<const data_t &>(data);
            QGraphicsItem* item = new QGraphicsSimpleTextItem(QString("%1").arg(number));
            items.append(item);
        }
        catch(stromx::core::BadCast&)
        {
        }
        
        return items;
    }
    
    QMap<int, QGraphicsItem*> m_items;
};

#endif // DATAVISUALIZER_H
