#include <python.h>
#include "racf_result.h"

extern "C" {
  void racfu(racfu_result_t *result, char *request_json);
}

// Entry point to the call_RACFu() function
static PyObject* call_RACFu(PyObject* self, PyObject* args, PyObject *kwargs) {
  PyObject* request_dictionary;
  PyObject* result_dictionary;

  static char *kwlist[] = {"request_dictionary", NULL};

  printf("about to parse args!\n");
  if (!PyArg_ParseTupleAndKeywords(
      args, kwargs, "O", kwlist,
      request_dictionary
    )) {
      return NULL;
  }

  request_dictionary = PyObject_Repr(request_dictionary);
  const char* request_as_string = PyStrin_AsString(request_dictionary);

  racfu_result_t result;

  racfu(&result,request_as_string);

  result_dictionary = Py_BuildValue(
    "{s:y,s:B,s:s}",
    "raw_result", result.raw_result,
    "raw_result_length", result.raw_result_length,
    "result_json", result.result_json
  );

  free(result.raw_result);
  free(result.result_json);

  return result_dictionary;
}

//Method docstrings
static char call_RACFu_docs[] =
   "call_RACFu(request_dictionary: dict): Invokes RACFu with the specified json request."
   "Returns this information in a python dictionary. Included in this dictionary are "
   "return and reason codes from R_Admin and IRRSMO00 RACF Callable Services and RACFu itself.\n";

// Method definition
static PyMethodDef pyRACFu_core_methods[] = {
   {"call_RACFu", (PyCFunction)call_RACFu,
      METH_VARARGS | METH_KEYWORDS, call_RACFu_docs},
  {NULL}
};

//Module definition
static struct PyModuleDef pyRACFu_core_module_def =
{
        PyModuleDef_HEAD_INIT,
        "pyRACFu_core", 
        "Thin connecting layer that allows pyRACFu to invoke RACFu directly.\n",
        -1,
        pyRACFu_core_methods
};

//Module initialization function
PyMODINIT_FUNC PyInit_pyRACFu_core(void)
{
        Py_Initialize();
        return PyModule_Create(&pyRACFu_core_module_def);
}