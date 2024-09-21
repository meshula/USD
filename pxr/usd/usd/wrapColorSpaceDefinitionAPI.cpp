//
// Copyright 2016 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
#include "pxr/usd/usd/colorSpaceDefinitionAPI.h"
#include "pxr/usd/usd/schemaBase.h"

#include "pxr/usd/sdf/primSpec.h"

#include "pxr/usd/usd/pyConversions.h"
#include "pxr/base/tf/pyAnnotatedBoolResult.h"
#include "pxr/base/tf/pyContainerConversions.h"
#include "pxr/base/tf/pyResultConversions.h"
#include "pxr/base/tf/pyUtils.h"
#include "pxr/base/tf/wrapTypeHelpers.h"

#include "pxr/external/boost/python.hpp"

#include <string>

PXR_NAMESPACE_USING_DIRECTIVE

using namespace pxr_boost::python;

namespace {

#define WRAP_CUSTOM                                                     \
    template <class Cls> static void _CustomWrapCode(Cls &_class)

// fwd decl.
WRAP_CUSTOM;

        
static UsdAttribute
_CreateColorSpaceDefinitionNameAttr(UsdColorSpaceDefinitionAPI &self,
                                      object defaultVal, bool writeSparsely) {
    return self.CreateColorSpaceDefinitionNameAttr(
        UsdPythonToSdfType(defaultVal, SdfValueTypeNames->Token), writeSparsely);
}
        
static UsdAttribute
_CreateColorSpaceRedChromaAttr(UsdColorSpaceDefinitionAPI &self,
                                      object defaultVal, bool writeSparsely) {
    return self.CreateColorSpaceRedChromaAttr(
        UsdPythonToSdfType(defaultVal, SdfValueTypeNames->Float2), writeSparsely);
}
        
static UsdAttribute
_CreateColorSpaceGreenChromaAttr(UsdColorSpaceDefinitionAPI &self,
                                      object defaultVal, bool writeSparsely) {
    return self.CreateColorSpaceGreenChromaAttr(
        UsdPythonToSdfType(defaultVal, SdfValueTypeNames->Float2), writeSparsely);
}
        
static UsdAttribute
_CreateColorSpaceBlueChromaAttr(UsdColorSpaceDefinitionAPI &self,
                                      object defaultVal, bool writeSparsely) {
    return self.CreateColorSpaceBlueChromaAttr(
        UsdPythonToSdfType(defaultVal, SdfValueTypeNames->Float2), writeSparsely);
}
        
static UsdAttribute
_CreateColorSpaceWhitePointAttr(UsdColorSpaceDefinitionAPI &self,
                                      object defaultVal, bool writeSparsely) {
    return self.CreateColorSpaceWhitePointAttr(
        UsdPythonToSdfType(defaultVal, SdfValueTypeNames->Float2), writeSparsely);
}
        
static UsdAttribute
_CreateColorSpaceGammaAttr(UsdColorSpaceDefinitionAPI &self,
                                      object defaultVal, bool writeSparsely) {
    return self.CreateColorSpaceGammaAttr(
        UsdPythonToSdfType(defaultVal, SdfValueTypeNames->Float), writeSparsely);
}
        
static UsdAttribute
_CreateColorSpaceLinearBiasAttr(UsdColorSpaceDefinitionAPI &self,
                                      object defaultVal, bool writeSparsely) {
    return self.CreateColorSpaceLinearBiasAttr(
        UsdPythonToSdfType(defaultVal, SdfValueTypeNames->Float), writeSparsely);
}

static std::string
_Repr(const UsdColorSpaceDefinitionAPI &self)
{
    std::string primRepr = TfPyRepr(self.GetPrim());
    return TfStringPrintf(
        "Usd.ColorSpaceDefinitionAPI(%s)",
        primRepr.c_str());
}

struct UsdColorSpaceDefinitionAPI_CanApplyResult : 
    public TfPyAnnotatedBoolResult<std::string>
{
    UsdColorSpaceDefinitionAPI_CanApplyResult(bool val, std::string const &msg) :
        TfPyAnnotatedBoolResult<std::string>(val, msg) {}
};

static UsdColorSpaceDefinitionAPI_CanApplyResult
_WrapCanApply(const UsdPrim& prim)
{
    std::string whyNot;
    bool result = UsdColorSpaceDefinitionAPI::CanApply(prim, &whyNot);
    return UsdColorSpaceDefinitionAPI_CanApplyResult(result, whyNot);
}

} // anonymous namespace

void wrapUsdColorSpaceDefinitionAPI()
{
    typedef UsdColorSpaceDefinitionAPI This;

    UsdColorSpaceDefinitionAPI_CanApplyResult::Wrap<UsdColorSpaceDefinitionAPI_CanApplyResult>(
        "_CanApplyResult", "whyNot");

    class_<This, bases<UsdAPISchemaBase> >
        cls("ColorSpaceDefinitionAPI");

    cls
        .def(init<UsdPrim>(arg("prim")))
        .def(init<UsdSchemaBase const&>(arg("schemaObj")))
        .def(TfTypePythonClass())

        .def("Get", &This::Get, (arg("stage"), arg("path")))
        .staticmethod("Get")

        .def("CanApply", &_WrapCanApply, (arg("prim")))
        .staticmethod("CanApply")

        .def("Apply", &This::Apply, (arg("prim")))
        .staticmethod("Apply")

        .def("GetSchemaAttributeNames",
             &This::GetSchemaAttributeNames,
             arg("includeInherited")=true,
             return_value_policy<TfPySequenceToList>())
        .staticmethod("GetSchemaAttributeNames")

        .def("_GetStaticTfType", (TfType const &(*)()) TfType::Find<This>,
             return_value_policy<return_by_value>())
        .staticmethod("_GetStaticTfType")

        .def(!self)

        
        .def("GetColorSpaceDefinitionNameAttr",
             &This::GetColorSpaceDefinitionNameAttr)
        .def("CreateColorSpaceDefinitionNameAttr",
             &_CreateColorSpaceDefinitionNameAttr,
             (arg("defaultValue")=object(),
              arg("writeSparsely")=false))
        
        .def("GetColorSpaceRedChromaAttr",
             &This::GetColorSpaceRedChromaAttr)
        .def("CreateColorSpaceRedChromaAttr",
             &_CreateColorSpaceRedChromaAttr,
             (arg("defaultValue")=object(),
              arg("writeSparsely")=false))
        
        .def("GetColorSpaceGreenChromaAttr",
             &This::GetColorSpaceGreenChromaAttr)
        .def("CreateColorSpaceGreenChromaAttr",
             &_CreateColorSpaceGreenChromaAttr,
             (arg("defaultValue")=object(),
              arg("writeSparsely")=false))
        
        .def("GetColorSpaceBlueChromaAttr",
             &This::GetColorSpaceBlueChromaAttr)
        .def("CreateColorSpaceBlueChromaAttr",
             &_CreateColorSpaceBlueChromaAttr,
             (arg("defaultValue")=object(),
              arg("writeSparsely")=false))
        
        .def("GetColorSpaceWhitePointAttr",
             &This::GetColorSpaceWhitePointAttr)
        .def("CreateColorSpaceWhitePointAttr",
             &_CreateColorSpaceWhitePointAttr,
             (arg("defaultValue")=object(),
              arg("writeSparsely")=false))
        
        .def("GetColorSpaceGammaAttr",
             &This::GetColorSpaceGammaAttr)
        .def("CreateColorSpaceGammaAttr",
             &_CreateColorSpaceGammaAttr,
             (arg("defaultValue")=object(),
              arg("writeSparsely")=false))
        
        .def("GetColorSpaceLinearBiasAttr",
             &This::GetColorSpaceLinearBiasAttr)
        .def("CreateColorSpaceLinearBiasAttr",
             &_CreateColorSpaceLinearBiasAttr,
             (arg("defaultValue")=object(),
              arg("writeSparsely")=false))

        .def("__repr__", ::_Repr)
    ;

    _CustomWrapCode(cls);
}

// ===================================================================== //
// Feel free to add custom code below this line, it will be preserved by 
// the code generator.  The entry point for your custom code should look
// minimally like the following:
//
// WRAP_CUSTOM {
//     _class
//         .def("MyCustomMethod", ...)
//     ;
// }
//
// Of course any other ancillary or support code may be provided.
// 
// Just remember to wrap code in the appropriate delimiters:
// 'namespace {', '}'.
//
// ===================================================================== //
// --(BEGIN CUSTOM CODE)--

namespace {

WRAP_CUSTOM {
}

}
