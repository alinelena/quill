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

#include <QuillImage>
#include <QCache>
#include <QDebug>

#include "tilecache.h"

class ImageTile
{
public:

    QuillImage image;
    int key;
};

TileCache::TileCache(int cost)
{
    m_cache.setMaxCost(cost);
}

TileCache::~TileCache()
{
}

void TileCache::resizeCache(const int cost)
{
    m_cache.setMaxCost(cost);
}

int TileCache::cacheCost() const
{
    return m_cache.maxCost();
}

bool TileCache::searchKey(const int key) const
{
    return m_cache.contains(key);
}

void TileCache::setTile(int tileId, int tileMapId, const QuillImage &tile)
{
    ImageTile* imageTile = new ImageTile;
    imageTile->image = tile;
    imageTile->key = tileMapId;

    m_cache.insert(tileId, imageTile);
}

QuillImage TileCache::tile(int tileId, int tileMapId) const
{
    if (searchKey(tileId)) {
        ImageTile *object = m_cache.object(tileId);
        if (object->key == tileMapId)
            return object->image;
        else
            return QuillImage();
    } else
        return QuillImage();
}

void TileCache::clear()
{
    m_cache.clear();
}