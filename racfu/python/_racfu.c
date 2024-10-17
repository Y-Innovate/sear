#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include "racfu.h"

// Entry point to the call_racfu() function
static PyObject* call_racfu(PyObject* self, PyObject* args) {
  PyObject *request_dictionary, *return_dictionary;

  request_dictionary = PyObject_Str(args);
  const char* request_as_string = PyUnicode_AsUTF8(request_dictionary);

  racfu_result_t result;

  racfu(&result, request_as_string);

  return_dictionary = Py_BuildValue(
      "{s:y#,s:y#,s:s}", "raw_request", result.raw_request,
      result.raw_request_length, "raw_result", result.raw_result,
      result.raw_result_length, "result_json", result.result_json);

  free(result.raw_request);
  free(result.raw_result);
  free(result.result_json);

  return return_dictionary;
}

// Method definition
static PyMethodDef _C_methods[] = {
    {"call_racfu", (PyCFunction)call_racfu, METH_O,
     "Python interface to RACF administration APIs"},
    {NULL}
};

// Module definition
static struct PyModuleDef _C_module_def = {
    PyModuleDef_HEAD_INIT, "_C", "Python interface to RACF administration APIs",
    -1, _C_methods};

// Module initialization function
PyMODINIT_FUNC PyInit__C(void) {
  Py_Initialize();
  return PyModule_Create(&_C_module_def);
}