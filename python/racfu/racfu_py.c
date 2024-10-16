#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include "racfu.hpp"

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

// Method docstrings
static char call_racfu_docs[] =
    "call_racfu(self, request_json: string) -> dict: Invokes racfu with "
    "the specified json request."
    "Returns this information in a python dictionary. Included in this "
    "dictionary are return and reason codes from R_Admin and IRRSMO00 "
    "RACF Callable Services and racfu itself.\n";

// Method definition
static PyMethodDef racfu_py_methods[] = {
    {"call_racfu", (PyCFunction)call_racfu, METH_O, call_racfu_docs},
    {NULL}
};

// Module definition
static struct PyModuleDef racfu_py_module_def = {
    PyModuleDef_HEAD_INIT, "racfu_py",
    "Thin connecting layer that allows python code to invoke racfu directly.\n",
    -1, racfu_py_methods};

// Module initialization function
PyMODINIT_FUNC PyInit_racfu_py(void) {
  Py_Initialize();
  return PyModule_Create(&racfu_py_module_def);
}