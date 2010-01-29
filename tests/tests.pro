
TEMPLATE = subdirs

DEPENDPATH += .
INCLUDEPATH += .

CONFIG += ordered

SUBDIRS += \
           ut_unittests \
           ut_filtergenerator \
           ut_imageloader \
           ut_partialloader \
           ut_tiling \
           ut_quill \
           ut_tilemap \
           ut_savemap \
           ut_imagecache \
           ut_command \
           ut_stack \
           ut_xml \
           ut_core \
           ut_file \
           ut_thumbnail \
           ut_crashrecovery \
           ut_tileloading \
           ut_error \
           ut_logger \
           ut_metadata \
           benchmark  \

# --- install
tatam.files = tests.xml
tatam.path  = $$(DESTDIR)/usr/share/libquill-tests/

tatamimages.files += images/image_16x4.jpg
tatamimages.files += images/image_16x4.png
tatamimages.files += images/exif.jpg
tatamimages.files += images/xmp.jpg

tatamimages.path  = $$(DESTDIR)/usr/share/libquill-tests/images/

INSTALLS += tatam tatamimages
