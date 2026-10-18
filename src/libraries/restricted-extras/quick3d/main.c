#include <libqt6c.h>

void on_failed(void* engine UNUSED, const void* url UNUSED) {
    fprintf(stderr, "Failed to create QML object\n");
    q_guiapplication_quit();
    abort();
}

int main(int argc, char* argv[]) {
    QGuiApplication* qapp = q_guiapplication_new(&argc, argv);

    q_surfaceformat_set_default_format(q_quick3d_ideal_surface_format1(4));

    QQmlApplicationEngine* engine = q_qmlapplicationengine_new();
    q_qmlapplicationengine_on_object_creation_failed(engine, on_failed);
    q_qmlapplicationengine_load2(engine, "src/libraries/restricted-extras/quick3d/main.qml");
    if (q_qmlapplicationengine_root_objects(engine).len == 0) {
        fprintf(stderr, "No objects found in the QML file\n");
        return 1;
        q_qmlapplicationengine_delete(engine);
        q_guiapplication_delete(qapp);
    }

    int result = q_guiapplication_exec();

    q_qmlapplicationengine_delete(engine);
    q_guiapplication_delete(qapp);

    return result;
}
