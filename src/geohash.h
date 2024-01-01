#ifndef GEOHASH_H
#define GEOHASH_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

// Validate and decode a geohash into a lat/lon pair. Returns true on success,
// or false if the buffer does not contain a valid geohash.
bool geohash_decode(const char *buf, size_t len, double *lat, double *lon);

// Store a geohash encoding of a lat/lon pair at a given precision in a buffer.
// Adds a null terminator to the end of the buffer. Returns true on success,
bool geohash_encode(double lat, double lon, size_t precision, char *buf);

#endif