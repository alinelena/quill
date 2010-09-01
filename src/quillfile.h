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

/*!
  \class QuillFile

  \brief Class representing a single file currently being viewed or
  edited, as well as the current state of the file.

A QuillFile represents a single file object. Multiple accesses to the
same file will refer to the same object. Currently, there is no proper
way of freeing a QuillFile object.

QuillFile contains information about the current state of the file; it
is also used to receive and process edit requests from the user. In
addition, it transparently handles thumbnail loading and saving
according to an extension of the FreeDesktop standard.

QuillFiles can be edited by calling runFilter() and then save().

Each file has several display levels. The general properties of the
levels are configurable using the Core class. Preview images of the
file can be generated by calling setDisplayLevel(). For background
processing, it is not necessary to call setDisplayLevel(), just do
edits with runFilter()

As QuillFiles are completely asynchronous, all image data is returned
to the application using asynchronous QObject signals.
 */

#ifndef QUILLFILE_H
#define QUILLFILE_H

#include <QObject>
#include <QDateTime>
#include "quill.h"
#include "quillerror.h"

class QuillImage;
class QuillImageFilter;
class QSize;
class QRect;
class File;
class QuillFilePrivate;

typedef QList<QuillImage> QuillImageList;
Q_DECLARE_METATYPE(QuillImageList)

class QuillFile : public QObject
{
Q_OBJECT

    friend class File;
    friend class ut_file;
public:

    static const int Priority_High = 128;
    static const int Priority_Normal = 0;
    static const int Priority_Low = -128;

    /*!
      Provides access to a file object. The object will be created
      if necessary. The object becomes property of the caller and can
      be destroyed at any time.

      @param fileName The name of the file where changes are
      made. Must be non-empty. The file may be non-existing, in which
      case the file will be created. If creating the file fails,
      this operation will also fail, returning a QuillFile with isValid()
      set to false. Will create directories if necessary.

      @param fileFormat The file format, if not evident from the file
      name. As in QImageReader::setFormat().
    */

    QuillFile(const QString &fileName,
              const QString &fileFormat = "");


    ~QuillFile();

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
      Sets file as read only, disabling all edits. Only makes sense
      with originals.

      DEPRECATED, dropped
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

      If target preview level is a cropped one (see
      Quill::setPreviewLevelMinimumSize()), this will subscribe to
      image changes at the target display level and nothing else. If
      target preview level is not a cropped one, this will subscribe
      to image changes at the target level and any levels which can be
      temporarily substituted to it (any smaller uncropped preview levels).

      If Quill::fileLimit() of the target level, or any level below
      it, has been exceeded, this command does nothing, returns false,
      and an error() signal is emitted.
    */

    bool setDisplayLevel(int level);

    /*!
      Returns the current display level of the file.
    */

    int displayLevel() const;

    /*!
      Note: this method currently has no effect.

      Sets the priority that Quill will use in handling loads and
      edits on this file. Files that are most meaningful for the user
      like ones currently displayed should have a high priority; files
      that are cached since they are about to be displayed should have
      a lower priority.

      Any integer value is valid for priority. Small priority
      differences (less than 100) mean that Quill may prefer a fast
      operation on a lower-priority file before a slow operation on a
      higher-priority file. Bigger differences mean that Quill will
      prefer files with higher priority no matter the operation. The
      constants QuillFile::Priority_Low, QuillFile::Priority_Normal
      and QuillFile::Priority_High can also be used.

      Priority has no effect on saving changes to edited files or
      saving thumbnails. As these operations are not user intensive,
      they always carry the lowest possible priority; additionally,
      this function will not have an effect on relative priorities of
      save operations.
     */

    void setPriority(int priority);

    /*!
      Note: this method currently has no effect and will return zero.

      Returns the priority value for this QuillFile instance. See setPriority().
    */

    int priority() const;

    /*!
      Saves the current state of the active QuillFile with a new name
      and format. The new file will not have an edit history. The
      handle to the new file cannot be accessed until the new file has
      been saved.

      If the target file name is already in use, export will fail.
    */

    void saveAs(const QString &fileName,
                        const QString &fileFormat = "");

    /*!
      Starts to asynchronously save any changes made to the file (if
      any). If there were any changes, the saved() signal is emitted
      when the changes are finished.
    */

    void save();

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
      Empties the redo history for the file. Redoing will no longer be
      possible after this action.
     */

    void dropRedoHistory();

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
      Sets the image representation of the current state of the
      file. The image is not protected; it might be overwritten or
      dropped at any time. The specific parameters (full image size)
      in QuillImage are currently ignored. This can currently only modify
      a full image or a preview, not tiles.
    */

    void setImage(int level, const QuillImage &image);

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
      If the file exists in the file system.
     */

    bool exists() const;

    /*!
      Returns a timestamp of the last time the file was edited. This
      value is cached by Quill and may not be accurate if the file has
      been edited by other processes during the lifetime of the
      QuillFile.
     */

    QDateTime lastModified() const;

    /*!
      If the file is supported and has no errors.

      DEPRECATED - Please use supportsThumbnails(), supportsViewing(),
      or supportsEditing().
     */

    bool supported() const;

    /*!
      Tries to force Quill to acknowledge the file as supported or
      unsupported. This should only be used after the contents of a
      file have been changed. If this is set to true with a file not
      recognizable by Quill, it will revert to unsupported.

      DEPRECATED - Please use refresh() if the file contents have been changed.
     */

    void setSupported(bool supported);

    /*!
      Returns true if thumbnails of the file can be viewed.

      This means that either the file is directly supported by Quill, or
      that thumbnails of the file can be created by the external thumbnailer.
    */

    bool supportsThumbnails() const;

    /*!
      Returns true if any resolution version of the file can be
      easily viewed.

      This means that the image format is supported by Quill via a Qt
      image format plugin, and the image size is not larger than the
      constraints set by Quill::setImageSizeLimit(),
      Quill::setImagePixelsLimit(), and
      Quill::setNonTiledImagePixelsLimit().

      See also supportsThumbnails().
    */

    bool supportsViewing() const;

    /*!
      Returns true if the file can be edited.

      This means that the file can be fully viewed (see
      supportsViewing()), the user has write access, plus the image
      format is supported by Quill via a read-write Qt image format
      plugin.
    */

    bool supportsEditing() const;

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
      Returns the original, read-only copy of this file instance.
    */

    QuillFile *original();

    /*!
      Sets the waiting-for-data status for this file.

      Set this to true if you want to start editing an image which is not
      completely available yet, for example if it is currently being
      transferred by the network. Use setImage() to immediately set a preview
      level. Set this to false when the full image has arrived.

      Warning: the status must be set before raising the preview level, or the
      image will get an "unsupported" status.

      DEPRECATED - setImage() will now automatically put the file into
      waiting state. Also, refresh() should be used to remove the
      waiting state from a file.
    */

    void setWaitingForData(bool);

    /*!
      Returns the waiting-for-data status for the file. See setWaitingForData().
    */

    bool isWaitingForData() const;

    /*!
      Used to inform Quill that file contents have changed in the file
      system; should also be used when file image was initially set
      with setImage() and file contents are now available in the
      system.
     */

    void refresh();

    /*!
      Returns true if Quill Core holds an internal file object
      related to this one.
    */

    bool isValid() const;
    /*!
      If we can revert to the original operation
     */
    bool canRevert() const;

    /*!
      Reverts to the original operation
     */
    void revert();

    /*!
      If we can restore to the pre-revert operation
     */
    bool canRestore() const;

    /*!
      Restors to the pre-revert operation
     */
    void restore();

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

    /*!
      There was an error in the file.
     */
    void error(QuillError error);

private:

    /*!
      Constructor creating an invalid QuillFile object.
     */
    QuillFile();

    /*!
      Constructor creating an QuillFile object from a File object.
     */
    QuillFile(File *file);

    /*!
      Attaches an internal file object to the QuillFile object.
     */
    void attach(File *file);

    /*!
      The referred file object has been destroyed.
     */
    void invalidate();

    /*!
      Instructs the QuillFile object to emit a signal on new image
      data being available.
     */

    void emitImageAvailable(const QuillImage &image);

    /*!
      Instructs the QuillFile object to emit a signal.
     */

    void emitImageAvailable(const QList<QuillImage> &imageList);

    /*!
      Returns the internal file object.
     */

    File *internalFile();

private:
    QuillFilePrivate *priv;
};

#endif
