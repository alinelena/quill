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

#include <QUndoStack>
#include <QuillImage>
#include <QFileInfo>
#include <QBuffer>
#include <QDataStream>
#include <QImageWriter>
#include <QuillImageFilter>
#include <QuillImageFilterFactory>
#include <QuillImageFilterGenerator>

#include "quillundostack.h"
#include "quillundocommand.h"
#include "file.h"
#include "core.h"
#include "tilemap.h"
#include "savemap.h"
#include "logger.h"

QuillUndoStack::QuillUndoStack(File *file) :
    m_stack(new QUndoStack()), m_file(file), m_isSessionRecording(false),
    m_recordingSessionId(0), m_nextSessionId(1), m_savedIndex(0),
    m_saveCommand(0), m_saveMap(0),m_revertIndex(0)
{
}

QuillUndoStack::~QuillUndoStack()
{
    delete m_stack;
    delete m_saveCommand;
    delete m_saveMap;
}

File* QuillUndoStack::file()
{
    return m_file;
}

void QuillUndoStack::setInitialLoadFilter(QuillImageFilter *filter)
{
    QFile loadFile(m_file->originalFileName());
    if (loadFile.exists() && (loadFile.size() > 0)) {
        filter->setOption(QuillImageFilter::FileName,
                          m_file->originalFileName());
        filter->setOption(QuillImageFilter::FileFormat,
                          m_file->fileFormat());
    }
    else {
        filter->setOption(QuillImageFilter::FileName,
                          m_file->fileName());
        filter->setOption(QuillImageFilter::FileFormat,
                          m_file->targetFormat());
    }

    QByteArray format = filter->option(QuillImageFilter::FileFormat).toByteArray();

    if (!format.isNull() &&
        !QImageWriter::supportedImageFormats().contains(format))
        m_file->setReadOnly();

    m_file->setFileFormat(filter->option(QuillImageFilter::MimeType).toByteArray());
}

void QuillUndoStack::load()
{
    QuillImageFilter *filter =
        QuillImageFilterFactory::createImageFilter(QuillImageFilter::Role_Load);
    filter->setOption(QuillImageFilter::BackgroundColor,
                      Core::instance()->backgroundRenderingColor());
    setInitialLoadFilter(filter);
    add(filter);
}

void QuillUndoStack::calculateFullImageSize(QuillUndoCommand *command)
{
    // full image size
    QSize previousFullSize;

    QuillImageFilter *filter = command->filter();

    if (filter->role() == QuillImageFilter::Role_Load)
        previousFullSize = QSize();
    else
        previousFullSize = command->prev()->fullImageSize();

    QSize fullSize = filter->newFullImageSize(previousFullSize);

    if (fullSize.isEmpty()) {
        if (filter->role() == QuillImageFilter::Role_Load)
            m_file->processFilterError(filter);
        else if (previousFullSize.isValid()) {
            // Any new command which would make the image 0x0 is rejected
            undo();
            command->setFilter(0);
            delete filter;
        }
        return;
    }

    if (!m_file->checkImageSize(fullSize))
        m_file->imageSizeError();

    // Vector graphics are always rendered to a fixed size
    if ((m_file->fileFormat() == "image/svg+xml") &&
        Core::instance()->vectorGraphicsRenderingSize().isValid()) {
        QSize maximumSize = Core::instance()->vectorGraphicsRenderingSize().
            boundedTo(Core::instance()->previewSize(Core::instance()->previewLevelCount()-1));
        fullSize = QuillUndoCommand::scaleBounding(fullSize, maximumSize);
    }

    command->setFullImageSize(fullSize);

    // tile map
    if (!Core::instance()->defaultTileSize().isEmpty() && !command->tileMap()) {
        TileMap *tileMap;
        if (filter->role() == QuillImageFilter::Role_Load)
            tileMap = new TileMap(fullSize,
                                  Core::instance()->defaultTileSize(),
                                  Core::instance()->tileCache());
        else
            tileMap = new TileMap(command->prev()->tileMap(), filter);

        command->setTileMap(tileMap);
    }
}

void QuillUndoStack::add(QuillImageFilter *filter)
{
    QuillUndoCommand *cmd = new QuillUndoCommand(this);

    cmd->setFilter(filter);
    cmd->setIndex(index());
    if (m_isSessionRecording &&
        (cmd->filter()->role() != QuillImageFilter::Role_Load))
        cmd->setSessionId(m_recordingSessionId);

    // add to stack
    m_stack->push(cmd);

    Logger::log("[Stack] "+filter->name()+" added to stack");

    // full image size
    if (!m_file->isWaitingForData())
        calculateFullImageSize(cmd);
    setRevertIndex(0);
}

bool QuillUndoStack::canUndo() const
{
    if (!m_stack->canUndo())
        return false;

    // The initial load cannot be undone
    if (index() <= 1)
        return false;

    // Session mode: cannot undo outside session
    if ((m_isSessionRecording) &&
        (!command()->belongsToSession(m_recordingSessionId)))
        return false;

    return true;
}

void QuillUndoStack::undo()
{
    if (canUndo()) {
        // In case of an intermediate load, we make a double undo
        if ((command()->filter()->role() == QuillImageFilter::Role_Load) && (m_stack->index() > 2))
            m_stack->undo();

        // If we are not currently recording a session, an entire
        // session should be undone at once.
        if (!m_isSessionRecording && command()->belongsToSession())
        {
            int toUndoSessionId = command()->sessionId();

            do
                m_stack->undo();
            while (canUndo() && command()->belongsToSession(toUndoSessionId));
        }
        else m_stack->undo();

        // If we have any stored images in cache, move them to protected
        // This is here instead of command::undo() so that intermediate steps
        // need not be protected.
        command()->protectImages();
    }
}

bool QuillUndoStack::canRedo() const
{
    if (!m_stack->canRedo())
        return false;

    // Invalid filters cannot be redone
    if (!command(index())->filter())
        return false;

    // Session mode: cannot redo outside session
    if ((m_isSessionRecording) &&
        (!command(index())->belongsToSession(m_recordingSessionId)))
        return false;

    return true;
}

void QuillUndoStack::redo()
{
    if (canRedo()) {
        // If we are not currently recording a session, an entire
        // session should be redone at once.
        if (!m_isSessionRecording &&
            command(index())->belongsToSession())
        {
            int toRedoSessionId = command(index())->sessionId();

            do
                m_stack->redo();
            while (canRedo() &&
                   command(index())->belongsToSession(toRedoSessionId));
        }
        else m_stack->redo();

        // In case of intermediate load, double redo
        if (canRedo() && (command(index())->filter()->role() == QuillImageFilter::Role_Load))
            m_stack->redo();

        // If we have any stored images in cache, move them to protected
        // This is here instead of command::redo() so that intermediate steps
        // need not be protected.
        command()->protectImages();

        setRevertIndex(0);
    }
}
QuillImage QuillUndoStack::bestImage(int maxLevel) const
{
    QuillUndoCommand *curr = command();
    if (curr)
        return curr->bestImage(maxLevel);
    else
        return QuillImage();
}

QuillImage QuillUndoStack::image(int level) const
{
    QuillUndoCommand *curr = command();
    if (curr)
        return curr->image(level);
    else
        return QuillImage();
}

void QuillUndoStack::setImage(int level, const QuillImage &image)
{
    QuillUndoCommand *curr = command();
    if (curr) {
        curr->setImage(level, image);
        curr->setFullImageSize(image.fullImageSize());
    }
}

QList<QuillImage> QuillUndoStack::allImageLevels(int maxLevel) const
{
    QuillUndoCommand *curr = command();
    if (curr)
        return curr->allImageLevels(maxLevel);
    else
        return QList<QuillImage>();
}

int QuillUndoStack::count() const
{
    return m_stack->count();
}

int QuillUndoStack::index() const
{
    return m_stack->index();
}

QuillUndoCommand *QuillUndoStack::command() const
{
    if (index() > 0)
        return command(index()-1);
    else
        return 0;
}

QuillUndoCommand *QuillUndoStack::command(int index) const
{
    // This can be done since the stack will only hold QuillUndoCommands.

    return (QuillUndoCommand*) m_stack->command(index);
}

QuillUndoCommand *QuillUndoStack::find(int id) const
{
    for (int index=0; index<m_stack->count(); index++) {
        QuillUndoCommand *cmd = command(index);
        if (cmd && (cmd->uniqueId() == id))
            return cmd;
    }

    if ((m_saveCommand && m_saveCommand->uniqueId() == id))
        return m_saveCommand;

    return 0;
}

bool QuillUndoStack::isClean() const
{
    return m_stack->isClean();
}

QSize QuillUndoStack::fullImageSize()
{
    if (isClean())
        load();

    if (command())
        return command()->fullImageSize();
    else
        return QSize();
}

void QuillUndoStack::setSavedIndex(int index)
{
    m_savedIndex = index;
}

int QuillUndoStack::savedIndex() const
{
    return m_savedIndex;
}

bool QuillUndoStack::isDirty() const
{
    return (command()->index() != savedIndex());
}

void QuillUndoStack::startSession()
{
    if (!m_isSessionRecording) {
        m_isSessionRecording = true;
        m_recordingSessionId = m_nextSessionId;
        m_nextSessionId++;
    }
}

void QuillUndoStack::endSession()
{
    m_isSessionRecording = false;
}

bool QuillUndoStack::isSession() const
{
    return m_isSessionRecording;
}

void QuillUndoStack::prepareSave(const QString &fileName,
                                 const QByteArray &rawExifDump)
{
    m_isSessionRecording = false;

    delete m_saveCommand;
    delete m_saveMap;

    if (!Core::instance()->defaultTileSize().isEmpty())
        m_saveMap = new SaveMap(command()->fullImageSize(),
                                Core::instance()->saveBufferSize(),
                                command()->tileMap());

    QuillImageFilter *saveFilter =
        QuillImageFilterFactory::createImageFilter(QuillImageFilter::Role_Save);

    saveFilter->setOption(QuillImageFilter::FileName,
                          QVariant(fileName));

    saveFilter->setOption(QuillImageFilter::FileFormat,
                          QVariant(m_file->targetFormat()));

    saveFilter->setOption(QuillImageFilter::RawExifData,
                          QVariant(rawExifDump));

    m_saveCommand = new QuillUndoCommand(this);
    m_saveCommand->setFilter(saveFilter);

    if (!Core::instance()->defaultTileSize().isEmpty()) {
        saveFilter->setOption(QuillImageFilter::TileCount,
                              QVariant(m_saveMap->bufferCount()));
        m_saveCommand->setTileMap(new TileMap(command()->tileMap(),
                                                  saveFilter));
    }
}

void QuillUndoStack::concludeSave()
{
    m_savedIndex = command()->index();

    // Update initial load filter to point to modified file
    if (m_stack->command(0))
        setInitialLoadFilter(command(0)->filter());

    delete m_saveCommand;
    m_saveCommand = 0;

    delete m_saveMap;
    m_saveMap = 0;
}

QuillUndoCommand *QuillUndoStack::saveCommand()
{
    return m_saveCommand;
}

SaveMap *QuillUndoStack::saveMap()
{
    return m_saveMap;
}

void QuillUndoStack::setRevertIndex(int index)
{
    m_revertIndex = index;
}

int QuillUndoStack::revertIndex() const
{
    return m_revertIndex;
}


void QuillUndoStack::revert()
{
    setRevertIndex(index());
    do{
        undo();
    }
    while(canUndo());
}

void QuillUndoStack::restore()
{
    int revertIndex = m_revertIndex;
    while(canRedo() && (index() < revertIndex))
        redo();

    setRevertIndex(0);
}

bool QuillUndoStack::canRevert() const
{
    return canUndo();
}

bool QuillUndoStack::canRestore() const
{
    return revertIndex() > 0;
}

