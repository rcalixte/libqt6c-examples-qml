#include <libqt6c.h>

void on_failed(void* engine UNUSED, const void* url UNUSED) {
    fprintf(stderr, "Failed to create QML object\n");
    q_application_quit();
    abort();
}

int main(int argc, char* argv[]) {
    k_icontheme_init_theme();

    QApplication* qapp = q_application_new(&argc, argv);

    k_localizedstring_set_application_domain("tutorial");
    q_application_set_organization_name("KDE");
    q_application_set_organization_domain("kde.org");
    q_application_set_application_name("Qt 6 Kirigami Example");
    q_application_set_desktop_file_name("org.kde.tutorial");

    q_application_set_style2("breeze");
    if (getenv("QT_QUICK_CONTROLS_STYLE") == NULL)
        q_quickstyle_set_style("org.kde.desktop");

    QQmlApplicationEngine* engine = q_qmlapplicationengine_new();
    KLocalizedQmlContext* context = k_localizedqmlcontext_new2(engine);
    q_qmlcontext_set_context_object(q_qmlapplicationengine_root_context(engine), context);
    q_qmlapplicationengine_on_object_creation_failed(engine, on_failed);
    q_qmlapplicationengine_load2(engine, "src/libraries/extras/kirigami/main.qml");

    libqt_list objects = q_qmlapplicationengine_root_objects(engine);
    if (objects.len == 0) {
        fprintf(stderr, "No objects found in the QML file\n");
        libqt_free(objects.data.ptr);
        q_qmlapplicationengine_delete(engine);
        q_application_delete(qapp);
        return 1;
    }

    int result = q_application_exec();

    libqt_free(objects.data.ptr);
    q_qmlapplicationengine_delete(engine);
    q_application_delete(qapp);

    return result;
}
