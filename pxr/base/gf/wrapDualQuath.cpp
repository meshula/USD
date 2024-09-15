//
// Copyright 2021 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
////////////////////////////////////////////////////////////////////////
// This file is generated by a script.  Do not edit directly.  Edit the
// wrapDualQuat.template.cpp file to make changes.

#include "pxr/pxr.h"
#include "pxr/base/gf/dualQuatd.h"
#include "pxr/base/gf/dualQuatf.h"
#include "pxr/base/gf/dualQuath.h"

#include "pxr/base/tf/hash.h"
#include "pxr/base/tf/pyUtils.h"
#include "pxr/base/tf/wrapTypeHelpers.h"
#include "pxr/base/tf/pyContainerConversions.h"

#include "pxr/external/boost/python/class.hpp"
#include "pxr/external/boost/python/copy_const_reference.hpp"
#include "pxr/external/boost/python/def.hpp"
#include "pxr/external/boost/python/implicit.hpp"
#include "pxr/external/boost/python/make_constructor.hpp"
#include "pxr/external/boost/python/operators.hpp"
#include "pxr/external/boost/python/overloads.hpp"
#include "pxr/external/boost/python/return_arg.hpp"

#include <string>

using std::string;

PXR_NAMESPACE_USING_DIRECTIVE

using namespace pxr_boost::python;

namespace {

static string __repr__(GfDualQuath const &self) {
    return TF_PY_REPR_PREFIX + "DualQuath(" +
        TfPyRepr(self.GetReal()) + ", " +
        TfPyRepr(self.GetDual()) + ")";
}

static GfDualQuath __truediv__(const GfDualQuath &self, GfHalf value)
{
    return self / value;
}

static GfDualQuath& __itruediv__(GfDualQuath &self, GfHalf value)
{
    return self /= value;
}

static size_t __hash__(GfDualQuath const &self) {
    return TfHash{}(self);
}

// Zero-initialized default ctor for python.
static GfDualQuath *__init__() {
    return new GfDualQuath(GfQuath(0), GfQuath(0));
}

} // anonymous namespace

void wrapDualQuath()
{
    object getReal =
        make_function(&GfDualQuath::GetReal,
                      return_value_policy<return_by_value>());
    object setReal =
        make_function((void (GfDualQuath::*)(const GfQuath &))
                      &GfDualQuath::SetReal);

    object getDual =
        make_function(&GfDualQuath::GetDual,
                      return_value_policy<return_by_value>());
    object setDual =
        make_function((void (GfDualQuath::*)(const GfQuath &))
                      &GfDualQuath::SetDual);

    def( "Dot",
         (GfHalf (*)(const GfDualQuath &, const GfDualQuath &))
         GfDot);

    class_<GfDualQuath> cls("DualQuath", no_init);
    cls
        .def("__init__", make_constructor(__init__))

        .def( TfTypePythonClass() )

        .def(init<GfDualQuath>())
        .def(init<GfHalf>(arg("realVal")))
        .def(init< const GfQuath & >(arg("real")))
        .def(init< const GfQuath &, const GfQuath & >(
                 (arg("real"), arg("dual"))))
        .def(init< const GfQuath &, const GfVec3h & >(
                 (arg("rotation"), arg("translation"))))
        .def(init<const GfDualQuatd & >())
        .def(init<const GfDualQuatf & >())

        .def("GetZero", &GfDualQuath::GetZero)
        .staticmethod("GetZero")

        .def("GetIdentity", &GfDualQuath::GetIdentity)
        .staticmethod("GetIdentity")

        .def("GetReal", getReal)
        .def("SetReal", setReal)
        .add_property("real", getReal, setReal)

        .def("GetDual", getDual)
        .def("SetDual", setDual)
        .add_property("dual", getDual, setDual)

        .def("GetLength", &GfDualQuath::GetLength)

        .def("GetNormalized", &GfDualQuath::GetNormalized,
             (arg("eps")=GF_MIN_VECTOR_LENGTH))
        .def("Normalize", &GfDualQuath::Normalize,
             (arg("eps")=GF_MIN_VECTOR_LENGTH), return_self<>())

        .def("GetConjugate", &GfDualQuath::GetConjugate)
        .def("GetInverse", &GfDualQuath::GetInverse)
        .def("SetTranslation",
             (void (GfDualQuath::*)(const GfVec3h &))
             &GfDualQuath::SetTranslation)
        .def("GetTranslation", &GfDualQuath::GetTranslation)

        .def("Transform",
             (GfVec3h (GfDualQuath::*)(const GfVec3h &) const)
             &GfDualQuath::Transform)

        .def( str(self) )
        .def( self == self )
        .def( self != self )
        .def( self += self )
        .def( self -= self )
        .def( self *= self )
        .def( self *= GfHalf() )
        .def( self /= GfHalf() )
        .def( self + self )
        .def( self - self )
        .def( self * self )
        .def( self * GfHalf() )
        .def( GfHalf() * self )
        .def( self / GfHalf() )

        .def("__repr__", __repr__)
        .def("__hash__", __hash__)
        ;


    to_python_converter<std::vector<GfDualQuath>,
        TfPySequenceToPython<std::vector<GfDualQuath> > >();

    if (!PyObject_HasAttrString(cls.ptr(), "__truediv__")) {
        // __truediv__ not added by .def( self / GfHalf() ) above, which
        // happens when building with python 2, but we need it to support
        // "from __future__ import division"
        cls.def("__truediv__", __truediv__);
    }
    if (!PyObject_HasAttrString(cls.ptr(), "__itruediv__")) {
        // __itruediv__ not added by .def( self /= GfHalf() ) above, which
        // happens when building with python 2, but we need it to support
        // "from __future__ import division". This is also a workaround for a 
        // bug in the current version of pxr_boost::python that incorrectly wraps
        // in-place division with __idiv__ when building with python 3.
        cls.def("__itruediv__", __itruediv__, return_self<>());
    }
}
