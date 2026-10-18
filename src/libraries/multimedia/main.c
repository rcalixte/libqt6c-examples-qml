#include <libqt6c.h>

const char** name_filters() {
    QMediaFormat* media_format = q_mediaformat_new();
    libqt_list supported_formats = q_mediaformat_supported_file_formats(media_format,
                                                                        QMEDIAFORMAT_CONVERSIONMODE_DECODE);
    const char** results = (const char**)calloc(supported_formats.len + 1, sizeof *results);
    if (results == NULL) {
        fprintf(stderr, "Failed to allocate memory for results\n");
        q_mediaformat_delete(media_format);
        abort();
    }

    size_t index = 0;
    for (size_t i = 0; i < supported_formats.len; i++) {
        QMediaFormat* format = q_mediaformat_new3(supported_formats.data.ints[i]);
        QMimeType* mime = q_mediaformat_mime_type(format);
        const char** suffixes = q_mimetype_suffixes(mime);

        if (!q_mimetype_is_valid(mime) || libqt_strv_length(suffixes) == 0) {
            free(suffixes);
            q_mediaformat_delete(format);
            continue;
        }

        q_mimetype_delete(mime);

        const char* filter = q_mediaformat_file_format_description(supported_formats.data.ints[i]);
        size_t total = 1;
        for (size_t j = 0; suffixes[j] != NULL; j++)
            total += strlen(suffixes[j]) + (j > 0 ? 4 : 0);

        char* suffixes_str = (char*)malloc(total * sizeof *suffixes_str);
        if (suffixes_str == NULL) {
            fprintf(stderr, "Failed to allocate memory for suffixes_str\n");
            libqt_free(filter);
            free(suffixes);
            q_mediaformat_delete(format);
            abort();
        }

        char* p = suffixes_str;
        size_t remaining = total;
        for (size_t j = 0; suffixes[j] != NULL; j++) {
            int n = snprintf(p, remaining, "%s%s", j > 0 ? " *." : "", suffixes[j]);
            p += n;
            remaining -= (size_t)n;
        }

        size_t result_len = strlen(filter) + total + 6;
        char* result = (char*)malloc(result_len * sizeof *result);
        if (result == NULL) {
            fprintf(stderr, "Failed to allocate memory for result\n");
            free(suffixes_str);
            libqt_free(filter);
            free(suffixes);
            q_mediaformat_delete(format);
            abort();
        }
        snprintf(result, result_len, "%s (*.%s)", filter, suffixes_str);

        free(suffixes_str);
        libqt_free(filter);
        libqt_free(suffixes);
        q_mediaformat_delete(format);

        results[index] = result;
        index += 1;
    }

    return results;
}

void on_failed(void* engine UNUSED, const void* url UNUSED) {
    fprintf(stderr, "Failed to create QML object\n");
    q_guiapplication_quit();
    abort();
}

int main(int argc, char* argv[]) {
    QGuiApplication* qapp = q_guiapplication_new(&argc, argv);

    const char** filters = name_filters();

    libqt_map properties;
    properties.len = 3;
    const char** prop_keys = (const char**)malloc(properties.len * sizeof *prop_keys);
    if (prop_keys == NULL) {
        fprintf(stderr, "Failed to allocate memory for property key\n");
        free(filters);
        q_guiapplication_delete(qapp);
        abort();
    }
    prop_keys[0] = "source";
    prop_keys[1] = "nameFilters";
    prop_keys[2] = "selectedNameFilter";

    QVariant** prop_values = (QVariant**)malloc(properties.len * sizeof *prop_values);
    if (prop_values == NULL) {
        fprintf(stderr, "Failed to allocate memory for property value\n");
        libqt_free(prop_keys);
        q_guiapplication_delete(qapp);
        abort();
    }
    QVariant* source_variant = q_variant_new24("sample-10s-720p.mp4");
    QVariant* filters_variant = q_variant_new25(filters);
    QVariant* preferred_variant = q_variant_new4(0);
    prop_values[0] = source_variant;
    prop_values[1] = filters_variant;
    prop_values[2] = preferred_variant;

    properties.keys = (void*)prop_keys;
    properties.values = (void*)prop_values;

    QQmlApplicationEngine* engine = q_qmlapplicationengine_new();
    q_qmlapplicationengine_set_initial_properties(engine, properties);
    q_qmlapplicationengine_on_object_creation_failed(engine, on_failed);
    q_qmlapplicationengine_load2(engine, "src/libraries/multimedia/main.qml");

    int result = q_guiapplication_exec();

    free(filters);
    q_variant_delete(preferred_variant);
    q_variant_delete(filters_variant);
    q_variant_delete(source_variant);
    free(prop_values);
    libqt_free(prop_keys);
    q_qmlapplicationengine_delete(engine);
    q_guiapplication_delete(qapp);

    return result;
}
