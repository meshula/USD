//
// Copyright 2016 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
////////////////////////////////////////////////////////////////////////

#include "pxr/pxr.h"
#include "pxr/base/tf/registryManager.h"
#include "pxr/base/tf/scriptModuleLoader.h"
#include "pxr/base/tf/token.h"

#include <vector>

PXR_NAMESPACE_OPEN_SCOPE

TF_REGISTRY_FUNCTION(TfScriptModuleLoader) {
    // List of direct dependencies for this library.
    const std::vector<TfToken> reqs = {
        TfToken("arch"),
        TfToken("gf"),
        TfToken("js"),
        TfToken("kind"),
        TfToken("plug"),
        TfToken("sdf"),
        TfToken("tf"),
        TfToken("trace"),
        TfToken("usd"),
        TfToken("vt"),
        TfToken("work")
    };
    TfScriptModuleLoader::GetInstance().
        RegisterLibrary(TfToken("usdGeom"), TfToken("pxr.UsdGeom"), reqs);
}

PXR_NAMESPACE_CLOSE_SCOPE


