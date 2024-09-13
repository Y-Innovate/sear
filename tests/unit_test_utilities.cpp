#include "unit_test_utilities.hpp"

#include <stdio.h>
#include <stdlib.h>

#include <nlohmann/json.hpp>

char *get_sample(const char *filename, const char *mode) {
  // open file
  FILE *fp = fopen(filename, mode);
  if (fp == NULL) {
    perror("");
    printf("Unable to open sample '%s' in '%s' mode for reading.\n", filename,
           mode);
    exit(1);
  }
  // get size of file
  fseek(fp, 0L, SEEK_END);
  int size = ftell(fp);
  rewind(fp);
  // allocate space to read in data from file
  char *file_data = (char *)calloc(size + 1, sizeof(char));
  if (file_data == NULL) {
    perror("");
    printf("Unable to allocate space to load data from '%s'.\n", filename);
    fclose(fp);
    exit(1);
  }
  // read file data
  fread(file_data, size, 1, fp);
  fclose(fp);
  return file_data;
}

char *get_raw_sample(const char *filename) {
  return get_sample(filename, "rb");
}

std::string get_json_sample(const char *filename) {
  char *json_sample_string = get_sample(filename, "r");
  std::string json_sample_cpp_string =
      nlohmann::json::parse(json_sample_string).dump();
  free(json_sample_string);
  return json_sample_cpp_string;
}
