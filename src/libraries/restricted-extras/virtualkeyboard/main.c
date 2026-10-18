#include <libqt6c.h>

void on_quit(void* engine UNUSED) {
    q_guiapplication_quit();
}

void on_failed(void* engine UNUSED, const void* url UNUSED) {
    fprintf(stderr, "Failed to create QML object\n");
    q_guiapplication_quit();
    abort();
}

int main(int argc, char* argv[]) {
    if (getenv("QT_IM_MODULE") == NULL) {
        // uncomment to use setenv instead (where supported)
        // setenv("QT_IM_MODULE", "qtvirtualkeyboard", 1);
        q_qtenvironmentvariables_qputenv("QT_IM_MODULE", "qtvirtualkeyboard");
    }

    QGuiApplication* qapp = q_guiapplication_new(&argc, argv);

    QQmlApplicationEngine* engine = q_qmlapplicationengine_new();
    q_qmlapplicationengine_load2(engine, "src/libraries/restricted-extras/virtualkeyboard/main.qml");

    q_qmlapplicationengine_on_quit(engine, on_quit);
    q_qmlapplicationengine_on_object_creation_failed(engine, on_failed);

    int result = q_guiapplication_exec();

    q_qmlapplicationengine_delete(engine);
    q_guiapplication_delete(qapp);

    return result;
}
