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

#include "pxr/pxr.h"
#include "pxr/base/gf/color.h"
#include "pxr/base/gf/vec3f.h"
#include "pxr/base/gf/colorSpace.h"
#include "pxr/base/tf/pyUtils.h"
#include "pxr/base/tf/stringUtils.h"
#include "pxr/base/tf/wrapTypeHelpers.h"
#include <boost/python/class.hpp>
#include <boost/python/def.hpp>
#include <boost/python/operators.hpp>
#include <string>

using namespace boost::python;

PXR_NAMESPACE_USING_DIRECTIVE

namespace {

std::string __repr__(GfColor const &self)
{
    std::string colorSpaceName = self.GetColorSpace().GetName().GetString();
    return TF_PY_REPR_PREFIX + 
        TfStringPrintf("Color(%s, %s)", 
                             TfPyRepr(self.GetRGB()).c_str(),
			     TfPyRepr(colorSpaceName).c_str());
}

}

void wrapColor()
{
    class_<GfColor>("GfColor")
        .def(init<>())
        .def(init<const GfColorSpace&>())
        .def(init<const GfVec3f&, const GfColorSpace&>())
        .def(init<const GfColor&, const GfColorSpace&>())
        .def("__repr__", &__repr__)
        .def("SetFromXY", &GfColor::SetFromChromaticity)
        .def("SetFromXYZ", &GfColor::SetFromXYZ)
        .def("SetFromBlackbodyKelvin", &GfColor::SetFromBlackbodyKelvin)
        .def("GetRGB", &GfColor::GetRGB)
        .def("GetColorSpace", &GfColor::GetColorSpace)
        .def("GetXYZ", &GfColor::GetXYZ)
        .def("GetChromaticity", &GfColor::GetChromaticity)
        .def(self == self)
        .def(self != self);

        def("IsClose", (bool (*)(const GfColor &v1, const GfColor &v2, double))
            GfIsClose);
}
