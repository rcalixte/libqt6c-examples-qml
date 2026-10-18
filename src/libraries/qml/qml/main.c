#include <libqt6c.h>

static char buf[16];

int on_row_count(const void* self UNUSED, const void* index UNUSED) {
    return 1000;
}

QVariant* on_data(const void* self UNUSED, const void* index, int role) {
    if (!q_modelindex_is_valid(index))
        return q_variant_new();

    switch (role) {
    case QT_ITEMDATAROLE_DISPLAYROLE: {
        snprintf(buf, sizeof(buf), "this is row %d", q_modelindex_row(index));
        return q_variant_new24(buf);
    }
    default:
        return q_variant_new();
    }
}

void on_quit(void* engine UNUSED) {
    q_guiapplication_quit();
}

void on_failed(void* engine UNUSED, const void* url UNUSED) {
    fprintf(stderr, "Failed to create QML object\n");
    q_guiapplication_quit();
    abort();
}

int main(int argc, char* argv[]) {
    QGuiApplication* qapp = q_guiapplication_new(&argc, argv);

    QQmlApplicationEngine* engine = q_qmlapplicationengine_new();
    QAbstractListModel* model = q_abstractlistmodel_new();
    q_abstractlistmodel_on_row_count(model, on_row_count);
    q_abstractlistmodel_on_data(model, on_data);
    q_qmlapplicationengine_on_quit(engine, on_quit);
    q_qmlapplicationengine_on_object_creation_failed(engine, on_failed);
    q_qmlcontext_set_context_property(q_qmlapplicationengine_root_context(engine), "my_model", model);
    q_qmlapplicationengine_load2(engine, "src/libraries/qml/qml/main.qml");

    int result = q_guiapplication_exec();

    q_abstractlistmodel_delete(model);
    q_qmlapplicationengine_delete(engine);
    q_guiapplication_delete(qapp);

    return result;
}
