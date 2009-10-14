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

#ifndef QUILLFILE_H
#define QUILLFILE_H

#include <QMetaType>
#include <QList>
#include <QuillImageFilter>

#include "quill.h"

class QuillImage;
class QuillImageFilter;
class QuillUndoStack;
class QuillFilePrivate;
class QuillFile;

typedef QList<QuillImage> QuillImageList;
Q_DECLARE_METATYPE(QuillImageList)

class QuillFile : public QObject
{
Q_OBJECT

    friend class ut_thumbnail;

public:
    QuillFile(QObject *parent);
    virtual ~QuillFile();

    /*!
      Returns the actual file name associated with the QuillFile object.
    */

    QString fileName() const;

    /*!
      Returns the original file name associated with the QuillFile
      object. The original is used to save the unmodified backup of
      the file.
    */

    QString originalFileName() const;

    /*!
      Returns format associated with the QuillFile object.
    */

    QString fileFormat() const;

    /*!
      Returns the target format associated with the QuillFile object.
      This is different from fileFormat() if the file has been
      exported into a different format.
    */

    QString targetFormat() const;

    /*!
      Sets the file name.

      Internal/testing use only.
    */

    void setFileName(const QString &fileName);

    /*!
      Sets the original file name.

      Internal/testing use only.
    */

    void setOriginalFileName(const QString &originalFileName);

    /*!
      Sets the file format.

      Internal/testing use only.
    */

    void setFileFormat(const QString &fileFormat);

    /*!
      Sets the target format.

      Internal/testing use only.
    */

    void setTargetFormat(const QString &targetFormat);

    /*!
      Sets file as read only, disabling all edits. Only makes sense
      with originals.
    */

    void setReadOnly();

    /*!
      If the file is read only.
    */

    bool isReadOnly() const;

    /*!
      Sets the display level of the file.
      -1 = no display, 0 = first level only, ...
      previewlevelcount = full image or tiles.

      Default is -1. This can be modified on the fly.
    */

    void setDisplayLevel(int level);

    /*!
      Returns the current display level of the file.
    */

    int displayLevel() const;

    /*!
      Starts to asynchronously save any changes made to the file (if
      any). If there were any changes, the saved() signal is emitted
      when the changes are finished.
    */

    void save();

    /*!
      Exports the file into a new name and format.

      @param format The file format. This parameter must be given if
      the format is not evident from the file extension.
    */

    QuillFile *exportFile(const QString &newFileName,
                          const QString &fileFormat = "");

    /*!
      If the file is in the progress of saving.
    */

    bool isSaveInProgress() const;

    /*!
      Starts to asynchronously run an operation.
     */

    void runFilter(QuillImageFilter *filter);

    /*!
      Starts an undo session. When an undo session is in progress,
      no undo/redo outside the session is permitted. A closed
      undo session will be treated as one operation by undo() and
      redo().
     */

    void startSession();

    /*!
      Concludes an undo session started by startSession(). A closed
      undo session will be treated as one operation by undo() and
      redo().
     */

    void endSession();

    /*!
      If an undo session is in progress. See startSession() and
      endSession().
    */

    bool isSession() const;

    /*!
      If the previous operation can be undone.
     */

    bool canUndo() const;

    /*!
      Undoes the previous operation.
     */

    void undo();

    /*!
      If an undone operation can be redone.
     */

    bool canRedo() const;

    /*!
      Redoes a previously undone operation.
     */

    void redo();

    /*!
      Returns a representation of the current state of the file.

      @result the highest-resolution representation of the full image
      which is available. This can exceed the display level of the
      image. This function will not return tiles.
    */

    QuillImage image() const;

    /*!
      Returns a representation of the current state of the file.

      @param level The resolution level to be used.

      @result a representation of the full image using exactly the
      given resolution level. This can exceed the display level of the
      image. This function will not return tiles.
    */

    QuillImage image(int level) const;

    /*!
      Returns all image levels of the current state of the file.

      @result all representations of the full image which are currently cached,
      including all resolution levels and tiles.
    */

    QList<QuillImage> allImageLevels() const;

    /*!
      Returns the full image size, in pixels, of the current state of the file.
    */

    QSize fullImageSize() const;

    /*!
      Sets the viewport area (in full-image coordinates), only
      takes effect if tiling is in use. Only tiles within the viewport
      will be processed and returned. The default is no viewport
      (e.g. no tiles will be returned).
    */

    void setViewPort(const QRect &viewPort);

    /*!
      Returns the viewport in full-image coordinates. Only tiles
      within the viewport will be processed and returned.
     */

    QRect viewPort() const;

    /*!
      If a related thumbnail has been cached to the file system.
     */

    bool hasThumbnail(int level) const;

    /*!
      Gets the file name associated with the given preview level.

      @return the file name associated with the given preview level,
      or an empty string if the given level is not cached. The file is
      not guaranteed to exist.
     */

    QString thumbnailFileName(int level) const;

    /*!
      Returns the associated undo stack.

      Internal/testing use only.
     */

    QuillUndoStack *stack() const;

    /*!
      If the file exists in the file system.
     */

    bool exists() const;

    /*!
      If the file is supported and has no errors.
     */

    bool supported() const;

    /*!
      Reads a complete file object from edit history.

      @param parent a Quill Core object.

      Internal/testing use only.
     */

    static QuillFile *readFromEditHistory(const QString &fileName,
                                          QObject *parent);

    /*!
      Copies a file over another file in the file system.

      Qt does not have an overwriting copy for platform reasons,
      this has been is implemented for efficiency reasons.
     */

    static void overwritingCopy(const QString &fileName,
                                const QString &targetName);

    /*!
      Completely removes a file along with its associated original
      backup, edit history, and any thumbnails. Does not remove the
      associated file object. This operation cannot be undone.
     */
    void remove();

    /*!
      Removes the thumbnails for a file. This operation cannot be undone.
     */
    void removeThumbnails();

    /*!
      Immediately concludes any save operation. Internal use only.
     */

    void concludeSave();

    /*!
      Returns the original, read-only copy of this file instance.
    */

    QuillFile *original();

    /*!
      Immediately triggers the imageAvailable() signal. Internal use only.
     */

    void emitImageAvailable(QList<QuillImage> imageList);

    /*!
      Triggers an error.
     */

    void setError(Quill::Error errorCode);

signals:
    /*!
      Triggered when there is a new image representation available on
      the active state of the file.

      @result a set of preview levels, the full version, or a tile
      representing the active state of the file.
     */

    void imageAvailable(const QuillImageList images);

    /*!
      Saving a file has been concluded on the background.
    */
    void saved();

    /*!
      The associated file has been removed.
     */
    void removed();

    /*
      There was an error in the file.
     */
    void error(Quill::Error errorCode);

private:
    void prepareSave();

    static QString fileNameHash(const QString &fileName);

    static QString editHistoryFileName(const QString &fileName,
                                       const QString &editHistoryDirectory);

    /*!
      Writes the edit history.
     */

    void writeEditHistory(const QString &history);

    QuillFilePrivate *priv;
};

#endif // QUILLFILE_H