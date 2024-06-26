#ifdef ENABLE_PYTHON
#include <Python.h>
#include "metastore.h"
#include "metaentry.h"

static PyObject* compare(PyObject* self, PyObject* args);
static PyObject* savemetadata(PyObject* self, PyObject* args);

static PyMethodDef module_methods[] = {
	{"compare", compare, METH_VARARGS, "returns list of files changed"},
	{"savemetadata", savemetadata, METH_VARARGS, "saves metadata file"},
	{NULL, NULL, 0, NULL}  /* Sentinel */
};

static PyObject* savemetadata(PyObject* self, PyObject* args) {
	char * path;
	char * libmetastorefile;
	static struct metasettings settings = {
		.metafile = METAFILE,
		.do_mtime = false,
		.do_emptydirs = false,
		.do_removeemptydirs = false,
		.do_git = false,
	};


	// Parse the input arguments
	if (!PyArg_ParseTuple(args, "ss", &path, &libmetastorefile)) {
		return NULL;
	}

	int i, c;
	struct metahash *real = NULL;

	settings.metafile = strdup( libmetastorefile );
	mentries_recurse_path(path, &real, &settings);
	mentries_tofile(real, settings.metafile);

	Py_INCREF(Py_True);
	return Py_True;
}


//pfunc(real, stored, mentry_compare(real, stored, st));
void python_compare_callback( struct metaentry *real, struct metaentry *stored,  int cmp, void * param ) {
	char * path;

	if (!real && (!stored || (cmp == DIFF_NONE || cmp & DIFF_ADDED))) {
		return;
	}
	if (cmp == DIFF_NONE) {
		return;
	}

	path = real ? real->path : stored->path;

	PyObject* py_long_cmp = PyLong_FromLong(cmp);
	PyObject* py_str_path = PyUnicode_FromString(path);
	PyObject* item_list = PyList_New(2);
	PyList_SET_ITEM(item_list, 0, py_str_path);
	PyList_SET_ITEM(item_list, 1, py_long_cmp);

	PyList_Append(param, item_list);


	return;
}

static PyObject* compare(PyObject* self, PyObject* args) {
	const char *path, *libmetastorefile;

	static struct metasettings settings = {
		.metafile = METAFILE,
		.do_mtime = false,
		.do_emptydirs = false,
		.do_removeemptydirs = false,
		.do_git = false,
	};

	// Parse the input arguments
	if (!PyArg_ParseTuple(args, "ss", &path, &libmetastorefile)) {
		return NULL;
	}

	PyObject* list = PyList_New(0);
	if (list == NULL) {
		return NULL;
	}

	// Add the strings to the list
	//PyList_SET_ITEM(list, 0, PyUnicode_FromString(str1));
	//PyList_SET_ITEM(list, 1, PyUnicode_FromString(str2));

	//if (PyList_Append(list, item) == -1) {
	//	Py_DECREF(item);
	//	Py_DECREF(list);
	//	return NULL;
	//}

	// Return the list
	int i, c;
	struct metahash *real = NULL;
	struct metahash *stored = NULL;
	int action = 0;

	settings.do_mtime = true;
	settings.metafile = strdup( libmetastorefile );

	mentries_fromfile(&stored, settings.metafile);
	if (!stored) {
		return Py_None;
	}

	mentries_recurse_path(path, &real, &settings);

	mentries_compare(real, stored, python_compare_callback, &settings, list);

	return list;
}

static struct PyModuleDef module_def = {
	PyModuleDef_HEAD_INIT,
	"libmetastore",
	NULL,
	-1,
	module_methods
};

PyMODINIT_FUNC PyInit_libmetastore(void) {
  // Create the module object
  PyObject* module = PyModule_Create(&module_def);
  if (module == NULL) {
      return NULL;
  }
  
  // Add the constants to the module
  PyModule_AddIntConstant(module, "DIFF_NONE", DIFF_NONE );
  PyModule_AddIntConstant(module, "DIFF_OWNER", DIFF_OWNER );
  PyModule_AddIntConstant(module, "DIFF_GROUP", DIFF_GROUP );
  PyModule_AddIntConstant(module, "DIFF_MODE", DIFF_MODE );
  PyModule_AddIntConstant(module, "DIFF_TYPE", DIFF_TYPE );
  PyModule_AddIntConstant(module, "DIFF_MTIME", DIFF_MTIME );
  PyModule_AddIntConstant(module, "DIFF_XATTR", DIFF_XATTR );
  PyModule_AddIntConstant(module, "DIFF_ADDED", DIFF_ADDED );
  PyModule_AddIntConstant(module, "DIFF_DELE", DIFF_DELE );

    
	return module;
}
#endif
