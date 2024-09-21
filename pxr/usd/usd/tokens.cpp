//
// Copyright 2016 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
#include "pxr/usd/usd/tokens.h"

PXR_NAMESPACE_OPEN_SCOPE

UsdTokensType::UsdTokensType() :
    apiSchemas("apiSchemas", TfToken::Immortal),
    clips("clips", TfToken::Immortal),
    clipSets("clipSets", TfToken::Immortal),
    collection("collection", TfToken::Immortal),
    collection_MultipleApplyTemplate_("collection:__INSTANCE_NAME__", TfToken::Immortal),
    collection_MultipleApplyTemplate_Excludes("collection:__INSTANCE_NAME__:excludes", TfToken::Immortal),
    collection_MultipleApplyTemplate_ExpansionRule("collection:__INSTANCE_NAME__:expansionRule", TfToken::Immortal),
    collection_MultipleApplyTemplate_IncludeRoot("collection:__INSTANCE_NAME__:includeRoot", TfToken::Immortal),
    collection_MultipleApplyTemplate_Includes("collection:__INSTANCE_NAME__:includes", TfToken::Immortal),
    collection_MultipleApplyTemplate_MembershipExpression("collection:__INSTANCE_NAME__:membershipExpression", TfToken::Immortal),
    colorSpaceBlueChroma("colorSpace:blueChroma", TfToken::Immortal),
    colorSpaceDefinitionName("colorSpace:definitionName", TfToken::Immortal),
    colorSpaceGamma("colorSpace:gamma", TfToken::Immortal),
    colorSpaceGreenChroma("colorSpace:greenChroma", TfToken::Immortal),
    colorSpaceLinearBias("colorSpace:linearBias", TfToken::Immortal),
    colorSpaceName("colorSpace:name", TfToken::Immortal),
    colorSpaceRedChroma("colorSpace:redChroma", TfToken::Immortal),
    colorSpaceWhitePoint("colorSpace:whitePoint", TfToken::Immortal),
    custom("custom", TfToken::Immortal),
    exclude("exclude", TfToken::Immortal),
    expandPrims("expandPrims", TfToken::Immortal),
    expandPrimsAndProperties("expandPrimsAndProperties", TfToken::Immortal),
    explicitOnly("explicitOnly", TfToken::Immortal),
    fallbackPrimTypes("fallbackPrimTypes", TfToken::Immortal),
    lin_rec709("lin_rec709", TfToken::Immortal),
    APISchemaBase("APISchemaBase", TfToken::Immortal),
    ClipsAPI("ClipsAPI", TfToken::Immortal),
    CollectionAPI("CollectionAPI", TfToken::Immortal),
    ColorSpaceAPI("ColorSpaceAPI", TfToken::Immortal),
    ColorSpaceDefinitionAPI("ColorSpaceDefinitionAPI", TfToken::Immortal),
    ModelAPI("ModelAPI", TfToken::Immortal),
    Typed("Typed", TfToken::Immortal),
    allTokens({
        apiSchemas,
        clips,
        clipSets,
        collection,
        collection_MultipleApplyTemplate_,
        collection_MultipleApplyTemplate_Excludes,
        collection_MultipleApplyTemplate_ExpansionRule,
        collection_MultipleApplyTemplate_IncludeRoot,
        collection_MultipleApplyTemplate_Includes,
        collection_MultipleApplyTemplate_MembershipExpression,
        colorSpaceBlueChroma,
        colorSpaceDefinitionName,
        colorSpaceGamma,
        colorSpaceGreenChroma,
        colorSpaceLinearBias,
        colorSpaceName,
        colorSpaceRedChroma,
        colorSpaceWhitePoint,
        custom,
        exclude,
        expandPrims,
        expandPrimsAndProperties,
        explicitOnly,
        fallbackPrimTypes,
        lin_rec709,
        APISchemaBase,
        ClipsAPI,
        CollectionAPI,
        ColorSpaceAPI,
        ColorSpaceDefinitionAPI,
        ModelAPI,
        Typed
    })
{
}

TfStaticData<UsdTokensType> UsdTokens;

PXR_NAMESPACE_CLOSE_SCOPE
