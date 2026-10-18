// This example needs to be built similar to the following command:
// zig build classinfo -Dqtprivate-paths={"/usr/include/x86_64-linux-gnu/qt6/QtCore/6.8.2","/usr/include/x86_64-linux-gnu/qt6/QtCore/6.8.2/QtCore/private"}

#include <libqt6c.h>

static QMetaObject* shelf_meta = NULL;

const QMetaObject* on_meta_object(const void* self) {
    if (shelf_meta != NULL)
        return shelf_meta;

    QMetaObjectBuilder* builder = q_metaobjectbuilder_new();

    q_metaobjectbuilder_set_class_name(builder, "Shelf");
    q_metaobjectbuilder_set_super_class(builder, q_object_super_meta_object(self));
    q_metaobjectbuilder_add_class_info(builder, "DefaultProperty", "items");

    shelf_meta = q_metaobjectbuilder_to_meta_object(builder);

    q_metaobjectbuilder_delete(builder);

    return shelf_meta;
}

const char* default_property(void* object) {
    const QMetaObject* mo = q_object_meta_object(object);
    int index = q_metaobject_index_of_class_info(mo, "DefaultProperty");
    if (index < 0)
        return NULL;

    QMetaClassInfo* info = q_metaobject_class_info(mo, index);
    const char* value = q_metaclassinfo_value(info);
    q_metaclassinfo_delete(info);

    return value;
}

void append_to_default(void* object, void* child) {
    const char* prop = default_property(object);
    q_object_set_parent(child, object);

    QVariant* current = q_object_property(object, prop);
    const char* old = q_variant_to_string(current);
    const char* name = q_object_object_name(child);

    size_t old_len = (old == NULL) ? 0 : strlen(old);
    size_t name_len = strlen(name);

    char* maybe_name = (char*)malloc(old_len + name_len + 3);
    if (maybe_name == NULL) {
        fprintf(stderr, "Failed to allocate memory for maybe_name\n");
        libqt_free(name);
        libqt_free(old);
        q_variant_delete(current);
        abort();
    }

    const char* joined = name;
    if (old_len != 0) {
        snprintf(maybe_name, old_len + name_len + 3, "%s, %s", old, name);
        joined = maybe_name;
    }

    QVariant* updated = q_variant_new24(joined);
    q_object_set_property(object, prop, updated);

    q_variant_delete(updated);
    libqt_free(maybe_name);
    libqt_free(name);
    libqt_free(old);
    q_variant_delete(current);
}

const char* describe(void* object) {
    const char* prop = default_property(object);
    QVariant* value = q_object_property(object, prop);
    const char* value_str = q_variant_to_string(value);
    const char* class_name = q_metaobject_class_name(q_object_meta_object(object));
    size_t maxlen = strlen(class_name) + strlen(prop) + strlen(value_str) + 4;

    char* description = (char*)malloc(maxlen);
    if (description == NULL) {
        fprintf(stderr, "Failed to allocate memory for description\n");
        libqt_free(value_str);
        q_variant_delete(value);
        abort();
    }

    snprintf(description, maxlen, "%s.%s: %s",
             class_name, prop, value_str);

    libqt_free(value_str);
    q_variant_delete(value);

    return description;
}

void on_failed(void* engine UNUSED, const void* url UNUSED) {
    fprintf(stderr, "Failed to create QML object\n");
    q_guiapplication_quit();
    abort();
}

int main(int argc, char* argv[]) {
    QGuiApplication* qapp = q_guiapplication_new(&argc, argv);

    QObject* shelf = q_object_new();
    q_object_on_meta_object(shelf, on_meta_object);

    // equivalent of QML:
    // Shelf {
    //   QtObject { objectName: "Book" }
    //   QtObject { objectName: "Lamp" }
    //   QtObject { objectName: "Plant" }
    // }
    const char* names[] = {"Book", "Lamp", "Plant", NULL};
    for (size_t i = 0; names[i] != NULL; i++) {
        QObject* item = q_object_new();
        q_object_set_object_name(item, names[i]);
        append_to_default(shelf, item);
    }

    libqt_map properties;
    properties.len = 1;
    const char** prop_keys = (const char**)malloc(properties.len * sizeof *prop_keys);
    if (prop_keys == NULL) {
        fprintf(stderr, "Failed to allocate memory for property key\n");
        q_object_delete(shelf);
        q_guiapplication_delete(qapp);
        abort();
    }
    prop_keys[0] = "description";

    QVariant** prop_values = (QVariant**)malloc(properties.len * sizeof *prop_values);
    if (prop_values == NULL) {
        fprintf(stderr, "Failed to allocate memory for property value\n");
        libqt_free(prop_keys);
        q_object_delete(shelf);
        q_guiapplication_delete(qapp);
        abort();
    }

    const char* text = describe(shelf);
    QVariant* variant = q_variant_new24(text);
    prop_values[0] = variant;

    properties.keys = (void*)prop_keys;
    properties.values = (void*)prop_values;

    QQmlApplicationEngine* engine = q_qmlapplicationengine_new();
    q_qmlapplicationengine_set_initial_properties(engine, properties);
    q_qmlapplicationengine_on_object_creation_failed(engine, on_failed);
    q_qmlapplicationengine_load2(engine, "src/libraries/qml/classinfo/main.qml");

    int result = q_guiapplication_exec();

    q_variant_delete(variant);
    libqt_free(text);
    free(prop_values);
    libqt_free(prop_keys);
    q_object_delete(shelf);
    q_qmlapplicationengine_delete(engine);
    q_guiapplication_delete(qapp);

    return result;
}
