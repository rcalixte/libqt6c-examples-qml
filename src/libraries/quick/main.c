#include <libqt6c.h>

static QLabel* label = NULL;
static QGenericArgument* ret = NULL;
static QQuickItem* root = NULL;

void on_quit(void* engine UNUSED) {
    q_application_quit();
}

void on_clicked(void* button UNUSED) {
    q_metaobject_invoke_method4(root, "reset", ret);
    q_label_set_text(label, "### Counter reset from C!");
}

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    QWidget* widget = q_widget_new2();
    QVBoxLayout* layout = q_vboxlayout_new(widget);

    QQuickWidget* quick_widget = q_quickwidget_new2();
    q_quickwidget_set_resize_mode(quick_widget, QQUICKWIDGET_RESIZEMODE_SIZEROOTOBJECTTOVIEW);
    QUrl* url = q_url_from_local_file("src/libraries/quick/main.qml");

    q_quickwidget_set_source(quick_widget, url);

    if (q_quickwidget_status(quick_widget) == QQUICKWIDGET_STATUS_ERROR) {
        libqt_list errors = q_quickwidget_errors(quick_widget);
        QQmlError** err_data = (QQmlError**)errors.data.ptr;
        for (size_t i = 0; i < errors.len; i++) {
            const char* err_str = q_qmlerror_to_string(err_data[i]);
            fprintf(stderr, "%s\n", err_str);
            libqt_free(err_str);
            q_qmlerror_delete(err_data[i]);
        }
        free(err_data);
        q_url_delete(url);
        q_quickwidget_delete(quick_widget);
        q_widget_delete(widget);
        q_application_delete(qapp);
        abort();
    }

    q_url_delete(url);

    q_qmlengine_on_quit(q_quickwidget_engine(quick_widget), on_quit);

    label = q_label_new3("### Click inside the shaded area above.");
    q_label_set_text_format(label, QT_TEXTFORMAT_MARKDOWNTEXT);
    QPushButton* button = q_pushbutton_new3("Reset the counter");

    q_vboxlayout_add_widget2(layout, quick_widget, 1);
    q_vboxlayout_add_widget(layout, label);
    q_vboxlayout_add_widget(layout, button);

    root = q_quickwidget_root_object(quick_widget);
    const QMetaObject* root_meta = q_quickitem_meta_object(root);
    const QMetaObject* label_meta = q_label_meta_object(label);
    const char* root_signal = "tapped(QString)";
    const char* label_slot = "setText(QString)";
    int signal_index = q_metaobject_index_of_signal(root_meta, root_signal);
    int slot_index = q_metaobject_index_of_slot(label_meta, label_slot);
    if (signal_index < 0) {
        fprintf(stderr, "Could not find `%s` signal\n", root_signal);
        q_widget_delete(widget);
        q_application_delete(qapp);
        abort();
    }
    if (slot_index < 0) {
        fprintf(stderr, "Could not find `%s` slot\n", label_slot);
        q_widget_delete(widget);
        q_application_delete(qapp);
        abort();
    }

    QMetaMethod* root_method = q_metaobject_method(root_meta, signal_index);
    QMetaMethod* label_method = q_metaobject_method(label_meta, slot_index);

    ret = q_genericargument_new3();

    q_object_connect2(root, root_method, label, label_method);
    q_pushbutton_on_clicked(button, on_clicked);

    q_widget_set_window_title(widget, "Qt 6 Quick Example");
    q_widget_resize(widget, 450, 400);
    q_widget_show(widget);

    int result = q_application_exec();

    q_genericargument_delete(ret);
    q_metamethod_delete(root_method);
    q_metamethod_delete(label_method);
    q_widget_delete(widget);
    q_application_delete(qapp);

    return result;
}
