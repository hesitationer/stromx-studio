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

#ifndef ITEMDELEGATE_H
#define ITEMDELEGATE_H

#include <QStyledItemDelegate>

class QDoubleSpinBox;
class QSpinBox;

/** 
 * \brief Delegate to support custom data in item views.
 * 
 * This class provides the following custom item view editors:
 * - A popup menu if the item view returns a QStringList object
 *   for the role Roles::ChoicesRole. 
 * - A popup menu with colors if the item view returns a QColor
 *   object for the role Roles::ColorRole.
 * - A push button if the item view returns a QString object for 
 *   the role Roles::TriggerRole.
 */
class ItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
    
public:
    static const int ROW_HEIGHT;
    
    /**
     * Returns true if \c value can safely converted to an \c int value
     * (i.e. the unsigned value is smaller than the maximal signed integer value).
     */
    static bool canConvertToInt(const QVariant value);
    
    explicit ItemDelegate(QObject* parent = 0);
    
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;
    virtual void setEditorData (QWidget* editor, const QModelIndex & index) const;
    virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex & index ) const;
    virtual QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const;
    
private slots:
    void commitEditEvent();
    
    /** 
     * Tries to read the min, max and step roles from the data at index
     * and sets the according spinbox properties.
     */
    void setSpinBoxParameters(const QModelIndex & index, QSpinBox* spinBox) const;
    
    /** 
     * Tries to read the min, max and step roles from the data at index
     * and sets the according properties of double spin box.
     */
    void setDoubleSpinBoxParameters(const QModelIndex & index, QDoubleSpinBox* spinBox) const;
};

#endif // ITEMDELEGATE_H
