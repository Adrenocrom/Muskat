#include "muskat.h"

int main( int argc, char ** argv ) {
    Q_INIT_RESOURCE(muskat);
    QApplication app( argc, argv );

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    if (QCoreApplication::arguments().contains(QStringLiteral("--multisample")))
        format.setSamples(4);
    QSurfaceFormat::setDefaultFormat(format);

    MainWindow mw;
    mw.show();


    return app.exec();
}
