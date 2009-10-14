##########
# the project file in src/
##########

TEMPLATE = lib
TARGET = quill
DEPENDPATH += .
INCLUDEPATH += .
LIBS +=-lqtimagefilter -lquillimagefilter -lgcov
QMAKE_CXXFLAGS += -ftest-coverage -fprofile-arcs -fno-elide-constructors
DEFINES     +=
CONFIG += DEBUG

#this is for removing coverage information while doing qmake as "qmake COV_OPTION=off"
for(OPTION,$$list($$lower($$COV_OPTION))){
    isEqual(OPTION, off){
        QMAKE_CXXFLAGS -= -ftest-coverage -fprofile-arcs -fno-elide-constructors
        LIBS -= -lgcov
    }
}

# --- input

HEADERS += quill.h \
           quillfile.h \
           core.h \
           tilecache.h \
           tilemap.h \
           savemap.h \
           threadmanager.h \
           quillundocommand.h \
           quillundostack.h \
           imagecache.h \
           historyxml.h

SOURCES += quill.cpp \
           quillfile.cpp \
           core.cpp \
           tilecache.cpp \
           tilemap.cpp \
           savemap.cpp \
           threadmanager.cpp \
           quillundocommand.cpp \
           quillundostack.cpp \
           imagecache.cpp \
           historyxml.cpp

INSTALL_HEADERS = \
           Quill \
           QuillFile \
           quill.h \
           quillfile.h

# --- install
headers.files = $$INSTALL_HEADERS
headers.path = $$(DESTDIR)/usr/include
target.path = $$(DESTDIR)/usr/lib
INSTALLS += target headers

# ---clean
QMAKE_CLEAN += \
	*.gcov *.gcno *.log *.moc_* *.gcda