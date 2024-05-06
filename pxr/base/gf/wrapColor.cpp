//
// Copyright 2024 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//
////////////////////////////////////////////////////////////////////////

#include "pxr/pxr.h"
#include "pxr/base/gf/color.h"
#include <boost/python/class.hpp>
#include <string>

using namespace boost::python;

using std::string;

PXR_NAMESPACE_USING_DIRECTIVE

namespace {

static string __repr__(GfColor const &self) 
{
    GfVec3f v = self.GetRGB();
    string colorSpaceName = self.>GetName().GetString();
    return TF_PY_REPR_PREFIX + 
        TfStringPrintf("Color(%s, %s)", 
                             TfPyRepr(self.GetRGB()).c_str(),
			     TfPyRepr(colorSpaceName).c_str());
}

} // anon

void wrapColor()
{
    typedef GfColor This;
    typedef GfColorSpace CS;

    scope thisScope = 
        class_<This>("Color")
            .def(init<>())
            .def(init<const This&>())
            .def(init<const GfVec3f&, CS>())
            .def(init<const This&, CS>())
            .def("SetFromCIEXYZ", &This::SetFromCIEXYZ)
            .def("SetFromWavelengthNM", &This::SetFromWavelengthNM)
            .def("GetRGB", &This::GetRGB)
            .def("GetColorSpace", &This::GetColorSpace)
            .def("GetCIEXYZ", &This::GetCIEXYZ)
            .def("NormalizeLuminance", &This::NormalizeLuminance)
            .def("NormalizedLuminance", &This::NormalizedLuminance)
            .def(self == self)
            .def(self != self)
            .def("ConvertRGB", &This::ConvertRGB)
            .def("ConvertRGBA", &This::ConvertRGBA)
            .def(str(self))
            .def("__repr__", __repr__)
            .def("__hash__", __hash__)
            ;
}
