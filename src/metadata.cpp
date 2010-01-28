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

#include <QVariant>
#include <QDebug>
#include <exempi-2.0/exempi/xmpconsts.h>

#include "metadata.h"

const QString Schema_DC = NS_DC;
const QString Schema_Exif = NS_EXIF;
const QString Schema_Photoshop = NS_PHOTOSHOP;
const QString Schema_IPTC = NS_IPTC4XMP;
const QString Schema_XAP = NS_XAP;

XmpTag::XmpTag() : schema(""), tag("")
{
}

XmpTag::XmpTag(const QString &schema, const QString &tag) :
    schema(schema), tag(tag)
{
}

Metadata::Metadata(const QString &fileName)
{
    m_exifData = exif_data_new_from_file(fileName.toAscii().constData());
    m_exifByteOrder = exif_data_get_byte_order(m_exifData);

    xmp_init();
    XmpFilePtr xmpFilePtr = xmp_files_open_new(fileName.toAscii().constData(),
                                               XMP_OPEN_READ);
    m_xmpPtr = xmp_files_get_new_xmp(xmpFilePtr);

    initTags();
}

Metadata::~Metadata()
{
    exif_data_unref(m_exifData);
}

void Metadata::initTags()
{
    m_exifTags.insert(Tag_Make, EXIF_TAG_MAKE);
    m_exifTags.insert(Tag_Model, EXIF_TAG_MODEL);
    m_exifTags.insert(Tag_ImageWidth, EXIF_TAG_IMAGE_WIDTH);
    m_exifTags.insert(Tag_ImageHeight, EXIF_TAG_IMAGE_LENGTH);
    m_exifTags.insert(Tag_FocalLength, EXIF_TAG_FOCAL_LENGTH);
    m_exifTags.insert(Tag_ExposureTime, EXIF_TAG_EXPOSURE_TIME);
    m_exifTags.insert(Tag_TimestampOriginal, EXIF_TAG_DATE_TIME_ORIGINAL);

    m_xmpTags.insert(Tag_Title, XmpTag(Schema_Exif, "Title"));
    m_xmpTags.insert(Tag_Copyright, XmpTag(Schema_Exif, "Copyright"));
    m_xmpTags.insert(Tag_Creator, XmpTag(Schema_DC, "creator"));
    m_xmpTags.insert(Tag_Keywords, XmpTag(Schema_DC, "keywords"));
    m_xmpTags.insert(Tag_Subject, XmpTag(Schema_DC, "subject"));
    m_xmpTags.insert(Tag_City, XmpTag(Schema_Photoshop, "City"));
    m_xmpTags.insert(Tag_Country, XmpTag(Schema_Photoshop, "Country"));
    m_xmpTags.insert(Tag_Location, XmpTag(Schema_Photoshop, "Location"));
    //    m_xmpTags.insert(Tag_City, XmpTag(Schema_IPTC, "City"));
    //    m_xmpTags.insert(Tag_Country, XmpTag(Schema_IPTC, "Country"));
    //    m_xmpTags.insert(Tag_Location, XmpTag(Schema_IPTC, "Location"));
    m_xmpTags.insert(Tag_Rating, XmpTag(Schema_XAP, "Rating"));
    m_xmpTags.insert(Tag_Timestamp, XmpTag(Schema_XAP, "MetadataDate"));
}

bool Metadata::isValid()
{
    return ((m_exifData != 0) || (m_xmpPtr != 0));
}

QVariant Metadata::entry(Metadata::Tag tag)
{
    // Prioritize EXIF over XMP as required by metadata working group
    QVariant exifResult = entryExif(tag);
    if (!exifResult.isNull())
        return exifResult;
    else
        return entryXmp(tag);
}

QVariant Metadata::entryExif(Metadata::Tag tag)
{
    if (!m_exifData)
        return QVariant();

    if (!m_exifTags.contains(tag))
        return QVariant();

    ExifTag exifTag = m_exifTags[tag];

    ExifEntry *entry = exif_data_get_entry(m_exifData, exifTag);
    if (!entry)
        return QVariant();

    QVariant result;

    switch(entry->format) {
    case EXIF_FORMAT_ASCII:
        result = QVariant(QString((const char*)entry->data));
        break;

    case EXIF_FORMAT_SHORT:
        result = QVariant(exif_get_short(entry->data, m_exifByteOrder));
        break;

    case EXIF_FORMAT_LONG:
        result = QVariant(exif_get_long(entry->data, m_exifByteOrder));
        break;

    case EXIF_FORMAT_RATIONAL: {
        ExifRational rational = exif_get_rational(entry->data, m_exifByteOrder);
        if (rational.denominator == 0)
            result = QVariant();
        else
            result = QVariant((float)rational.numerator /
                              (float)rational.denominator);
        break;
    }

    case EXIF_FORMAT_SRATIONAL: {
        ExifSRational srational = exif_get_srational(entry->data, m_exifByteOrder);
        if (srational.denominator == 0)
            result = QVariant();
        else
            result = QVariant((float)srational.numerator /
                              (float)srational.denominator);
        break;
    }

    case EXIF_FORMAT_FLOAT:
        result = QVariant(*((float*)entry->data));
        break;

    case EXIF_FORMAT_DOUBLE:
        result = QVariant(*((float*)entry->data));
        break;

    default:
        result = QVariant();
        break;
    }

    return result;
}

QVariant Metadata::entryXmp(Metadata::Tag tag)
{
    if (!m_xmpPtr)
        return QVariant();

    if (!m_xmpTags.contains(tag))
        return QVariant();

    XmpTag xmpTag = m_xmpTags[tag];

    uint32_t propBits;
    XmpStringPtr xmpStringPtr = xmp_string_new();

    if (!xmp_get_property(m_xmpPtr,
                          xmpTag.schema.toAscii().constData(),
                          xmpTag.tag.toAscii().constData(),
                          xmpStringPtr,
                          &propBits))
        return QVariant();

    if (XMP_IS_PROP_ARRAY(propBits)) {
        if (!xmp_get_array_item(m_xmpPtr,
                                xmpTag.schema.toAscii().constData(),
                                xmpTag.tag.toAscii().constData(),
                                1,
                                xmpStringPtr,
                                &propBits))
            return QVariant();
    }

    QVariant result = QVariant(QString(xmp_string_cstr(xmpStringPtr)));
    xmp_string_free(xmpStringPtr);
    return result;
}