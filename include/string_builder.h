/*
 * Copyright (C) 2025 Adaías Magdiel
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef MGDL_STRING_BUILDER_H
#define MGDL_STRING_BUILDER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef MGDL_SB_IMPLEMENTATION
#define SB_DEFAULT_CAPACITY 256

// ================================================================
// Public API
// ================================================================

// StringBuilder structure
typedef struct StringBuilder {
  char *data;      // Pointer to the string data.
  size_t length;   // Current length of the string (excluding null terminator).
  size_t capacity; // Total allocated memory for the string.
} StringBuilder;

int sb_init(StringBuilder *sb);
int sb_ensure_capacity(StringBuilder *sb, size_t additional_length);
int sb_append(StringBuilder *sb, const char *string);
int sb_replace(StringBuilder *sb, const char *str1, const char *str2);
int sb_trim(StringBuilder *sb, const char *chars_to_trim);
int sb_ltrim(StringBuilder *sb, const char *chars_to_trim);
int sb_rtrim(StringBuilder *sb, const char *chars_to_trim);
int sb_reset(StringBuilder *sb);
void sb_reverse(StringBuilder *sb);
char **sb_split(const StringBuilder *sb, const char *delimiter, size_t *count);
StringBuilder *sb_split_to_builders(const StringBuilder *sb,
                                    const char *delimiter, size_t *count);
int sb_read_file(StringBuilder *sb, const char *filename);
void sb_free_array(StringBuilder *sbs, size_t count);
void sb_free(StringBuilder *sb);

// ================================================================
// Implementation
// ================================================================

// Initialize the StringBuilder
int sb_init(StringBuilder *sb) {
  if (!sb)
    return -1;

  sb->length = 0;
  sb->capacity = SB_DEFAULT_CAPACITY;

  sb->data = malloc(sb->capacity * sizeof(char));
  if (!sb->data)
    return -1;

  sb->data[0] = '\0';
  return 0;
}

// Ensures the StringBuilder has enough capacity for a given size.
// Returns 0 on success, -1 on failure.
int sb_ensure_capacity(StringBuilder *sb, size_t new_capacity) {
  if (!sb || !sb->data) {
    return -1;
  }

  if (new_capacity > sb->capacity) {
    size_t new_cap = new_capacity + (new_capacity / 2);
    char *new_data = realloc(sb->data, new_cap);
    if (!new_data) {
      return -1;
    }
    sb->data = new_data;
    sb->capacity = new_cap;
  }
  return 0;
}

// Appends a string to the StringBuilder.
// Returns the length of the appended string on success, -1 on failure.
int sb_append(StringBuilder *sb, const char *string) {
  if (!sb || !string) {
    return -1;
  }

  size_t str_len = strlen(string);
  size_t new_len = sb->length + str_len;

  if (new_len >= sb->capacity) {
    if (sb_ensure_capacity(sb, new_len + 1) != 0) {
      return -1;
    }
  }

  strcat(sb->data, string);
  sb->length = new_len;

  return str_len;
}

// Replaces all occurrences of `str1` with `str2` in the StringBuilder.
// Returns 0 on success, -1 on failure.
int sb_replace(StringBuilder *sb, const char *str1, const char *str2) {
  if (!sb || !sb->data || !str1 || !str2) {
    return -1;
  }

  size_t len_str1 = strlen(str1);
  size_t len_str2 = strlen(str2);

  if (len_str1 == 0) {
    return 0;
  }

  size_t count = 0;
  const char *pos = sb->data;
  while ((pos = strstr(pos, str1)) != NULL) {
    count++;
    pos += len_str1;
  }

  size_t new_length = sb->length + count * (len_str2 - len_str1);
  if (new_length >= sb->capacity) {
    if (sb_ensure_capacity(sb, new_length + 1) != 0) {
      return -1;
    }
  }

  char *temp = malloc(new_length + 1);
  if (!temp) {
    return -1;
  }

  const char *src = sb->data;
  char *dst = temp;
  while ((pos = strstr(src, str1)) != NULL) {
    size_t prefix_len = pos - src;

    memcpy(dst, src, prefix_len);
    dst += prefix_len;

    memcpy(dst, str2, len_str2);
    dst += len_str2;

    src = pos + len_str1;
  }

  size_t suffix_len = sb->length - (src - sb->data);
  memcpy(dst, src, suffix_len);
  dst += suffix_len;

  *dst = '\0';

  free(sb->data);
  sb->data = temp;
  sb->length = new_length;

  return 0;
}

// Removes leading and trailing characters from the StringBuilder.
// If `chars_to_trim` is NULL, trims whitespace characters (" \t\n\r").
// Returns 0 on success, -1 on failure.
int sb_trim(StringBuilder *sb, const char *chars_to_trim) {
  if (!sb || !sb->data) {
    return -1;
  }

  const char *trim_chars = (chars_to_trim != NULL) ? chars_to_trim : " \t\n\r";

  size_t start = 0;
  while (start < sb->length && strchr(trim_chars, sb->data[start]) != NULL) {
    start++;
  }

  size_t end = sb->length;
  while (end > start && strchr(trim_chars, sb->data[end - 1]) != NULL) {
    end--;
  }

  if (start >= end) {
    sb->data[0] = '\0';
    sb->length = 0;
    return 0;
  }

  memmove(sb->data, sb->data + start, end - start);

  sb->data[end - start] = '\0';
  sb->length = end - start;

  return 0;
}

// Removes leading characters from the StringBuilder.
// If `chars_to_trim` is NULL, trims whitespace characters (" \t\n\r").
// Returns 0 on success, -1 on failure.
int sb_ltrim(StringBuilder *sb, const char *chars_to_trim) {
  if (!sb || !sb->data) {
    return -1;
  }

  const char *trim_chars = (chars_to_trim != NULL) ? chars_to_trim : " \t\n\r";

  size_t start = 0;
  while (start < sb->length && strchr(trim_chars, sb->data[start]) != NULL) {
    start++;
  }

  if (start >= sb->length) {
    sb->data[0] = '\0';
    sb->length = 0;
    return 0;
  }

  memmove(sb->data, sb->data + start, sb->length - start);

  sb->data[sb->length - start] = '\0';
  sb->length = sb->length - start;

  return 0;
}

// Removes trailing characters from the StringBuilder.
// If `chars_to_trim` is NULL, trims whitespace characters (" \t\n\r").
// Returns 0 on success, -1 on failure.
int sb_rtrim(StringBuilder *sb, const char *chars_to_trim) {
  if (!sb || !sb->data) {
    return -1;
  }

  const char *trim_chars = (chars_to_trim != NULL) ? chars_to_trim : " \t\n\r";

  // ola....
  // 012^456  - length: 7 | end = 3

  size_t end = sb->length;
  while (end > 0 && strchr(trim_chars, sb->data[end - 1]) != NULL) {
    end--;
  }

  if (end == 0) {
    sb->data[0] = '\0';
    sb->length = 0;
    return 0;
  }

  memmove(sb->data, sb->data, end);

  sb->data[end] = '\0';
  sb->length = end;

  return 0;
}

// Resets the StringBuilder, effectively clearing its content.
// Returns 0 on success, -1 on failure.
int sb_reset(StringBuilder *sb) {
  if (!sb || !sb->data) {
    return -1;
  }

  sb->length = 0;
  sb->data[0] = '\0';

  return 0;
}

// Reverses the string in the StringBuilder (does not support multibyte
// characters).
void sb_reverse(StringBuilder *sb) {
  if (!sb || !sb->data) {
    return;
  }

  char buffer[sb->length + 1];
  strcpy(buffer, sb->data);

  for (size_t i = 0; i < sb->length; i++) {
    sb->data[i] = buffer[sb->length - i - 1];
  }
}

// Splits the StringBuilder into substrings based on a delimiter.
// Returns an array of strings and sets `count` to the number of substrings.
// The caller is responsible for freeing the memory allocated for the array and
// substrings.
char **sb_split(const StringBuilder *sb, const char *delimiter, size_t *count) {
  if (!sb || !sb->data || !count) {
    return NULL;
  }

  const char *delim =
      (delimiter != NULL && strlen(delimiter) > 0) ? delimiter : " ";

  size_t num_substrings = 0;
  const char *start = sb->data;
  const char *end;

  while ((end = strstr(start, delim)) != NULL) {
    if (end > start) {
      num_substrings++;
    }
    start = end + strlen(delim);
  }

  if (start < sb->data + sb->length) {
    num_substrings++;
  }

  char **substrings = malloc((num_substrings + 1) * sizeof(char *));
  if (!substrings) {
    return NULL;
  }

  size_t i = 0;
  start = sb->data;
  while ((end = strstr(start, delim)) != NULL) {
    if (end > start) {
      size_t len = end - start;
      substrings[i] = malloc((len + 1) * sizeof(char));
      if (!substrings[i]) {

        for (size_t j = 0; j < i; j++) {
          free(substrings[j]);
        }
        free(substrings);
        return NULL;
      }
      memcpy(substrings[i], start, len);
      substrings[i][len] = '\0';
      i++;
    }
    start = end + strlen(delim);
  }

  if (start < sb->data + sb->length) {
    size_t len = sb->data + sb->length - start;
    substrings[i] = malloc((len + 1) * sizeof(char));
    if (!substrings[i]) {

      for (size_t j = 0; j < i; j++) {
        free(substrings[j]);
      }
      free(substrings);
      return NULL;
    }
    memcpy(substrings[i], start, len);
    substrings[i][len] = '\0';
    i++;
  }

  substrings[num_substrings] = NULL;

  *count = num_substrings;

  return substrings;
}

// Splits the StringBuilder into an array of StringBuilders based on a
// delimiter. Returns an array of StringBuilders and sets `count` to the number
// of substrings. The caller is responsible for freeing the memory allocated for
// the array and the individual StringBuilders.
StringBuilder *sb_split_to_builders(const StringBuilder *sb,
                                    const char *delimiter, size_t *count) {
  if (!sb || !sb->data || sb->data[0] == '\0' || !count || !delimiter ||
      strlen(delimiter) == 0) {
    *count = 0;
    return NULL;
  }

  size_t num_substrings = 0;
  const char *start = sb->data;
  const char *end;

  while ((end = strstr(start, delimiter)) != NULL) {
    num_substrings++;
    start = end + strlen(delimiter);
  }

  if (*start) {
    num_substrings++;
  }

  StringBuilder *sub_sbs = malloc(num_substrings * sizeof(StringBuilder));
  if (!sub_sbs) {
    return NULL;
  }

  size_t i = 0;
  start = sb->data;
  while ((end = strstr(start, delimiter)) != NULL) {
    size_t len = end - start;
    sb_init(&sub_sbs[i]);
    sb_append(&sub_sbs[i], start);
    sub_sbs[i].data[len] = '\0'; // Garantir terminação correta
    sub_sbs[i].length = len;
    i++;
    start = end + strlen(delimiter);
  }

  if (*start) {
    sb_init(&sub_sbs[i]);
    sb_append(&sub_sbs[i], start);
    i++;
  }

  *count = num_substrings;
  return sub_sbs;
}

// Reads the content of a file and appends it to the StringBuilder.
// Returns 0 on success, -1 on failure.
int sb_read_file(StringBuilder *sb, const char *filename) {
  if (!sb || !filename) {
    return -1;
  }

  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    return -1;
  }

  fseek(fp, 0, SEEK_END);
  long file_size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  if (file_size == -1) {
    fclose(fp);
    return -1;
  }

  if (sb_ensure_capacity(sb, sb->length + file_size + 1) != 0) {
    fclose(fp);
    return -1;
  }

  size_t bytes_read = fread(sb->data + sb->length, 1, file_size, fp);
  if (bytes_read != (size_t)file_size) {
    fclose(fp);
    return -1;
  }

  sb->length += bytes_read;
  sb->data[sb->length] = '\0';

  fclose(fp);
  return 0;
}

// Free the StringBuilder
void sb_free(StringBuilder *sb) {
  if (sb && sb->data) {
    free(sb->data);
    sb->data = NULL;
    sb->length = 0;
    sb->capacity = SB_DEFAULT_CAPACITY;
  }
}

// Free an array of StringBuilders
void sb_free_array(StringBuilder *sbs, size_t count) {
  if (!sbs)
    return;
  for (size_t i = 0; i < count; i++) {
    sb_free(&sbs[i]);
  }
  free(sbs);
}

#endif
#endif
