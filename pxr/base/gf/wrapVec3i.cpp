//
// Copyright 2016 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
////////////////////////////////////////////////////////////////////////
// This file is generated by a script.  Do not edit directly.  Edit the
// wrapVec.template.cpp file to make changes.

#include "pxr/pxr.h"
#include "pxr/base/gf/vec3i.h"

#include "pxr/base/gf/pyBufferUtils.h"

#include "pxr/base/tf/hash.h"
#include "pxr/base/tf/pyContainerConversions.h"
#include "pxr/base/tf/pyUtils.h"
#include "pxr/base/tf/stringUtils.h"
#include "pxr/base/tf/wrapTypeHelpers.h"


#include <boost/python/class.hpp>
#include <boost/python/def.hpp>
#include <boost/python/make_constructor.hpp>
#include <boost/python/operators.hpp>
#include <boost/python/overloads.hpp>
#include <boost/python/return_arg.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/slice.hpp>

#include <string>

using namespace boost::python;

using std::string;

PXR_NAMESPACE_USING_DIRECTIVE

namespace {

////////////////////////////////////////////////////////////////////////
// Python buffer protocol support.

// Python's getbuffer interface function.
static int
getbuffer(PyObject *self, Py_buffer *view, int flags) {
    if (view == NULL) {
        PyErr_SetString(PyExc_ValueError, "NULL view in getbuffer");
        return -1;
    }

    // We don't support fortran order.
    if ((flags & PyBUF_F_CONTIGUOUS) == PyBUF_F_CONTIGUOUS) {
        PyErr_SetString(PyExc_ValueError, "Fortran contiguity unsupported");
        return -1;
    }

    GfVec3i &vec = extract<GfVec3i &>(self);

    view->obj = self;
    view->buf = static_cast<void *>(vec.data());
    view->len = sizeof(GfVec3i);
    view->readonly = 0;
    view->itemsize = sizeof(int);
    if ((flags & PyBUF_FORMAT) == PyBUF_FORMAT) {
        view->format = Gf_GetPyBufferFmtFor<int>();
    } else {
        view->format = NULL;
    }
    if ((flags & PyBUF_ND) == PyBUF_ND) {
        view->ndim = 1;
        static Py_ssize_t shape = 3;
        view->shape = &shape;
    } else {
        view->ndim = 0;
        view->shape = NULL;
    }
    if ((flags & PyBUF_STRIDES) == PyBUF_STRIDES) {
        static Py_ssize_t strides = sizeof(int);
        view->strides = &strides;
    } else {
        view->strides = NULL;
    }
    view->suboffsets = NULL;
    view->internal = NULL;

    Py_INCREF(self); // need to retain a reference to self.
    return 0;
}

// This structure serves to instantiate a PyBufferProcs instance with pointers
// to the right buffer protocol functions.
static PyBufferProcs bufferProcs = {
    (getbufferproc) getbuffer,
    (releasebufferproc) 0,
};

// End python buffer protocol support.
////////////////////////////////////////////////////////////////////////


static string __repr__(GfVec3i const &self) {
    string elems;
    for (size_t i = 0; i < self.dimension; ++i) 
        elems += (i ? ", " : "") + TfPyRepr(self[i]);

    return TF_PY_REPR_PREFIX + "Vec3i(" + elems + ")";
}

static size_t __hash__(GfVec3i const &self) {
    return TfHash{}(self);
}

 

 
static int
normalizeIndex(int index) {
    return TfPyNormalizeIndex(index, 3, true /*throw error*/);
}

static int __len__(const GfVec3i &self) { return 3; }

// Implements __getitem__ for a single index
static int __getitem__(const GfVec3i &self, int index) {
    return self[normalizeIndex(index)];
}

// Implements __getitem__ for a slice
static list __getslice__(const GfVec3i &self, slice indices) {
    list result;

    const int* begin = self.data();
    const int* end = begin + 3;

    slice::range<const int*> bounds;
    try {
        // This appears to be a typo in the boost headers.  The method
        // name should be "get_indices".
        //
        bounds = indices.get_indicies<>(begin, end);
    } catch (std::invalid_argument const &) {
        return result;
    }

    while (bounds.start != bounds.stop) {
        result.append(*bounds.start);
        bounds.start += bounds.step;
    }
    // Unlike STL ranges, bounds represents a *closed* interval.  This
    // means that we must append exactly one more item at the end of
    // the list.
    //
    result.append(*bounds.start);

    return result;
}

static void __setitem__(GfVec3i &self, int index, int value) {
    self[normalizeIndex(index)] = value;
}

// Handles refcounting & extraction for PySequence_GetItem.
static int _SequenceGetItem(PyObject *seq, Py_ssize_t i) {
    boost::python::handle<> h(PySequence_GetItem(seq, i));
    return extract<int>(boost::python::object(h));
}

static bool _SequenceCheckItem(PyObject *seq, Py_ssize_t i) {
    boost::python::handle<> h(PySequence_GetItem(seq, i));
    extract<int> e((boost::python::object(h)));
    return e.check();
}

static void __setslice__(GfVec3i &self, slice indices, object values) {
    // Verify our arguments
    //
    PyObject* valuesObj = values.ptr();

    if (!PySequence_Check(valuesObj)) {
        TfPyThrowTypeError("value must be a sequence");
    }

    int* begin = self.data();
    int* end = begin + 3;

    Py_ssize_t sliceLength = -1;

    slice::range<int*> bounds;

    // Convince g++ that we're not using uninitialized values.
    //
    bounds.start = 0;
    bounds.stop  = 0;
    bounds.step  = 0;

    try {
        // This appears to be a typo in the boost headers.  The method
        // name should be "get_indices".
        //
        bounds = indices.get_indicies<>(begin, end);
    } catch (std::invalid_argument const &) {
        sliceLength = 0;
    }

    // If sliceLength was not set in the exception handling code above,
    // figure out how long it really is.
    //
    if (sliceLength == -1) {
        sliceLength = ((bounds.stop - bounds.start) / bounds.step) + 1;
    }

    if (PySequence_Length(valuesObj) != sliceLength) {
        TfPyThrowValueError(
            TfStringPrintf(
                "attempt to assign sequence of size %zd to slice of size %zd",
                PySequence_Length(valuesObj), sliceLength));
    }

    // Short circuit for empty slices
    //
    if (sliceLength == 0) {
        return;
    }

    // Make sure that all items can be extracted before changing the GfVec3i.
    //
    for (Py_ssize_t i = 0; i < sliceLength; ++i) {
        // This will throw a TypeError if any of the items cannot be converted.
        _SequenceGetItem(valuesObj, i);
    }

    for (Py_ssize_t i = 0; i < sliceLength; ++i) {
        *bounds.start = _SequenceGetItem(valuesObj, i);
        bounds.start += bounds.step;
    }
}

static bool __contains__(const GfVec3i &self, int value) {
    for (size_t i = 0; i < 3; ++i) {
        if (self[i] == value)
            return true;
    }
    return false;
}

static GfVec3i __truediv__(const GfVec3i &self, int value)
{
    return self / value;
}

static GfVec3i& __itruediv__(GfVec3i &self, int value)
{
    return self /= value;
}

template <class V>
static V *__init__() {
    // Default contstructor zero-initializes from python.
    return new V(0);
}

struct FromPythonTuple {
    FromPythonTuple() {
        converter::registry::
            push_back(&_convertible, &_construct,
                      boost::python::type_id<GfVec3i>());
    }

  private:
    
    static void *_convertible(PyObject *obj_ptr) {
        // If this object is a GfVec already, disregard.
        if (PyObject_HasAttrString(obj_ptr, "__isGfVec"))
            return 0;

        typedef int Scalar;

        // XXX: Would like to allow general sequences, but currently clients
        // depend on this behavior.
        if ((PyTuple_Check(obj_ptr) || PyList_Check(obj_ptr)) &&
            PySequence_Size(obj_ptr) == 3 &&
            _SequenceCheckItem(obj_ptr, 0) &&
            _SequenceCheckItem(obj_ptr, 1) &&
            _SequenceCheckItem(obj_ptr, 2)) {
            return obj_ptr;
        }
        return 0;
    }

    static void _construct(PyObject *obj_ptr, converter::
                           rvalue_from_python_stage1_data *data) {
        typedef int Scalar;
        void *storage = ((converter::rvalue_from_python_storage<GfVec3i>*)data)
	    ->storage.bytes;
        new (storage)
	    GfVec3i(
                _SequenceGetItem(obj_ptr, 0),
                _SequenceGetItem(obj_ptr, 1),
                _SequenceGetItem(obj_ptr, 2));
        data->convertible = storage;
    }
};

// This adds support for python's builtin pickling library
// This is used by our Shake plugins which need to pickle entire classes
// (including code), which we don't support in pxml.
struct PickleSuite : boost::python::pickle_suite
{
    static boost::python::tuple getinitargs(const GfVec3i &v) {
        return boost::python::make_tuple(v[0], v[1], v[2]);
    }
};

} // anonymous namespace 

void wrapVec3i()
{
    typedef GfVec3i Vec;
    typedef int Scalar;

    static const size_t _dimension = 3;
    static const bool _true = true;
    
    def("Dot", (Scalar (*)( const Vec &, const Vec &))GfDot);
    
 
    class_<GfVec3i> cls("Vec3i", no_init);
    cls
        .def("__init__", make_constructor(__init__<Vec>))

        // A tag indicating that this is a GfVec class, for internal use.
        .def_readonly("__isGfVec", _true)

        .def_pickle(PickleSuite())

        
        .def(init<Vec>())
        .def(init<Scalar>())
        .def(init<Scalar, Scalar, Scalar>())

        .def(TfTypePythonClass())

        .def("__len__", __len__ )
        .def("__getitem__", __getitem__ )
        .def("__getitem__", __getslice__ )
        .def("__setitem__", __setitem__ )
        .def("__setitem__", __setslice__ )
        .def("__contains__", __contains__ )

        .def_readonly("dimension", _dimension)
        

        .def(self == self)
        .def(self != self)
        .def(self += self)
        .def(self -= self)
        .def(self *= double())
        .def(self * double())
        .def(double() * self)
        .def(self /= int())
        .def(self / int())
        .def(-self)
        .def(self + self)
        .def(self - self)
        .def(self * self)
        .def(str(self))

        .def("Axis", &Vec::Axis).staticmethod("Axis")

        .def("XAxis", &Vec::XAxis).staticmethod("XAxis")
        .def("YAxis", &Vec::YAxis).staticmethod("YAxis")
        .def("ZAxis", &Vec::ZAxis).staticmethod("ZAxis")

        .def("GetDot", (Scalar (*)(const Vec &, const Vec &))GfDot)
        

        .def("__repr__", __repr__)
        .def("__hash__", __hash__)
        ;
    to_python_converter<std::vector<GfVec3i>,
        TfPySequenceToPython<std::vector<GfVec3i> > >();

    // Install buffer protocol: set the tp_as_buffer slot to point to a
    // structure of function pointers that implement the buffer protocol for
    // this type.
    auto *typeObj = reinterpret_cast<PyTypeObject *>(cls.ptr());
    typeObj->tp_as_buffer = &bufferProcs;

    // Allow appropriate tuples to be passed where Vecs are expected.
    FromPythonTuple();

    // Allow conversion of lists of GfVec3i to std::vector<GfVec3i>
    TfPyContainerConversions::from_python_sequence<
        std::vector<GfVec3i>,
        TfPyContainerConversions::variable_capacity_policy >();

    if (!PyObject_HasAttrString(cls.ptr(), "__truediv__")) {
        // __truediv__ not added by .def( self / double() ) above, which
        // happens when building with python 2, but we need it to support
        // "from __future__ import division"
        cls.def("__truediv__", __truediv__);
    }
    if (!PyObject_HasAttrString(cls.ptr(), "__itruediv__")) {
        // __itruediv__ not added by .def( self /= double() ) above, which
        // happens when building with python 2, but we need it to support
        // "from __future__ import division". This is also a workaround for a 
        // bug in the current version of boost::python that incorrectly wraps
        // in-place division with __idiv__ when building with python 3.
        cls.def("__itruediv__", __itruediv__, return_self<>{});
    }
}
