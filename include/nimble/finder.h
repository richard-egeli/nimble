#ifndef INCLUDE_NIMBLE_FINDER_H_
#define INCLUDE_NIMBLE_FINDER_H_

typedef struct Finder {
    char* filter;
} Finder;

void finder_open(Finder* finder);

void finder_close(Finder* finder);

#endif  // INCLUDE_NIMBLE_FINDER_H_
