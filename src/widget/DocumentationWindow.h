/* 
*  Copyright 2012 Thomas Fidler
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

#ifndef DOCUMENTATIONWINDOW_H
#define DOCUMENTATIONWINDOW_H

#include <QMainWindow>

/**
 * \brief Window which displays documentation.
 */
class DocumentationWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit DocumentationWindow(QWidget* parent = 0);
};

#endif // DOCUMENTATIONWINDOW_H