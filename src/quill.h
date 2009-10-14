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

#ifndef QUILL_H
#define QUILL_H

#include <QObject>
#include <QuillImageFilter>

class QImage;
class QString;
class QSize;
class QuillImage;
class QuillFilterGenerator;
class QuillFile;

class QuillPrivate;

class Quill : public QObject
{
Q_OBJECT

public:
    /*!
      For testing use only.
    */

    typedef enum _ThreadingMode
    {
        ThreadingNormal,
        ThreadingTest
    } ThreadingMode;

    typedef enum _Error
    {
        ErrorUnspecified = -1,
        NoError = 0,
        ErrorFileNonexistent = 1,
        ErrorFormatUnsupported,
        ErrorFileCorrupt,
        ErrorDirectoryCannotCreate,
    } Error;

    static QSize defaultViewPortSize;
    static int defaultCacheSize;

    /*!
      @param viewPortSize maximum viewport size
      @param threadingMode use for testing only!
    */
    Quill(const QSize &viewPortSize = Quill::defaultViewPortSize,
          Quill::ThreadingMode threadingMode = Quill::ThreadingNormal);
    virtual ~Quill();

    /*!
      Used to create a singleton pattern.
    */
    static Quill *instance();

    /*!
      Provides access to a file object. The object will be created
      if necessary. It will stay as property of the Quill object and
      kept in memory indefinitely.

      @param fileName The name of the file where changes are
      made. Must be non-empty. The file may be non-existing, in which
      case the file will be created. If creating the file fails, this
      operation will also fail. Will create directories if necessary.

      @param fileFormat The file format, if not evident from the file
      name. As in QImageReader::setFormat().
    */

    QuillFile *file(const QString &fileName,
                    const QString &fileFormat = "");

    /*!
      Modifies the preview level count. If new previews are
      created, both of their dimensions are twice those of the
      previous level by default. Calling this will fail if any files
      are either active or in the progress of saving.

      @param count The number of preview levels. Must be at least 1.
     */

    void setPreviewLevelCount(int count);

    /*!
      Returns the preview level count set by setPreviewLevelCount().
     */

    int previewLevelCount() const;

    /*!
      Set cache limit for the image cache for the given preview level.

      @param level preview level

      @param limit the cache limit, in bytes.
    */

    void setCacheLimit(int level, int limit);

    /*!
      Sets the recommended size for preview images for a certain
      level. Calling this will fail if any files are either active, or
      in the progress of saving.
     */

    void setPreviewSize(int level, QSize size);

    /*!
      Sets the recommended size for preview images for a certain
      level. Calling this will fail if any files are either active, or
      in the progress of saving.
    */

    QSize previewSize(int level) const;

    /*!
      Sets the maximum tile size if tiling is in use.
      The default is 0, which disables tiling.
     */

    void setDefaultTileSize(const QSize &size);

    /*!
      Sets the tile cache size (measured in tiles, not bytes!)
      The default is 20.
    */

    void setTileCacheSize(int size);

    /*!
      Sets the maximum save buffer size, in pixels (4 bytes per pixel).
    */

    void setSaveBufferSize(int size);

    /*!
      Dumps the image editor state into a byte array which can be
      saved to the file system.
     */

    QByteArray dump();

    /*!
      Recovers the state of the image editor after a crash.
      All files start as closed.
    */

    void recover(QByteArray history);

    /*
      Sets the default directory where to look for edit histories.
      Default is $HOME/.config/quill/history.
     */

    void setEditHistoryDirectory(const QString &directory);

    /*!
      Sets the directory where to look for ready-made thumbnails
      for a given preview level. Default is empty, meaning that all
      preview images for the level are generated from the full image.
     */

    void setThumbnailDirectory(int level, const QString &directory);

    /*!
      Sets the file extension which is used in storing and retrieving thumbnail
      files. This is also used to determine the format of the thumbnail files.
      Does not include a full stop. Default is "png".
     */

    void setThumbnailExtension(const QString &format);

    /*!
      To make background loading tests easier on fast machines

      Only works if Quill has been created with threadingMode =
      Quill::ThreadingTest. This will allow the background thread to
      apply one image operation, and make the foreground thread to
      wait for it. Calling this is required, or the background thread
      will freeze forever.
    */

    void releaseAndWait();

    /*!
      To make background loading tests easier on fast machines
      @param delay extra delay per background operation, in seconds
    */

    void setDebugDelay(int delay);

 signals:

    /*!
      There was a generic error in Quill.
     */

    void error(Quill::Error errorCode);

private:

    QuillPrivate *priv;
};

#endif // __QUILL_PUBLIC_INTERFACE_H_