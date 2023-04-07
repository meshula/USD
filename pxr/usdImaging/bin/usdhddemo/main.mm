//
// cc glfw-metal-example.m `pkg-config --cflags --libs glfw3` -framework AppKit -framework Metal -framework QuartzCore
//
#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

#include <pxr/base/plug/info.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usdGeom/xformable.h>

#include <pxr/imaging/hd/driver.h>
#include <pxr/imaging/hdSt/textureUtils.h>
#include <pxr/imaging/hgiMetal/hgi.h>
#include <pxr/imaging/hgiMetal/texture.h>
#include <pxr/imaging/hio/image.h>
#include <pxr/imaging/hio/imageRegistry.h>
#include <pxr/usdImaging/usdImagingGL/engine.h>

#include <iostream>

PXR_NAMESPACE_USING_DIRECTIVE

static void quit(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}


static const MTLPixelFormat AAPLDefaultColorPixelFormat = MTLPixelFormatBGRA8Unorm;

/// Copy a texture to the view
void blit(MTLRenderPassDescriptor* renderPassDescriptor,
          id<MTLRenderPipelineState> blitToViewPSO,
          id<MTLCommandBuffer> commandBuffer,
          id<MTLTexture> texture)
{
    if (!renderPassDescriptor)
        return;
    
    // Create a render command encoder to encode copy command.
    id <MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
    if (!renderEncoder)
        return;
    
    // Blit the texture to the view.
    [renderEncoder pushDebugGroup:@"Blit"];
    [renderEncoder setFragmentTexture:texture atIndex:0];
    [renderEncoder setRenderPipelineState:blitToViewPSO];
    [renderEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:3];
    [renderEncoder popDebugGroup];
}

int main(int argc, char *argv[])
{
    //-------------------------------------------------------------------------
    // USD set up
    //-------------------------------------------------------------------------
    //Plug_InitConfig();
    std::vector<std::string> test;
    std::vector<std::string> paths;
    paths.emplace_back("/var/tmp/usd-230402/lib/usd");
    paths.emplace_back("/var/tmp/usd-230402/plugin/usd");
    std::vector<std::string> msgs;
    msgs.emplace_back("looking for plugs here: /var/tmp/usd-230327/lib/usd");
    Plug_SetPaths(paths, msgs, true);

    UsdStageRefPtr _stage;
    _stage = UsdStage::CreateNew("/var/tmp/cube.usda");
    // create a cube on the stage
    _stage->DefinePrim(SdfPath("/Box"), TfToken("Cube"));
    UsdPrim cube = _stage->GetPrimAtPath(SdfPath("/Box"));
    GfVec3f scaleVec = { 5.f, 5.f, 5.f };
    UsdGeomXformable cubeXf(cube);
    cubeXf.AddScaleOp().Set(scaleVec);
    
    //-------------------------------------------------------------------------
    // Hydra set up
    //-------------------------------------------------------------------------
    HgiUniquePtr _hgi;
    _hgi = Hgi::CreatePlatformDefaultHgi();
    HdDriver driver{HgiTokens->renderDriver, VtValue(_hgi.get())};
    
    std::shared_ptr<class UsdImagingGLEngine> _engine;
    SdfPathVector excludedPaths;
    _engine.reset(new UsdImagingGLEngine(_stage->GetPseudoRoot().GetPath(),
                                         excludedPaths, SdfPathVector(),
                                         SdfPath::AbsoluteRootPath(), driver));
    _engine->SetEnablePresentation(false);
    _engine->SetRendererAov(HdAovTokens->color);

    //-------------------------------------------------------------------------
    // Metal set up
    //-------------------------------------------------------------------------
    const id<MTLDevice> gpu = MTLCreateSystemDefaultDevice();
    const id<MTLCommandQueue> queue = [gpu newCommandQueue];
    CAMetalLayer *swapchain = [CAMetalLayer layer];
    swapchain.device = gpu;
    swapchain.opaque = YES;
    MTLClearColor color = MTLClearColorMake(0, 0, 0, 1);

    //-------------------------------------------------------------------------
    // GLFW set up
    //-------------------------------------------------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    /*
    glfwWindowHint(GLFW_FLOAT_PIXEL_TYPE, GLFW_TRUE);
    glfwWindowHint(GLFW_RED_BITS, 16);
    glfwWindowHint(GLFW_GREEN_BITS, 16);
    glfwWindowHint(GLFW_BLUE_BITS, 16);
    glfwWindowHint(GLFW_ALPHA_BITS, 16);
    */
    
    GLFWwindow *window = glfwCreateWindow(640, 480, "USD Hydra demo", NULL, NULL);
    NSWindow *nswindow = glfwGetCocoaWindow(window);
    nswindow.contentView.layer = swapchain;
    nswindow.contentView.wantsLayer = YES;

    glfwSetKeyCallback(window, quit);

    //-------------------------------------------------------------------------
    // create blit to view pipeline
    //-------------------------------------------------------------------------
    id<MTLRenderPipelineState> blitToViewPSO;
    NSError* error = NULL;
    id<MTLLibrary> defaultLibrary = [gpu newDefaultLibrary];
    id<MTLFunction> vertexFunction = [defaultLibrary newFunctionWithName:@"vtxBlit"];
    id<MTLFunction> fragmentFunction = [defaultLibrary newFunctionWithName:@"fragBlitLinear"];
    
    // Set up the pipeline state object.
    MTLRenderPipelineDescriptor* pipelineStateDescriptor =
                                        [[MTLRenderPipelineDescriptor alloc] init];
    pipelineStateDescriptor.rasterSampleCount = 1;
    pipelineStateDescriptor.vertexFunction = vertexFunction;
    pipelineStateDescriptor.fragmentFunction = fragmentFunction;
    pipelineStateDescriptor.depthAttachmentPixelFormat = MTLPixelFormatInvalid;
    
    // Configure the color attachment for blending.
    MTLRenderPipelineColorAttachmentDescriptor* colorDescriptor = pipelineStateDescriptor.colorAttachments[0];
    colorDescriptor.pixelFormat = AAPLDefaultColorPixelFormat;
    colorDescriptor.blendingEnabled = YES;
    colorDescriptor.rgbBlendOperation = MTLBlendOperationAdd;
    colorDescriptor.alphaBlendOperation = MTLBlendOperationAdd;
    colorDescriptor.sourceRGBBlendFactor = MTLBlendFactorOne;
    colorDescriptor.sourceAlphaBlendFactor = MTLBlendFactorOne;
    colorDescriptor.destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
    colorDescriptor.destinationAlphaBlendFactor = MTLBlendFactorZero;
    
    blitToViewPSO = [gpu newRenderPipelineStateWithDescriptor:pipelineStateDescriptor
                                                        error:&error];
    if (!blitToViewPSO)
    {
        NSLog(@"Failed to created pipeline state, error %@", error);
    }
    
    
    
    //-------------------------------------------------------------------------
    // Read a texture
    //-------------------------------------------------------------------------
    const std::string stillLife = "/Users/nporcino/dev/assets/StillLife.exr";
    HioImageSharedPtr _image;
    bool canReadExr = HioImage::IsSupportedImageFile(stillLife);
    std::unique_ptr<char[]> imageData;
    HioImage::StorageSpec _spec;
    if (canReadExr) {
        _image = HioImage::OpenForReading(stillLife,
                                              0, // int subimage,
                                              0, // int mip,
                                              HioImage::SourceColorSpace::Auto,
                                              false); //bool suppressErrors)
        
        std::cout << "filename: " << _image->GetFilename() << "\n";
        std::cout << " dimensions: " << _image->GetWidth() << ", " << _image->GetHeight() << "\n";
        switch (_image->GetFormat()) {
            case HioFormatFloat16:
                std::cout << " format: HioFormatFloat16\n"; break;
            case HioFormatFloat16Vec2:
                std::cout << " format: HioFormatFloat16Vec2\n"; break;
            case HioFormatFloat16Vec3:
                std::cout << " format: HioFormatFloat16Vec3\n"; break;
            case HioFormatFloat16Vec4:
                std::cout << " format: HioFormatFloat16Vec4\n"; break;
            default:
                std::cout << " format: " << _image->GetFormat() << "\n"; break;
        }
        std::cout << " bytes per pixel: " << _image->GetBytesPerPixel() << "\n";
        std::cout << " mips: " << _image->GetNumMipLevels() << "\n";
        std::cout << (_image->IsColorSpaceSRGB() ? " srgb pixels\n" : " linear pixels\n");

        int cropTop = 123;
        int cropBottom = 102;
        
        _spec.width  = (int)(_image->GetWidth() * 0.85f);
        _spec.height = (int)((_image->GetHeight() - cropTop - cropBottom) * 0.85f);
        _spec.format = _image->GetFormat();
        _spec.flipped = false;

        const size_t bufsize = _spec.width * _spec.height * _image->GetBytesPerPixel();
        imageData.reset(new char[bufsize]);

        _spec.data = imageData.get();
        if (_image->ReadCropped(cropTop, cropBottom, 0, 0, _spec)) {
            // successfully read the image!
        }
    }
    
    //-------------------------------------------------------------------------
    // Create a hydra texture from the OpenEXR image
    //-------------------------------------------------------------------------
    id<MTLTexture> _metalTexture = nil;
    if (_image) {
        HgiTextureDesc textureDesc;
        textureDesc.debugName = "OpenEXRTexture";
        textureDesc.usage = HgiTextureUsageBitsShaderRead;
        textureDesc.format = HdStTextureUtils::GetHgiFormat(_spec.format, true);
        textureDesc.type = HgiTextureType2D;
        textureDesc.dimensions = GfVec3i(_spec.width, _spec.height, 1);
        textureDesc.layerCount = 1;
        textureDesc.mipLevels = 1;
        textureDesc.pixelsByteSize = _image->GetBytesPerPixel();
        textureDesc.initialData = _spec.data;
        
        HgiTextureHandle _gpuTexture = _hgi->CreateTexture(textureDesc);
        HgiMetalTexture* hgiMetalTexture = static_cast<HgiMetalTexture*>(_gpuTexture.Get());
        if (hgiMetalTexture) {
            _metalTexture = hgiMetalTexture->GetTextureId();
        }
    }
    
    
    //-------------------------------------------------------------------------
    // Render loop
    //-------------------------------------------------------------------------

    //const uint32_t AAPLMaxBuffersInFlight = 3;
    //dispatch_semaphore_t _inFlightSemaphore;
    //_inFlightSemaphore = dispatch_semaphore_create(AAPLMaxBuffersInFlight);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        // glfw provides its own semaphore
        //dispatch_semaphore_wait(_inFlightSemaphore, DISPATCH_TIME_FOREVER);

        static bool once = true;
        if (once) {
            

            
            once = false;
        }
        
        @autoreleasepool {
            color.red = (color.red > 1.0) ? 0 : color.red + 0.01;

            id<CAMetalDrawable> surface = [swapchain nextDrawable];

            MTLRenderPassDescriptor *pass = [MTLRenderPassDescriptor renderPassDescriptor];
            pass.colorAttachments[0].clearColor = color;
            pass.colorAttachments[0].loadAction  = MTLLoadActionClear;
            pass.colorAttachments[0].storeAction = MTLStoreActionStore;
            pass.colorAttachments[0].texture = surface.texture;

            id<MTLCommandBuffer> buffer = [queue commandBuffer];
            id<MTLRenderCommandEncoder> encoder = [buffer renderCommandEncoderWithDescriptor:pass];
            [encoder endEncoding];
            [buffer presentDrawable:surface];
            [buffer commit];
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
