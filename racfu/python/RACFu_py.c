#include <Python.h>

#include "racfu_result.h"

extern void racfu(racfu_result_t* result, const char* request_json);

// Entry point to the call_RACFu() function
static PyObject* call_RACFu(PyObject* self, PyObject* args) {
  PyObject *request_dictionary, *return_dictionary;

  request_dictionary = PyObject_Str(args);
  const char* request_as_string = PyUnicode_AsUTF8(request_dictionary);

  racfu_result_t result;

  racfu(&result, request_as_string);

  return_dictionary = Py_BuildValue(
      "{s:y,s:B,s:y,s:B,s:s}", "raw_request", result.raw_request,
      "raw_request_length", result.raw_request_length, "raw_result",
      result.raw_result, "raw_result_length", result.raw_result_length,
      "result_json", result.result_json);

  free(result.raw_request);
  free(result.raw_result);
  free(result.result_json);

  return return_dictionary;
}

// Method docstrings
static char call_RACFu_docs[] =
    "call_RACFu(request_json: string): Invokes RACFu with the specified "
    "json request."
    "Returns this information in a python dictionary. Included in this "
    "dictionary are "
    "return and reason codes from R_Admin and IRRSMO00 RACF Callable Services "
    "and RACFu itself.\n";

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