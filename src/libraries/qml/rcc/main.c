#include <libqt6c.h>
#include "rcc.h"

void on_failed(void* engine UNUSED, const void* url UNUSED) {
    fprintf(stderr, "Failed to create QML object\n");
    q_guiapplication_quit();
    abort();
}

int main(int argc, char* argv[]) {
    QGuiApplication* qapp = q_guiapplication_new(&argc, argv);

    bool ok = qrc_rcc_init();
    if (!ok)
        fprintf(stderr, "Resource initialization failed!\n");

    QQmlApplicationEngine* engine = q_qmlapplicationengine_new();
    q_qmlapplicationengine_on_object_creation_failed(engine, on_failed);
    q_qmlapplicationengine_load2(engine, "qrc:/main.qml");

    int result = q_guiapplication_exec();

    ok = qrc_rcc_delete();
    if (!ok)
        fprintf(stderr, "Resource deinitialization failed!\n");

    q_qmlapplicationengine_delete(engine);
    q_guiapplication_delete(qapp);

    return result;
}
