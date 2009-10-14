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

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QVariant>

class QuillFile;
class QuillImageFilter;
class Core;

class HistoryXml
{
    friend class ut_xml;

public:
    static QByteArray encode(QuillFile *file);
    static QByteArray encode(QList<QuillFile *> files);
    static QuillFile *decodeOne(const QByteArray & array, Core *core);
    static QList<QuillFile *> decode(const QByteArray & array, Core *core);

private:
    static void writeFilter(QuillImageFilter *filter, QXmlStreamWriter *writer);
    static bool writeComplexType(QVariant variant, QXmlStreamWriter *writer);
    static QXmlStreamReader::TokenType readToken(QXmlStreamReader *reader);
    static QuillImageFilter *readFilter(QXmlStreamReader *reader);
    static QVariant recoverVariant(QVariant::Type variantType, QString string);
    static QVariant recoverComplexType(QVariant::Type variantType, QXmlStreamReader *reader);
};