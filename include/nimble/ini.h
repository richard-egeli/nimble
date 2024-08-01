#ifndef INCLUDE_NIMBLE_INI_H_
#define INCLUDE_NIMBLE_INI_H_

typedef struct Section Section;

typedef struct Settings Settings;

int ini_property_get_int(Section* section, const char* key);

const char* ini_property_get_str(Section* section, const char* key);

Section* ini_section_get(Settings* ini, const char* key);

Settings* ini_load(const char* filename);

#endif  // INCLUDE_NIMBLE_INI_H_
