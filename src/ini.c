#include "nimble/ini.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashmap/hashmap.h"

static char section_key[64];
static char sub_section_key[64];

typedef struct Section {
    HashMap* properties;  // String - String
} Section;

typedef struct Settings {
    char section[64];
    HashMap* sections;
} Settings;

static Section* section_create(void) {
    Section* ini    = malloc(sizeof(*ini));
    ini->properties = hmap_new();
    return ini;
}

static void ini_section_add(Settings* ini, const char* name) {
    assert(ini != NULL);
    assert(name != NULL);

    Section* section = section_create();
    hmap_put(ini->sections, name, section);
}

static bool ini_parse_section(Settings* ini, const char* buffer) {
    char section[64];
    int count = sscanf(buffer, "[%[^]]", section);
    if (count == 1) {
        size_t length = strlen(section);
        memcpy(ini->section, section, length);
        ini_section_add(ini, section);
        ini->section[length] = '\0';
    }

    return count != 0;
}

static void trim(char* buf) {
    size_t len = strlen(buf);
    char* end  = &buf[len - 1];
    while (isspace(*end)) {
        end--;
    }

    *(end + 1) = '\0';
}

static bool ini_parse_bucket(Settings* ini, const char* buffer) {
    char key[64];
    char value[64];
    int count = sscanf(buffer, "%63[^=]= %63[^\n]", key, value);

    if (count == 2) {
        trim(key);
        trim(value);

        Section* section = ini_section_get(ini, ini->section);
        if (section != NULL) {
            char* copy = strdup(value);
            if (!hmap_put(section->properties, key, copy)) {
                free(copy);
            }
        }
    }

    return count == 2;
}

int ini_property_get_int(Section* section, const char* key) {
    char* property = NULL;
    if (hmap_get(section->properties, key, (void**)&property)) {
        return atoi(property);
    }

    return 0;
}

const char* ini_property_get_str(Section* section, const char* key) {
    char* property = NULL;
    if (hmap_get(section->properties, key, (void**)&property)) {
        return property;
    }

    return NULL;
}

Section* ini_section_get(Settings* ini, const char* key) {
    Section* section = NULL;
    hmap_get(ini->sections, key, (void**)&section);
    return section;
}

Settings* ini_load(const char* filename) {
    Settings* ini = malloc(sizeof(*ini));
    if (ini == NULL) {
        perror("failed to allocate settings");
        return NULL;
    }

    ini->sections = hmap_new();

    FILE* file    = fopen(filename, "r");
    if (file == NULL) {
        perror("failed to open ini file");
        return NULL;
    }

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), file)) {
        if (strlen(buffer) - 1 == 0) continue;
        if (ini_parse_section(ini, buffer)) continue;
        if (ini_parse_bucket(ini, buffer)) continue;
    }

    fclose(file);
    return ini;
}
