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
#include "pxr/imaging/hdSt/hgiUnitTestHelper.h"
#include "pxr/imaging/hgi/blitCmds.h"
#include "pxr/imaging/hgi/blitCmdsOps.h"

#include <iostream>
#include <string>
#include <sstream>

PXR_NAMESPACE_OPEN_SCOPE

HdSt_InitializationTestDriver::HdSt_InitializationTestDriver():
    _hgi(Hgi::CreateHgiOfChoice(TfToken("HgiVulkan")))
{
}

HdSt_InitializationTestDriver::~HdSt_InitializationTestDriver()
{
}

static const std::string glslfx_vertShaderStr =
"void main(void)\n"
"{\n"
"    gl_Position = position;\n"
"    uvOut = uvIn;\n"
"}\n";

static const std::string glslfx_fragShaderStr =
"void main(void)\n"
"{\n"
"    vec4 color = vec4(1.0, 0.0, 1.0, 1.0);\n"
"    hd_FragColor = color;\n"
"}\n";


HdSt_PipelineCreationTestDriver::HdSt_PipelineCreationTestDriver():
    _hgi(Hgi::CreateHgiOfChoice(TfToken("HgiVulkan")))
{
    _CreateVertexBufferDescriptor();
}

HdSt_PipelineCreationTestDriver::~HdSt_PipelineCreationTestDriver()
{
    if (_shaderProgram) {
        _DestroyShaderProgram();
    }
    if (_pipeline) {
        _hgi->DestroyGraphicsPipeline(&_pipeline);
    }
}

void
HdSt_PipelineCreationTestDriver::_CreateVertexBufferDescriptor()
{
    HgiVertexAttributeDesc posAttr;
    posAttr.format = HgiFormatFloat32Vec3;
    posAttr.offset = 0;
    posAttr.shaderBindLocation = 0;

    HgiVertexAttributeDesc uvAttr;
    uvAttr.format = HgiFormatFloat32Vec2;
    uvAttr.offset = sizeof(float) * 4; // after posAttr
    uvAttr.shaderBindLocation = 1;

    _vboDesc.bindingIndex = 0;
    _vboDesc.vertexStride = sizeof(float) * 6; // pos, uv
    _vboDesc.vertexAttributes = { posAttr, uvAttr };
}

bool
HdSt_PipelineCreationTestDriver::CreateTestPipeline()
{
    if (!_CreateShaderProgram())
        return false;     

    if(!_CreatePipeline())
        return false;

    return true;
}

bool 
HdSt_PipelineCreationTestDriver::_CreateShaderProgram()
{
    PXR_INTERNAL_NS::HgiShaderFunctionDesc vertDesc;
    vertDesc.debugName = PXR_INTERNAL_NS::TfToken("Vertex");
    vertDesc.shaderStage = PXR_INTERNAL_NS::HgiShaderStageVertex;
    PXR_INTERNAL_NS::HgiShaderFunctionAddStageInput(
        &vertDesc, "position", "vec4", "position");
    PXR_INTERNAL_NS::HgiShaderFunctionAddStageInput(
        &vertDesc, "uvIn", "vec2");
    PXR_INTERNAL_NS::HgiShaderFunctionAddStageOutput(
        &vertDesc, "gl_Position", "vec4", "position");
    PXR_INTERNAL_NS::HgiShaderFunctionAddStageOutput(
        &vertDesc, "uvOut", "vec2");

    PXR_INTERNAL_NS::HgiShaderFunctionDesc fragDesc;
    fragDesc.debugName = PXR_INTERNAL_NS::TfToken("Fragment");
    fragDesc.shaderStage = PXR_INTERNAL_NS::HgiShaderStageFragment;
    PXR_INTERNAL_NS::HgiShaderFunctionAddStageInput(
        &fragDesc, "uvOut", "vec2");
    PXR_INTERNAL_NS::HgiShaderFunctionAddStageOutput(
        &fragDesc, "hd_FragColor", "vec4", "color");

    vertDesc.shaderCode = glslfx_vertShaderStr.c_str();
    PXR_INTERNAL_NS::HgiShaderFunctionHandle vertFn = _hgi->CreateShaderFunction(vertDesc);
    
    fragDesc.shaderCode = glslfx_fragShaderStr.c_str();
    PXR_INTERNAL_NS::HgiShaderFunctionHandle fragFn = _hgi->CreateShaderFunction(fragDesc);
    
    PXR_INTERNAL_NS::HgiShaderProgramDesc programDesc;
    programDesc.debugName = PXR_INTERNAL_NS::TfToken("FullscreenTriangle").GetString();
    programDesc.shaderFunctions.push_back(std::move(vertFn));
    programDesc.shaderFunctions.push_back(std::move(fragFn));
    _shaderProgram = _hgi->CreateShaderProgram(programDesc);
    
    if (!_shaderProgram->IsValid() || !vertFn->IsValid() || !fragFn->IsValid()) {
        TF_CODING_ERROR("Failed to create shader program");
        _PrintCompileErrors();
        _DestroyShaderProgram();
        return false;
    }

    return true;
}

bool
HdSt_PipelineCreationTestDriver::_CreatePipeline()
{
    if (_pipeline) {
        _hgi->DestroyGraphicsPipeline(&_pipeline);
    }

    // Setup color attachment
    _colorAtt.blendEnabled = false;
    _colorAtt.loadOp = HgiAttachmentLoadOpDontCare;
    _colorAtt.storeOp = HgiAttachmentStoreOpStore;
    _colorAtt.srcColorBlendFactor = HgiBlendFactorZero;
    _colorAtt.dstColorBlendFactor = HgiBlendFactorZero;
    _colorAtt.colorBlendOp = HgiBlendOpAdd;
    _colorAtt.srcAlphaBlendFactor = HgiBlendFactorZero;
    _colorAtt.dstAlphaBlendFactor = HgiBlendFactorZero;
    _colorAtt.alphaBlendOp = HgiBlendOpAdd;
    _colorAtt.format = HgiFormatUNorm8Vec4;                  // setting some format
    _colorAtt.usage = HgiTextureUsageBitsColorTarget;        // setting some target
    
    // Setup depth attachment
    _depthAtt = PXR_INTERNAL_NS::HgiAttachmentDesc{};
    _depthAtt.blendEnabled = false;
    _depthAtt.loadOp = PXR_INTERNAL_NS::HgiAttachmentLoadOpDontCare;
    _depthAtt.storeOp = PXR_INTERNAL_NS::HgiAttachmentStoreOpStore;
    _depthAtt.srcColorBlendFactor = PXR_INTERNAL_NS::HgiBlendFactorZero;
    _depthAtt.dstColorBlendFactor = PXR_INTERNAL_NS::HgiBlendFactorZero;
    _depthAtt.colorBlendOp = PXR_INTERNAL_NS::HgiBlendOpAdd;
    _depthAtt.srcAlphaBlendFactor = PXR_INTERNAL_NS::HgiBlendFactorZero;
    _depthAtt.dstAlphaBlendFactor = PXR_INTERNAL_NS::HgiBlendFactorZero;
    _depthAtt.alphaBlendOp = PXR_INTERNAL_NS::HgiBlendOpAdd;
    _depthAtt.format = HgiFormatFloat32;
    _depthAtt.usage = HgiTextureUsageBitsDepthTarget;
    
    PXR_INTERNAL_NS::HgiGraphicsPipelineDesc desc;
    desc.debugName = "Forward Pipeline";
    desc.shaderProgram = _shaderProgram;
    desc.colorAttachmentDescs.push_back(_colorAtt);
    desc.depthAttachmentDesc = _depthAtt;

    HgiDepthStencilState depthState;
    depthState.depthTestEnabled = true;
    depthState.depthCompareFn = HgiCompareFunctionAlways;
    depthState.stencilTestEnabled = false;
    desc.depthState = depthState;

    desc.vertexBuffers = { _vboDesc };
    desc.depthState.depthWriteEnabled = false;
    desc.multiSampleState.alphaToCoverageEnable = false;
    desc.rasterizationState.cullMode = HgiCullModeBack;
    desc.rasterizationState.polygonMode = HgiPolygonModeFill;
    desc.rasterizationState.winding = HgiWindingCounterClockwise;
    desc.shaderProgram = _shaderProgram;

    _pipeline = _hgi->CreateGraphicsPipeline(desc);
    if (!_pipeline)
        return false;

    return true;
}

void
HdSt_PipelineCreationTestDriver::_DestroyShaderProgram()
{
    for (HgiShaderFunctionHandle fn : _shaderProgram->GetShaderFunctions()) {
        _hgi->DestroyShaderFunction(&fn);
    }
    _hgi->DestroyShaderProgram(&_shaderProgram);
}

void
HdSt_PipelineCreationTestDriver::_PrintCompileErrors()
{
    for (HgiShaderFunctionHandle fn : _shaderProgram->GetShaderFunctions()) {
        std::cout << fn->GetCompileErrors() << std::endl;
    }
    std::cout << _shaderProgram->GetCompileErrors() << std::endl;
}

HdSt_GfxCmdBfrExecutionTestDriver::HdSt_GfxCmdBfrExecutionTestDriver()
    : HdSt_PipelineCreationTestDriver()
    , _renderDim(600, 400, 1)
{
}

HdSt_GfxCmdBfrExecutionTestDriver::~HdSt_GfxCmdBfrExecutionTestDriver()
{
    if (_colorTarget)
        _hgi->DestroyTexture(&_colorTarget);

    if (_colorTargetView)
        _hgi->DestroyTextureView(&_colorTargetView);

    if (_depthTarget)
        _hgi->DestroyTexture(&_depthTarget);

    if (_depthTargetView)
        _hgi->DestroyTextureView(&_depthTargetView);

    if (_vertexBuffer)
        _hgi->DestroyBuffer(&_vertexBuffer);

    if (_indexBuffer)
        _hgi->DestroyBuffer(&_indexBuffer);
}

bool HdSt_GfxCmdBfrExecutionTestDriver::ExecuteTestGfxCmdBfr()
{
    if (!_CreateResourceBuffers())
        return false;

    if (!_CreateRenderTargets())
        return false;

    {
        const GfVec4i viewport(0, 0, _renderDim[0], _renderDim[1]);

        HgiGraphicsCmdsDesc gfxDesc;
        gfxDesc.colorAttachmentDescs.push_back(_colorAtt);
        gfxDesc.depthAttachmentDesc = _depthAtt;
        gfxDesc.colorTextures.push_back(_colorTarget);
        gfxDesc.depthTexture = _depthTarget;

        HgiGraphicsCmdsUniquePtr gfxCmds = _hgi->CreateGraphicsCmds(gfxDesc);
        gfxCmds->PushDebugGroup("Draw Test");
        gfxCmds->BindPipeline(_pipeline);
        gfxCmds->BindVertexBuffers({ {_vertexBuffer, 0, 0} });
        gfxCmds->SetViewport(viewport);
        gfxCmds->DrawIndexed(_indexBuffer, 3, 0, 0, 1, 0);
        gfxCmds->PopDebugGroup();

        _hgi->SubmitCmds(gfxCmds.get(), HgiSubmitWaitTypeWaitUntilCompleted);
    }

    return true;
}

bool HdSt_GfxCmdBfrExecutionTestDriver::WriteToDisk(const std::string& filePath)
{
    const size_t byteSize = pxr::HgiGetDataSize(pxr::HgiFormatUNorm8Vec4, _renderDim);

    std::vector<uint8_t> texels(byteSize, 0);
    HgiTextureGpuToCpuOp readBackOp{};
    readBackOp.cpuDestinationBuffer = texels.data();
    readBackOp.destinationBufferByteSize = byteSize;
    readBackOp.destinationByteOffset = 0;
    readBackOp.gpuSourceTexture = _colorTarget;
    readBackOp.mipLevel = 0;
    readBackOp.sourceTexelOffset = pxr::GfVec3i(0);

    pxr::HgiBlitCmdsUniquePtr blitCmds = _hgi->CreateBlitCmds();
    blitCmds->CopyTextureGpuToCpu(readBackOp);
    _hgi->SubmitCmds(blitCmds.get(), pxr::HgiSubmitWaitTypeWaitUntilCompleted);

    
    HioImage::StorageSpec storage;
    storage.width = _renderDim[0];
    storage.height = _renderDim[1];
    storage.format = pxr::HioFormatUNorm8Vec4;
    storage.data = readBackOp.cpuDestinationBuffer;

    if (storage.format == HioFormatInvalid) {
        printf("Hgi texture has format not corresponding to a"
                        "HioFormat");
        return false;
    }

    if (!storage.data) {
        printf("No data for texture");
        return false;
    }
        
    HioImageSharedPtr const image = HioImage::OpenForWriting(filePath);
    if (!image) {
        printf("Failed to open image for writing %s",
            filePath.c_str());
        return false;
    }

    if (!image->Write(storage)) {
        printf("Failed to write image to %s", filePath.c_str());
        return false;
    }
    
    return true;
}

bool HdSt_GfxCmdBfrExecutionTestDriver::_CreateResourceBuffers()
{
    if (_vertexBuffer)
       _hgi->DestroyBuffer(&_vertexBuffer);
    
    if(_indexBuffer)
        _hgi->DestroyBuffer(&_indexBuffer);

    constexpr size_t elementsPerVertex = 6;
    constexpr size_t vertDataCount = elementsPerVertex * 3;
    constexpr float vertData[vertDataCount] =
    { -0.25,  0.25, 0, 0.25,     0, 1,
      -0.25, -0.25, 0, 0.25,     0, 0,
       0.25, -0.25, 0, 0.25,     0.25, 0 };

    HgiBufferDesc vboDesc{};
    vboDesc.debugName = "VertexBuffer";
    vboDesc.usage = HgiBufferUsageVertex;
    vboDesc.initialData = vertData;
    vboDesc.byteSize = sizeof(vertData);
    vboDesc.vertexStride = elementsPerVertex * sizeof(vertData[0]);
    _vertexBuffer = _hgi->CreateBuffer(vboDesc);
    if (!_vertexBuffer)
        return false;

    static const int32_t indices[3] = { 0, 1, 2 };

    HgiBufferDesc iboDesc;
    iboDesc.debugName = "IndexBuffer";
    iboDesc.usage = HgiBufferUsageIndex32;
    iboDesc.initialData = indices;
    iboDesc.byteSize = sizeof(indices) * sizeof(indices[0]);
    _indexBuffer = _hgi->CreateBuffer(iboDesc);
    if (!_indexBuffer)
        return false;

    return true;
}

bool HdSt_GfxCmdBfrExecutionTestDriver::_CreateRenderTargets()
{
    if (_colorTarget)
        _hgi->DestroyTexture(&_colorTarget);

    if (_colorTargetView)
        _hgi->DestroyTextureView(&_colorTargetView);

    if (_depthTarget)
        _hgi->DestroyTexture(&_depthTarget);

    if (_depthTargetView)
        _hgi->DestroyTextureView(&_depthTargetView);

    // create color Render Target
    HgiTextureDesc desc{};
    desc.componentMapping = HgiComponentMapping{
        HgiComponentSwizzleR, HgiComponentSwizzleG,
        HgiComponentSwizzleB, HgiComponentSwizzleA, };
    desc.debugName = "Color Buffer";
    desc.dimensions = _renderDim;
    desc.format = HgiFormatUNorm8Vec4;
    desc.initialData = nullptr;
    desc.layerCount = 1;
    desc.mipLevels = 1;
    desc.pixelsByteSize = 0;
    desc.sampleCount = HgiSampleCount1;
    desc.type = HgiTextureType2D;
    desc.usage = HgiTextureUsageBitsColorTarget;
    _colorTarget = _hgi->CreateTexture(desc);
    if (!_colorTarget)
        return false;

    // create color view
    HgiTextureViewDesc viewDesc{};
    viewDesc.debugName = "Color Buffer View";
    viewDesc.format = HgiFormatUNorm8Vec4;;
    viewDesc.layerCount = 1;
    viewDesc.mipLevels = 1;
    viewDesc.sourceFirstLayer = 0;
    viewDesc.sourceFirstMip = 0;
    viewDesc.sourceTexture = _colorTarget;
    _colorTargetView = _hgi->CreateTextureView(viewDesc);
    if (!_colorTargetView)
        return false;

    // create depth target
    desc = HgiTextureDesc{};
    desc.componentMapping = HgiComponentMapping{
        HgiComponentSwizzleR, HgiComponentSwizzleG,
        HgiComponentSwizzleB, HgiComponentSwizzleA, };
    desc.debugName = "Depth Buffer";
    desc.dimensions = _renderDim;
    desc.format = HgiFormatFloat32;
    desc.initialData = nullptr;
    desc.layerCount = 1;
    desc.mipLevels = 1;
    desc.pixelsByteSize = 0;
    desc.sampleCount = HgiSampleCount1;
    desc.type = HgiTextureType2D;
    desc.usage = HgiTextureUsageBitsDepthTarget;
    _depthTarget = _hgi->CreateTexture(desc);
    if (!_depthTarget)
        return false;

    // create depth view
    viewDesc = HgiTextureViewDesc{};
    viewDesc.debugName = "Depth Buffer View";
    viewDesc.format = HgiFormatFloat32;;
    viewDesc.layerCount = 1;
    viewDesc.mipLevels = 1;
    viewDesc.sourceFirstLayer = 0;
    viewDesc.sourceFirstMip = 0;
    viewDesc.sourceTexture = _depthTarget;
    _depthTargetView = _hgi->CreateTextureView(viewDesc);
    if (!_depthTargetView)
        return false;

    return true;
}

PXR_NAMESPACE_CLOSE_SCOPE