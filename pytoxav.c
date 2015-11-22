/**
 * pytoxcore
 *
 * Copyright (C) 2015 Anton Batenev <antonbatenev@yandex.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
//----------------------------------------------------------------------------------------------
#include "pytoxcore.h"
//----------------------------------------------------------------------------------------------
#define CHECK_TOXAV(self)                                        \
    if ((self)->av == NULL) {                                    \
        PyErr_SetString(ToxAVException, "toxav object killed."); \
        return NULL;                                             \
    }
//----------------------------------------------------------------------------------------------
PyObject* ToxAVException;
//----------------------------------------------------------------------------------------------

static void callback_toxav_call(ToxAV* av, uint32_t friend_number, bool audio_enabled, bool video_enabled, void* self)
{
    PyObject_CallMethod((PyObject*)self, "toxav_call_cb", "III", friend_number, audio_enabled, video_enabled);
}
//----------------------------------------------------------------------------------------------

static PyObject* ToxAV_callback_stub(ToxCoreAV* self, PyObject* args)
{
    Py_RETURN_NONE;
}
//----------------------------------------------------------------------------------------------

static PyObject* ToxAV_toxav_version_major(ToxCoreAV* self, PyObject* args)
{
    uint32_t result = toxav_version_major();

    return PyLong_FromUnsignedLong(result);
}
//----------------------------------------------------------------------------------------------

static PyObject* ToxAV_toxav_version_minor(ToxCoreAV* self, PyObject* args)
{
    uint32_t result = toxav_version_minor();

    return PyLong_FromUnsignedLong(result);
}
//----------------------------------------------------------------------------------------------

static PyObject* ToxAV_toxav_version_patch(ToxCoreAV* self, PyObject* args)
{
    uint32_t result = toxav_version_patch();

    return PyLong_FromUnsignedLong(result);
}
//----------------------------------------------------------------------------------------------

static PyObject* ToxAV_toxav_version_is_compatible(ToxCoreAV* self, PyObject* args)
{
    uint32_t major;
    uint32_t minor;
    uint32_t patch;

    if (PyArg_ParseTuple(args, "III", &major, &minor, &patch) == false)
        return NULL;

    bool result = toxav_version_is_compatible(major, minor, patch);

    return PyBool_FromLong(result);
}
//----------------------------------------------------------------------------------------------

static PyObject* ToxAV_toxav_kill(ToxCoreAV* self, PyObject* args)
{
    if (self->av != NULL) {
        toxav_kill(self->av);
        self->av = NULL;
    }

    if (self->core != NULL) {
        Py_DECREF(self->core);
        self->core = NULL;
    }

    Py_RETURN_NONE;
}
//----------------------------------------------------------------------------------------------

static PyObject* ToxAV_toxav_iteration_interval(ToxCoreAV* self, PyObject* args)
{
    CHECK_TOXAV(self);

    uint32_t interval = toxav_iteration_interval(self->av);

    return PyLong_FromUnsignedLong(interval);
}
//----------------------------------------------------------------------------------------------

static PyObject* ToxAV_toxav_iterate(ToxCoreAV* self, PyObject* args)
{
    CHECK_TOXAV(self);

    toxav_iterate(self->av);

    if (PyErr_Occurred() != NULL)
        return NULL;

    Py_RETURN_NONE;
}
//----------------------------------------------------------------------------------------------

static PyObject* ToxAV_toxav_call(ToxCoreAV* self, PyObject* args)
{
    CHECK_TOXAV(self);

    uint32_t friend_number;
    uint32_t audio_bit_rate;
    uint32_t video_bit_rate;

    if (PyArg_ParseTuple(args, "III", &friend_number, &audio_bit_rate, &video_bit_rate) == false)
        return NULL;

    TOXAV_ERR_CALL error;
    bool result = toxav_call(self->av, friend_number, audio_bit_rate, video_bit_rate, &error);

    bool success = false;
    switch (error) {
        case TOXAV_ERR_CALL_OK:
            success = true;
            break;
        case TOXAV_ERR_CALL_MALLOC:
            PyErr_SetString(ToxAVException, "A resource allocation error occurred while trying to create the structures required for the call.");
            break;
        case TOXAV_ERR_CALL_SYNC:
            PyErr_SetString(ToxAVException, "Synchronization error occurred.");
            break;
        case TOXAV_ERR_CALL_FRIEND_NOT_FOUND:
            PyErr_SetString(ToxAVException, "The friend number did not designate a valid friend.");
            break;
        case TOXAV_ERR_CALL_FRIEND_NOT_CONNECTED:
            PyErr_SetString(ToxAVException, "The friend was valid, but not currently connected.");
            break;
        case TOXAV_ERR_CALL_FRIEND_ALREADY_IN_CALL:
            PyErr_SetString(ToxAVException, "Attempted to call a friend while already in an audio or video call with them.");
            break;
        case TOXAV_ERR_CALL_INVALID_BIT_RATE:
            PyErr_SetString(ToxAVException, "Audio or video bit rate is invalid.");
            break;
    };

    if (result == false || success == false)
        return NULL;

    Py_RETURN_NONE;
}
//----------------------------------------------------------------------------------------------

static PyObject* ToxAV_toxav_answer(ToxCoreAV* self, PyObject* args)
{
    CHECK_TOXAV(self);

    uint32_t friend_number;
    uint32_t audio_bit_rate;
    uint32_t video_bit_rate;

    if (PyArg_ParseTuple(args, "III", &friend_number, &audio_bit_rate, &video_bit_rate) == false)
        return NULL;

    TOXAV_ERR_ANSWER error;
    bool result = toxav_answer(self->av, friend_number, audio_bit_rate, video_bit_rate, &error);

    bool success = false;
    switch (error) {
        case TOXAV_ERR_ANSWER_OK:
            success = true;
            break;
        case TOXAV_ERR_ANSWER_SYNC:
            PyErr_SetString(ToxAVException, "Synchronization error occurred.");
            break;
        case TOXAV_ERR_ANSWER_CODEC_INITIALIZATION:
            PyErr_SetString(ToxAVException, "Failed to initialize codecs for call session. Note that codec initiation will fail if there is no receive callback registered for either audio or video.");
            break;
        case TOXAV_ERR_ANSWER_FRIEND_NOT_FOUND:
            PyErr_SetString(ToxAVException, "The friend number did not designate a valid friend.");
            break;
        case TOXAV_ERR_ANSWER_FRIEND_NOT_CALLING:
            PyErr_SetString(ToxAVException, "The friend was valid, but they are not currently trying to initiate a call. This is also returned if this client is already in a call with the friend.");
            break;
        case TOXAV_ERR_ANSWER_INVALID_BIT_RATE:
            PyErr_SetString(ToxAVException, "Audio or video bit rate is invalid.");
            break;
    };

    if (result == false || success == false)
        return NULL;

    Py_RETURN_NONE;
}
//----------------------------------------------------------------------------------------------

#if PY_MAJOR_VERSION >= 3
struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "pytoxav",
    "Python binding for ToxAV",
    -1,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};
#endif
//----------------------------------------------------------------------------------------------

PyMethodDef ToxAV_methods[] = {
    //
    // callbacks
    //

    {
        "toxav_call_cb", (PyCFunction)ToxAV_callback_stub, METH_VARARGS,
        "toxav_call_cb(friend_number, audio_enabled, video_enabled)\n"
        "This event is triggered when a friend answer for call."
    },

    //
    // methods
    //

    {
        "toxav_version_major", (PyCFunction)ToxAV_toxav_version_major, METH_NOARGS | METH_STATIC,
        "toxav_version_major()\n"
        "Return the major version number of the library. Can be used to display the "
        "ToxAV library version or to check whether the client is compatible with the "
        "dynamically linked version of ToxAV."
    },
    {
        "toxav_version_minor", (PyCFunction)ToxAV_toxav_version_minor, METH_NOARGS | METH_STATIC,
        "toxav_version_minor()\n"
        "Return the minor version number of the library."
    },
    {
        "toxav_version_patch", (PyCFunction)ToxAV_toxav_version_patch, METH_NOARGS | METH_STATIC,
        "toxav_version_patch()\n"
        "Return the patch number of the library."
    },
    {
        "toxav_version_is_compatible", (PyCFunction)ToxAV_toxav_version_is_compatible, METH_VARARGS | METH_STATIC,
        "toxav_version_is_compatible(major, minor, patch)\n"
        "Return whether the compiled library version is compatible with the passed "
        "version numbers."
    },
    {
        "toxav_kill", (PyCFunction)ToxAV_toxav_kill, METH_NOARGS,
        "toxav_kill()\n"
        "Releases all resources associated with the A/V session.\n"
        "If any calls were ongoing, these will be forcibly terminated without "
        "notifying peers. After calling this function, no other functions may be "
        "called and the av pointer becomes invalid."
    },
    // TODO: toxav_get_tox
    {
        "toxav_iteration_interval", (PyCFunction)ToxAV_toxav_iteration_interval, METH_NOARGS,
        "toxav_iteration_interval()\n"
        "Returns the interval in milliseconds when the next toxav_iterate call should "
        "be. If no call is active at the moment, this function returns 200."
    },
    {
        "toxav_iterate", (PyCFunction)ToxAV_toxav_iterate, METH_NOARGS,
        "toxav_iterate()\n"
        "Main loop for the session. This function needs to be called in intervals of "
        "toxav_iteration_interval() milliseconds. It is best called in the separate "
        "thread from tox_iterate."
    },
    {
        "toxav_call", (PyCFunction)ToxAV_toxav_call, METH_VARARGS,
        "toxav_call(friend_number, audio_bit_rate, video_bit_rate)\n"
        "Call a friend. This will start ringing the friend.\n"
        "It is the client's responsibility to stop ringing after a certain timeout, "
        "if such behaviour is desired. If the client does not stop ringing, the "
        "library will not stop until the friend is disconnected. Audio and video "
        "receiving are both enabled by default."
    },
    {
        "toxav_answer", (PyCFunction)ToxAV_toxav_answer, METH_VARARGS,
        "toxav_answer(friend_number, audio_bit_rate, video_bit_rate)\n"
        "Accept an incoming call.\n"
        "If answering fails for any reason, the call will still be pending and it is "
        "possible to try and answer it later. Audio and video receiving are both "
        "enabled by default."
    },
    {
        NULL
    }
};
//----------------------------------------------------------------------------------------------

static int init_helper(ToxCoreAV* self, PyObject* args)
{
    ToxAV_toxav_kill(self, NULL);

    PyObject* pycore = NULL;
    if (PyArg_ParseTuple(args, "O", &pycore) == false) {
        PyErr_SetString(ToxAVException, "You must supply a ToxCore param");
        return -1;
    }

    // TODO: Check arg class name - must be instance of ToxCore
    ToxCore* core = (ToxCore*)pycore;

    TOXAV_ERR_NEW error;
    ToxAV* av = toxav_new(core->tox, &error);

    bool success = false;
    switch (error) {
        case TOXAV_ERR_NEW_OK:
            success = true;
            break;
        case TOXAV_ERR_NEW_NULL:
            PyErr_SetString(ToxAVException, "One of the arguments to the function was NULL when it was not expected.");
            break;
        case TOXAV_ERR_NEW_MALLOC:
            PyErr_SetString(ToxAVException, "Memory allocation failure while trying to allocate structures required for the A/V session.");
            break;
        case TOXAV_ERR_NEW_MULTIPLE:
            PyErr_SetString(ToxAVException, "Attempted to create a second session for the same Tox instance.");
            break;
    }

    if (av == NULL || success == false)
        return -1;

    self->av   = av;
    self->core = core;

    Py_INCREF(self->core);

    toxav_callback_call(av, callback_toxav_call, self);

    return 0;
}
//----------------------------------------------------------------------------------------------

static PyObject* ToxAV_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
    ToxCoreAV* self = (ToxCoreAV*)type->tp_alloc(type, 0);

    self->av   = NULL;
    self->core = NULL;

    // we don't care about subclass's arguments
    if (init_helper(self, NULL) == -1)
        return NULL;

    return (PyObject*)self;
}
//----------------------------------------------------------------------------------------------

static int ToxAV_init(ToxCoreAV* self, PyObject* args, PyObject* kwds)
{
    // since __init__ in Python is optional(superclass need to call it explicitly),
    // we need to initialize self->toxav in ToxAV_new instead of init.
    // If ToxAV_init is called, we re-initialize self->toxav and pass
    // the new ipv6enabled setting.
    return init_helper(self, args);
}
//----------------------------------------------------------------------------------------------

static int ToxAV_dealloc(ToxCoreAV* self)
{
    ToxAV_toxav_kill(self, NULL);

    return 0;
}
//----------------------------------------------------------------------------------------------

PyTypeObject ToxAVType = {
#if PY_MAJOR_VERSION >= 3
    PyVarObject_HEAD_INIT(NULL, 0)
#else
    PyObject_HEAD_INIT(NULL)
    0,                                          /* ob_size           */
#endif
    "ToxAV",                                    /* tp_name           */
    sizeof(ToxCoreAV),                          /* tp_basicsize      */
    0,                                          /* tp_itemsize       */
    (destructor)ToxAV_dealloc,                  /* tp_dealloc        */
    0,                                          /* tp_print          */
    0,                                          /* tp_getattr        */
    0,                                          /* tp_setattr        */
    0,                                          /* tp_compare        */
    0,                                          /* tp_repr           */
    0,                                          /* tp_as_number      */
    0,                                          /* tp_as_sequence    */
    0,                                          /* tp_as_mapping     */
    0,                                          /* tp_hash           */
    0,                                          /* tp_call           */
    0,                                          /* tp_str            */
    0,                                          /* tp_getattro       */
    0,                                          /* tp_setattro       */
    0,                                          /* tp_as_buffer      */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,   /* tp_flags          */
    "ToxAV object",                             /* tp_doc            */
    0,                                          /* tp_traverse       */
    0,                                          /* tp_clear          */
    0,                                          /* tp_richcompare    */
    0,                                          /* tp_weaklistoffset */
    0,                                          /* tp_iter           */
    0,                                          /* tp_iternext       */
    ToxAV_methods,                              /* tp_methods        */
    0,                                          /* tp_members        */
    0,                                          /* tp_getset         */
    0,                                          /* tp_base           */
    0,                                          /* tp_dict           */
    0,                                          /* tp_descr_get      */
    0,                                          /* tp_descr_set      */
    0,                                          /* tp_dictoffset     */
    (initproc)ToxAV_init,                       /* tp_init           */
    0,                                          /* tp_alloc          */
    ToxAV_new,                                  /* tp_new            */
};
//----------------------------------------------------------------------------------------------

static void ToxAV_install_dict(void)
{
#define SET(name)                                  \
    PyObject* obj_##name = PyLong_FromLong(name);  \
    PyDict_SetItemString(dict, #name, obj_##name); \
    Py_DECREF(obj_##name);

    PyObject* dict = PyDict_New();
    if (dict == NULL)
        return;

    // #define TOXAV_VERSION_MAJOR
    SET(TOXAV_VERSION_MAJOR)
    // #define TOXAV_VERSION_MINOR
    SET(TOXAV_VERSION_MINOR)
    // #define TOXAV_VERSION_PATCH
    SET(TOXAV_VERSION_PATCH)

#undef SET

    ToxAVType.tp_dict = dict;
}
//----------------------------------------------------------------------------------------------

#if PY_MAJOR_VERSION >= 3
PyMODINIT_FUNC PyInit_pytoxav(void)
{
    PyObject* module = PyModule_Create(&moduledef);
#else
PyMODINIT_FUNC initpytoxav(void)
{
    PyObject* module = Py_InitModule("pytoxav", NULL);
#endif

    if (module == NULL)
        goto error;

    ToxAV_install_dict();

    // initialize toxav
    if (PyType_Ready(&ToxAVType) < 0) {
        fprintf(stderr, "Invalid PyTypeObject 'ToxAVType'\n");
        goto error;
    }

    Py_INCREF(&ToxAVType);
    PyModule_AddObject(module, "ToxAV", (PyObject*)&ToxAVType);

    ToxAVException = PyErr_NewException("pytoxav.ToxAVException", NULL, NULL);
    PyModule_AddObject(module, "ToxAVException", (PyObject*)ToxAVException);

#if PY_MAJOR_VERSION >= 3
    return module;
#endif

error:
#if PY_MAJOR_VERSION >= 3
    return NULL;
#else
    return;
#endif
}
//----------------------------------------------------------------------------------------------
