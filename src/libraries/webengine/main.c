#include <libqt6c.h>

void on_failed(void* engine UNUSED, const void* url UNUSED) {
    fprintf(stderr, "Failed to create QML object\n");
    q_guiapplication_quit();
    abort();
}

int main(int argc, char* argv[]) {
    q_coreapplication_set_organization_name("Qt 6 WebEngine Example");
    q_coreapplication_set_attribute(QT_APPLICATIONATTRIBUTE_AA_SHAREOPENGLCONTEXTS);
    q_twebenginequick_initialize();
    QGuiApplication* qapp = q_guiapplication_new(&argc, argv);

    QQmlApplicationEngine* engine = q_qmlapplicationengine_new();
    q_qmlapplicationengine_on_object_creation_failed(engine, on_failed);
    q_qmlapplicationengine_load2(engine, "src/libraries/webengine/main.qml");

    int result = q_guiapplication_exec();

    q_qmlapplicationengine_delete(engine);
    q_guiapplication_delete(qapp);

    return result;
}
