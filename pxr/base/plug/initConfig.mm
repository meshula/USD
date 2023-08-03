//
// Copyright 2023 Pixar
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
#include "pxr/base/plug/info.h"
#include "pxr/base/tf/diagnosticLite.h"
#include "pxr/base/tf/stringUtils.h"

#import <Foundation/Foundation.h>

PXR_NAMESPACE_OPEN_SCOPE

namespace {

ARCH_CONSTRUCTOR(Plug_InitConfig, 2, void)
{
    std::vector<std::string> result;
    NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
    NSString *usdPath = [resourcePath stringByAppendingPathComponent:@"usd"];
    NSString *otherPath = [resourcePath stringByAppendingPathComponent:@"plugin/usd"];
    std::vector<std::string> test;
    std::vector<std::string> paths;
    paths.emplace_back([usdPath UTF8String]);
    paths.emplace_back([otherPath UTF8String]);
    std::vector<std::string> debugMessages;
    debugMessages.emplace_back(
        TfStringPrintf(
            "Plug will search for plug infos in app bundle: '%s'\n",
            [resourcePath UTF8String]));
    Plug_SetPaths(paths, msgs, true);
}

} // anon

PXR_NAMESPACE_CLOSE_SCOPE
