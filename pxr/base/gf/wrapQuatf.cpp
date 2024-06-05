//
// Copyright 2016 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
////////////////////////////////////////////////////////////////////////
// This file is generated by a script.  Do not edit directly.  Edit the
// wrapQuat.template.cpp file to make changes.

#include "pxr/pxr.h"
#include "pxr/base/gf/quatd.h"
#include "pxr/base/gf/quatf.h"
#include "pxr/base/gf/quath.h"

#include "pxr/base/tf/pyUtils.h"
#include "pxr/base/tf/wrapTypeHelpers.h"
#include "pxr/base/tf/pyContainerConversions.h"

#include <boost/python/class.hpp>
#include <boost/python/copy_const_reference.hpp>
#include <boost/python/def.hpp>
#include <boost/python/implicit.hpp>
#include <boost/python/make_constructor.hpp>
#include <boost/python/operators.hpp>
#include <boost/python/overloads.hpp>
#include <boost/python/return_arg.hpp>

#include <string>

using namespace boost::python;

using std::string;

PXR_NAMESPACE_USING_DIRECTIVE

namespace {

static string __repr__(GfQuatf const &self) {
    return TF_PY_REPR_PREFIX + "Quatf(" +
        TfPyRepr(self.GetReal()) + ", " +
        TfPyRepr(self.GetImaginary()) + ")";
}

static GfQuatf __truediv__(const GfQuatf &self, float value)
{
    return self / value;
}

static GfQuatf& __itruediv__(GfQuatf &self, float value)
{
    return self /= value;
}

static size_t __hash__(GfQuatf const &self) {
    return TfHash()(self);
}

// Zero-initialized default ctor for python.
static GfQuatf *__init__() { return new GfQuatf(0); }

} // anonymous namespace 

void wrapQuatf()
{    
    object getImaginary =
        make_function(&GfQuatf::GetImaginary,
                      return_value_policy<return_by_value>());

    object setImaginaryVec =
        make_function((void (GfQuatf::*)(const GfVec3f &))
                      &GfQuatf::SetImaginary);

    object setImaginaryScl =
        make_function((void (GfQuatf::*)(float, float, float))
                      &GfQuatf::SetImaginary,
                      default_call_policies(),
                      (arg("i"), arg("j"), arg("k")));

    def("Slerp",
        (GfQuatf (*)(double, const GfQuatf&, const GfQuatf&))
        GfSlerp);

    def("Dot",
        (float (*)(const GfQuatf&, const GfQuatf&))
        GfDot);
    
    class_<GfQuatf> cls("Quatf", no_init);
    cls
        .def("__init__", make_constructor(__init__))
                          
        .def(TfTypePythonClass())

        .def(init<GfQuatf>())
        .def(init<float>(arg("real")))
        .def(init<float, const GfVec3f &>(
                 (arg("real"), arg("imaginary"))))
        .def(init<float, float, float, float>(
                 (arg("real"), arg("i"), arg("j"), arg("k"))))
        .def(init<const GfQuatd & >())

        .def("GetZero", &GfQuatf::GetZero)
        .staticmethod("GetZero")

        .def("GetIdentity", &GfQuatf::GetIdentity)
        .staticmethod("GetIdentity")

        .def("GetReal", &GfQuatf::GetReal)
        .def("SetReal", &GfQuatf::SetReal)
        .add_property("real", &GfQuatf::GetReal, &GfQuatf::SetReal)

        .def("GetImaginary", getImaginary)
        .def("SetImaginary", setImaginaryVec)
        .def("SetImaginary", setImaginaryScl)
        .add_property("imaginary", getImaginary, setImaginaryVec)

        .def("GetLength", &GfQuatf::GetLength)

        .def("GetNormalized", &GfQuatf::GetNormalized,
             (arg("eps")=GF_MIN_VECTOR_LENGTH))
        .def("Normalize", &GfQuatf::Normalize,
             (arg("eps")=GF_MIN_VECTOR_LENGTH), return_self<>())

        .def("GetConjugate", &GfQuatf::GetConjugate)
        .def("GetInverse", &GfQuatf::GetInverse)

        .def("Transform", &GfQuatf::Transform)

        .def(str(self))
        .def(-self)
        .def(self == self)
        .def(self != self)
        .def(self *= self)
        .def(self *= float())
        .def(self /= float())
        .def(self += self)
        .def(self -= self)
        .def(self + self)
        .def(self - self)
        .def(self * self)
        .def(self * float())
        .def(float() * self)
        .def(self / float())
        .def("__repr__", __repr__)
        .def("__hash__", __hash__)
        ;

    implicitly_convertible<GfQuath, GfQuatf>();

    to_python_converter<std::vector<GfQuatf>,
        TfPySequenceToPython<std::vector<GfQuatf> > >();

    if (!PyObject_HasAttrString(cls.ptr(), "__truediv__")) {
        // __truediv__ not added by .def( self / float() ) above, which
        // happens when building with python 2, but we need it to support
        // "from __future__ import division"
        cls.def("__truediv__", __truediv__);
    }
    if (!PyObject_HasAttrString(cls.ptr(), "__itruediv__")) {
        // __itruediv__ not added by .def( self /= float() ) above, which
        // happens when building with python 2, but we need it to support
        // "from __future__ import division". This is also a workaround for a 
        // bug in the current version of boost::python that incorrectly wraps
        // in-place division with __idiv__ when building with python 3.
        cls.def("__itruediv__", __itruediv__, return_self<>());
    }
}
