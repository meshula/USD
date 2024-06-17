//
// Copyright 2016 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
#ifndef PXR_USD_PCP_COMPOSE_SITE_H
#define PXR_USD_PCP_COMPOSE_SITE_H

/// \file pcp/composeSite.h
///
/// Single-site composition.
///
/// These are helpers that compose specific fields at single sites.
/// They compose the field for a given path across a layer stack,
/// using field-specific rules to combine the values.
/// 
/// These helpers are low-level utilities used by the rest of the
/// Pcp algorithms, to discover composition arcs in scene description.
/// These arcs are what guide the algorithm to pull additional
/// sites of scene description into the PcpPrimIndex.
///
/// Some of these field types support list-editing.  (See SdListOp.)
/// List-editing for these fields is applied across the fixed domain
/// of a single site; you cannot apply list-ops across sites.
/// The intention is to avoid subtle ordering issues in composition
/// semantics.
///
/// Note that these helpers do not take PcpSite as a literal parameter;
/// instead, they require the actual computed layer stack that a site
/// identified.  Rather than tying these helpers to PcpCache and its
/// process of computing layer stacks, they just employ the result.
/// Conceptually, though, they are operating on the scene description
/// identified by a PcpSite.

#include "pxr/pxr.h"
#include "pxr/usd/pcp/api.h"
#include "pxr/usd/pcp/errors.h"
#include "pxr/usd/pcp/node.h"
#include "pxr/usd/sdf/layerOffset.h"
#include "pxr/usd/sdf/path.h"
#include "pxr/usd/sdf/payload.h"
#include "pxr/usd/sdf/reference.h"
#include "pxr/usd/sdf/types.h"
#include "pxr/usd/sdf/site.h"

#include <set>
#include <string>
#include <unordered_set>
#include <vector>

PXR_NAMESPACE_OPEN_SCOPE

TF_DECLARE_WEAK_AND_REF_PTRS(PcpLayerStack);

class PcpLayerStackSite;

/// \struct PcpArcInfo
///
/// Helper information about an arc. All arcs have a 
/// layer that the arc comes from. References and payloads 
/// supply an authored asset path as well.
///
struct PcpArcInfo {
    SdfLayerHandle sourceLayer;
    SdfLayerOffset sourceLayerStackOffset;
    std::string authoredAssetPath;
    int arcNum;
};

/// A vector of reference or payload arc information.
typedef std::vector<PcpArcInfo> PcpArcInfoVector;

/// References
PCP_API
void
PcpComposeSiteReferences(
    PcpLayerStackRefPtr const &layerStack,
    SdfPath const &path,
    SdfReferenceVector *result,
    PcpArcInfoVector *info,
    std::unordered_set<std::string> *exprVarDependencies,
    PcpErrorVector *errors);

inline void
PcpComposeSiteReferences(
    PcpLayerStackRefPtr const &layerStack,
    SdfPath const &path,
    SdfReferenceVector *result,
    PcpArcInfoVector *info)
{
    return PcpComposeSiteReferences(
        layerStack, path, result, info, nullptr , nullptr);
}

inline void
PcpComposeSiteReferences(
    PcpNodeRef const &node,
    SdfReferenceVector *result,
    PcpArcInfoVector *info,
    std::unordered_set<std::string> *exprVarDependencies,
    PcpErrorVector *errors)
{
    return PcpComposeSiteReferences(
        node.GetLayerStack(), node.GetPath(), 
        result, info, exprVarDependencies, errors);
}

inline void
PcpComposeSiteReferences(
    PcpNodeRef const &node,
    SdfReferenceVector *result,
    PcpArcInfoVector *info)
{
    return PcpComposeSiteReferences(
        node.GetLayerStack(), node.GetPath(), result, info, nullptr, nullptr);
}

/// Payloads
PCP_API
void
PcpComposeSitePayloads(
    PcpLayerStackRefPtr const &layerStack,
    SdfPath const &path,
    SdfPayloadVector *result,
    PcpArcInfoVector *info,
    std::unordered_set<std::string> *exprVarDependencies,
    PcpErrorVector *errors);

inline void
PcpComposeSitePayloads(
    PcpLayerStackRefPtr const &layerStack,
    SdfPath const &path,
    SdfPayloadVector *result,
    PcpArcInfoVector *info)
{
    return PcpComposeSitePayloads(
        layerStack, path, result, info, nullptr, nullptr);
}

inline void
PcpComposeSitePayloads(
    PcpNodeRef const &node,
    SdfPayloadVector *result,
    PcpArcInfoVector *info,
    std::unordered_set<std::string> *exprVarDependencies,
    PcpErrorVector *errors)
{
    return PcpComposeSitePayloads(
        node.GetLayerStack(), node.GetPath(),
        result, info, exprVarDependencies, errors);
}

inline void
PcpComposeSitePayloads(
    PcpNodeRef const &node,
    SdfPayloadVector *result,
    PcpArcInfoVector *info)
{
    return PcpComposeSitePayloads(
        node.GetLayerStack(), node.GetPath(), result, info, nullptr, nullptr);
}

/// Permission
PCP_API
SdfPermission
PcpComposeSitePermission(PcpLayerStackRefPtr const &layerStack,
                         SdfPath const &path);

inline SdfPermission
PcpComposeSitePermission(PcpNodeRef const &node)
{
    return PcpComposeSitePermission(node.GetLayerStack(), node.GetPath());
}

/// Prim sites
PCP_API
void
PcpComposeSitePrimSites(PcpLayerStackRefPtr const &layerStack,
                        SdfPath const &path,
                        SdfSiteVector *result);

inline void
PcpComposeSitePrimSites(PcpNodeRef const &node, SdfSiteVector *result)
{
    return PcpComposeSitePrimSites(
        node.GetLayerStack(), node.GetPath(), result);
}

/// Relocates
PCP_API
void
PcpComposeSiteRelocates(PcpLayerStackRefPtr const &layerStack,
                        SdfPath const &path,
                        SdfRelocatesMap *result);

inline void
PcpComposeSiteRelocates(PcpNodeRef const &node, SdfRelocatesMap *result)
{
    return PcpComposeSiteRelocates(
        node.GetLayerStack(), node.GetPath(), result);
}

/// Has prim specs.
PCP_API
bool
PcpComposeSiteHasPrimSpecs(PcpLayerStackRefPtr const &layerStack,
                           SdfPath const &path);
inline bool
PcpComposeSiteHasPrimSpecs(PcpNodeRef const &node)
{
    return PcpComposeSiteHasPrimSpecs(node.GetLayerStack(), node.GetPath());
}

/// Symmetry
PCP_API
bool
PcpComposeSiteHasSymmetry(PcpLayerStackRefPtr const &layerStack,
                          SdfPath const &path);
inline bool
PcpComposeSiteHasSymmetry(PcpNodeRef const &node)
{
    return PcpComposeSiteHasSymmetry(node.GetLayerStack(), node.GetPath());
}

/// Inherits
PCP_API
void
PcpComposeSiteInherits(PcpLayerStackRefPtr const &layerStack,
                       SdfPath const &path, SdfPathVector *result,
                       PcpArcInfoVector *info);

PCP_API
void
PcpComposeSiteInherits(PcpLayerStackRefPtr const &layerStack,
                       SdfPath const &path, SdfPathVector *result);

inline void
PcpComposeSiteInherits(PcpNodeRef const &node, SdfPathVector *result)
{
    return PcpComposeSiteInherits(node.GetLayerStack(), node.GetPath(), result);
}

/// Specializes
PCP_API
void
PcpComposeSiteSpecializes(PcpLayerStackRefPtr const &layerStack,
                          SdfPath const &path, SdfPathVector *result,
                          PcpArcInfoVector *info);

PCP_API
void
PcpComposeSiteSpecializes(PcpLayerStackRefPtr const &layerStack,
                          SdfPath const &path, SdfPathVector *result);

inline void
PcpComposeSiteSpecializes(PcpNodeRef const &node, SdfPathVector *result)
{
    return PcpComposeSiteSpecializes(
        node.GetLayerStack(), node.GetPath(), result);
}

/// VariantSets
PCP_API
void
PcpComposeSiteVariantSets(PcpLayerStackRefPtr const &layerStack,
                          SdfPath const &path,
                          std::vector<std::string> *result,
                          PcpArcInfoVector *info);

PCP_API
void
PcpComposeSiteVariantSets(PcpLayerStackRefPtr const &layerStack,
                          SdfPath const &path,
                          std::vector<std::string> *result);
inline void
PcpComposeSiteVariantSets(PcpNodeRef const &node,
                          std::vector<std::string> *result) {
    return PcpComposeSiteVariantSets(
        node.GetLayerStack(), node.GetPath(), result);
}

/// VariantSetOptions
PCP_API
void
PcpComposeSiteVariantSetOptions(PcpLayerStackRefPtr const &layerStack,
                                SdfPath const &path,
                                std::string const &vsetName,
                                std::set<std::string> *result);
inline void
PcpComposeSiteVariantSetOptions(PcpNodeRef const &node,
                                std::string const &vsetName,
                                std::set<std::string> *result)
{
    return PcpComposeSiteVariantSetOptions(
        node.GetLayerStack(), node.GetPath(), vsetName, result);
}

/// VariantSelection
PCP_API
bool
PcpComposeSiteVariantSelection(
    PcpLayerStackRefPtr const &layerStack,
    SdfPath const &path,
    std::string const &vsetName,
    std::string *result,
    std::unordered_set<std::string> *exprVarDependencies,
    PcpErrorVector *errors);

inline bool
PcpComposeSiteVariantSelection(
    PcpLayerStackRefPtr const &layerStack,
    SdfPath const &path,
    std::string const &vsetName,
    std::string *result)
{
    return PcpComposeSiteVariantSelection(
        layerStack, path, vsetName, result, nullptr, nullptr);
}

inline bool
PcpComposeSiteVariantSelection(PcpNodeRef const &node,
                               std::string const &vsetName,
                               std::string *result)
{
    return PcpComposeSiteVariantSelection(
        node.GetLayerStack(), node.GetPath(), vsetName, result);
}

/// VariantSelections
PCP_API
void 
PcpComposeSiteVariantSelections(
    PcpLayerStackRefPtr const &layerStack,
    SdfPath const &path,
    SdfVariantSelectionMap *result,
    std::unordered_set<std::string> *exprVarDependencies,
    PcpErrorVector *errors);

inline void 
PcpComposeSiteVariantSelections(
    PcpLayerStackRefPtr const &layerStack,
    SdfPath const &path,
    SdfVariantSelectionMap *result)
{
    return PcpComposeSiteVariantSelections(
        layerStack, path, result, nullptr, nullptr);
}

inline void
PcpComposeSiteVariantSelections(PcpNodeRef const &node,
                                SdfVariantSelectionMap *result)
{
    return PcpComposeSiteVariantSelections(
        node.GetLayerStack(), node.GetPath(), result);
}

PCP_API
bool
PcpComposeSiteHasVariantSelections(
    PcpLayerStackRefPtr const &layerStack,
    SdfPath const &path);

/// Compose child names.
/// If the optional \p orderField is provided, its order will be applied.
PCP_API
void
PcpComposeSiteChildNames(SdfLayerRefPtrVector const &layers,
                         SdfPath const &path,
                         const TfToken & namesField,
                         TfTokenVector *nameOrder,
                         PcpTokenSet *nameSet,
                         const TfToken *orderField = nullptr);

PXR_NAMESPACE_CLOSE_SCOPE

#endif // PXR_USD_PCP_COMPOSE_SITE_H
