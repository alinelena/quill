/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Alexander Bokovoy <alexander.bokovoy@nokia.com>
**
** This file is part of the Quill package.
**
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
**
****************************************************************************/

#ifndef DISPLAY_LEVEL_H
#define DISPLAY_LEVEL_H

#include <QSize>
#include <QString>

class ImageCache;

class DisplayLevel {
 public:
    DisplayLevel(const QSize &size);
    ~DisplayLevel();

    void setSize(const QSize &size);
    QSize size() const;

    void setMinimumSize(const QSize &size);
    QSize minimumSize() const;

    bool isCropped() const;

    void setFileLimit(int limit);
    int fileLimit() const;

    void setThumbnailFlavorPath(const QString &name);
    QString thumbnailFlavorPath() const;

    ImageCache *imageCache() const;

 private:
    QSize m_size;
    QSize m_minimumSize;
    int m_fileLimit;
    QString m_thumbnailFlavorPath;
    ImageCache *m_imageCache;
};

#endif