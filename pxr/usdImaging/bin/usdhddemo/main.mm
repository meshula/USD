//
// cc glfw-metal-example.m `pkg-config --cflags --libs glfw3` -framework AppKit -framework Metal -framework QuartzCore
//
#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usdGeom/xformable.h>

#include <pxr/imaging/hio/image.h>
#include <pxr/imaging/hio/imageRegistry.h>


PXR_NAMESPACE_USING_DIRECTIVE

static void quit(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}


static const MTLPixelFormat AAPLDefaultColorPixelFormat = MTLPixelFormatBGRA8Unorm;

int main(int argc, char *argv[])
{
    //-------------------------------------------------------------------------
    // USD set up
    //-------------------------------------------------------------------------
    //Plug_InitConfig();

    UsdStageRefPtr stage;
    stage = UsdStage::CreateNew("/var/tmp/cube.usda");
    // create a cube on the stage
    stage->DefinePrim(SdfPath("/Box"), TfToken("Cube"));
    UsdPrim cube = stage->GetPrimAtPath(SdfPath("/Box"));
    GfVec3f scaleVec = { 5.f, 5.f, 5.f };
    UsdGeomXformable cubeXf(cube);
    cubeXf.AddScaleOp().Set(scaleVec);
    
    //-------------------------------------------------------------------------
    // Read a texture
    //-------------------------------------------------------------------------
    const std::string stillLife = "/Users/nporcino/dev/assets/StillLife.exr";
    bool canReadExr = HioImage::IsSupportedImageFile(stillLife);
    auto image = HioImage::OpenForReading(stillLife,
                              0, // int subimage,
                              0, // int mip,
                              HioImage::SourceColorSpace::Auto,
                              false); //bool suppressErrors)
    
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
    // Render loop
    //-------------------------------------------------------------------------

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

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
