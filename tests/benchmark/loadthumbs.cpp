#include <QCoreApplication>
#include <QEventLoop>
#include <QDir>
#include <QTime>
#include <QDebug>
#include <QTemporaryFile>

#include <Quill>
#include <QuillFile>
#include <QuillImageFilter>
#include <QuillImageFilterFactory>
#include "../../src/file.h"

void loadThumbs(QString originalFileName, int n, QSize size)
{
    qDebug() << "Loading" << n << size.width() << "x" << size.height() << "generated thumbnails for" << originalFileName;

    QEventLoop loop;
    QTime time;

    Quill::setTemporaryFilePath(QDir::homePath()+"/.config/quill/tmp/");
    Quill::setThumbnailFlavorName(0, "quill-benchmark");
    Quill::setThumbnailExtension("jpg");
    Quill::setPreviewSize(0, size);

    int numFiles = n;

    Quill::setFileLimit(0, numFiles);

    QTemporaryFile file[numFiles];
    QString fileName[numFiles];
    QuillFile *quillFile[numFiles];

    {
        QFile origFile(originalFileName);
        origFile.open(QIODevice::ReadOnly);
        QByteArray buf = origFile.readAll();
        origFile.close();

        QImage image = QImage(originalFileName).scaled(size);

        for (int i=0; i<numFiles; i++) {
            file[i].setFileTemplate(QDir::homePath()+"/.config/quill/tmp/XXXXXX.jpeg");
            file[i].open();
            fileName[i] = file[i].fileName();
            file[i].write(buf);
            file[i].flush();

            QString thumbFileName = QDir::homePath() +
                "/.thumbnails/quill-benchmark/" +
                File::fileNameHash(file[i].fileName()) + ".jpg";

            image.save(thumbFileName);
        }
    }

    time.start();

    for (int i=0; i<numFiles; i++) {
        quillFile[i] = new QuillFile(fileName[i], "jpg");
        QObject::connect(quillFile[i], SIGNAL(imageAvailable(const QuillImageList)),
                         &loop, SLOT(quit()));
    }

    int initTime = time.elapsed();

    for (int i=0; i<numFiles; i++)
        quillFile[i]->setDisplayLevel(0);

    int displayLevelTime = time.elapsed();

    while (Quill::isCalculationInProgress())
        loop.exec();

    int finalTime = time.elapsed();

    for (int i=0; i<numFiles; i++)
        if (quillFile[i]->image(0).isNull()) {
            qDebug("Error: not all images are loaded!");
            return;
        }

    qDebug() << "Initialize" << numFiles << "QuillFiles:"
             << initTime << "ms";

    qDebug() << "Set display levels of" << numFiles << "QuillFiles:"
             << displayLevelTime - initTime << "ms";

    qDebug() << "Use case load" << numFiles << "generated thumbnails:"
             << finalTime << "ms";

    for (int i=0; i<numFiles; i++)
        delete quillFile[i];
}
