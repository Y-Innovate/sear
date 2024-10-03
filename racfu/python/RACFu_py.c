#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include "racfu_result.h"
#include <stdio.h>


extern void racfu(racfu_result_t* result, const char* request_json);

void print_hex_string(char *s, int len){
  for(int i=0; i<len; i++){
    printf("%02x", (unsigned int) *(s+i));
  }
  printf("\n");
}

// Entry point to the call_RACFu() function
static PyObject* call_RACFu(PyObject* self, PyObject* args) {
  PyObject *request_dictionary, *return_dictionary;

  request_dictionary = PyObject_Str(args);
  const char* request_as_string = PyUnicode_AsUTF8(request_dictionary);

  racfu_result_t result;

  racfu(&result, request_as_string);

  printf("raw_request");
  print_hex_string(result.raw_request, result.raw_request_length);
  printf("raw_result");
  print_hex_string(result.raw_result, result.raw_result_length);

  return_dictionary = Py_BuildValue(
      "{s:y#,s:y#,s:s}", "raw_request", result.raw_request, result.raw_request_length,
      "raw_result", result.raw_result, result.raw_result_length,
      "result_json", result.result_json);

  free(result.raw_request);
  free(result.raw_result);
  free(result.result_json);

  return return_dictionary;
}

// Method docstrings
static char call_RACFu_docs[] =
    "call_RACFu(self, request_json: string) -> dict: Invokes RACFu with "
    "the specified json request."
    "Returns this information in a python dictionary. Included in this "
    "dictionary are return and reason codes from R_Admin and IRRSMO00 "
    "RACF Callable Services and RACFu itself.\n";

// Method definition
static PyMethodDef RACFu_py_methods[] = {
    {"call_RACFu", (PyCFunction)call_RACFu, METH_O, call_RACFu_docs},
    {NULL}
};

// Module definition
static struct PyModuleDef RACFu_py_module_def = {
    PyModuleDef_HEAD_INIT, "RACFu_py",
    "Thin connecting layer that allows python code to invoke RACFu directly.\n",
    -1, RACFu_py_methods};

// Module initialization function
PyMODINIT_FUNC PyInit_RACFu_py(void) {
  Py_Initialize();
  return PyModule_Create(&RACFu_py_module_def);
}