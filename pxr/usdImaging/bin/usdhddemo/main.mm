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

#include "pxr/imaging/glf/drawTarget.h"

#include "pxr/imaging/garch/glApi.h"

#include <iostream>



GLFWwindow *window = NULL;


PXR_NAMESPACE_OPEN_SCOPE


using std::set;
using std::vector;

int viewWidth = 512, viewHeight = 512;
float cameraAngleX=45;
float cameraAngleY=45;
float cameraDistance=1;

std::string colorImageFormat = "png";

#if 1
void
InitLights()
{
    GLfloat lightKa[] = {0.1f, 0.1f, 0.1f, 1.0f};  // ambient light
    GLfloat lightKd[] = {0.9f, 0.9f, 0.9f, 1.0f};  // diffuse light
    GLfloat lightKs[] = {1.0f, 1.0f, 1.0f, 1.0f};           // specular light
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightKa);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightKd);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightKs);

    // position the light
    float lightPos[4] = {-15, 15, 0, 1}; // positional light
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    glEnable(GL_LIGHT0);                        // MUST enable each light source after configuration
}

void
DrawSphere( GfVec3f diffuseColor )
{
    float shininess = 15.0f;
    float specularColor[4] = {1.00000f, 0.980392f, 0.549020f, 1.0f};

    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularColor);

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3fv( (GLfloat *)(&diffuseColor));

    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    
    glFrontFace(GL_CW);
    
    // Draw a sphere
    int Long = 48;
    int Lat = 64;
    float radius = 2.0;
    double longStep = (M_PI/Long);
    double latStep = (2.0 * M_PI / Lat);

    for (int i = 0; i < Long; ++i) {
        double a = i * longStep;
        double b = a + longStep;
        double r0 = radius * sin(a);
        double r1 = radius * sin(b);
        GLfloat z0 = radius * cos(a);
        GLfloat z1 = radius * cos(b);

        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= Lat; ++j) {
            double c = j * latStep;
            GLfloat x = cos(c);
            GLfloat y = sin(c);

            glNormal3f((x * r0) / radius, (y * r0) / radius, z0 / radius);
            glTexCoord2f(j / (GLfloat) Lat, i / (GLfloat) Long);
            glVertex3f(x * r0, y * r0, z0);

            glNormal3f((x * r1) / radius, (y * r1) / radius, z1 / radius);
            glTexCoord2f(j / (GLfloat) Lat, (i + 1) / (GLfloat) Long);
            glVertex3f(x * r1, y * r1, z1);
        }
        glEnd();
    }

    glFrontFace(GL_CCW);
}

void
TestGlfDrawTarget()
{
    GlfDrawTargetRefPtr
        dt = GlfDrawTarget::New( GfVec2i( viewWidth, viewHeight ) );
    TF_AXIOM(dt->GetFramebufferId() != 0);

    {
        TF_AXIOM(dt->IsBound()==false);

        dt->Bind();
        TF_AXIOM(dt->IsBound());
        TF_AXIOM(!dt->IsValid());
        TF_AXIOM(dt->GetSize()==GfVec2i(viewWidth,viewHeight));

        dt->Unbind();
        TF_AXIOM(!dt->IsBound());
    }
    
    GlfDrawTarget::AttachmentsMap const & aovs = dt->GetAttachments();
    GlfDrawTarget::AttachmentsMap::const_iterator it;
    
    {
        dt->Bind();
        TF_AXIOM(dt->IsBound());

        dt->AddAttachment( "color", GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA );
        dt->AddAttachment( "depth", GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_COMPONENT32F );
        TF_AXIOM(dt->IsValid());
        TF_AXIOM(aovs.size()==2);

        it = aovs.find("color");
        TF_AXIOM(it!=aovs.end());
        TF_AXIOM(it->second->GetGlTextureName()!=0);
        TF_AXIOM(it->second->GetFormat()==GL_RGBA);
        TF_AXIOM(it->second->GetType()==GL_UNSIGNED_BYTE);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
        glEnable(GL_CULL_FACE);
        glEnable(GL_COLOR_MATERIAL);

        glClearColor(0.5,0.5,0.5,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        InitLights();

        glViewport(0, 0, viewWidth, viewHeight);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        // Replacement for gluPerspective
        GLdouble fovY = 60.0f;
        GLdouble aspect = (float) viewWidth / (float) viewHeight;
        GLdouble zNear = 1.0;
        GLdouble zFar = 100.0;

        GLdouble fH = tan( fovY / 360.0 * M_PI ) * zNear;
        GLdouble fW = fH * aspect;
        glFrustum( -fW, fW, -fH, fH, zNear, zFar );

        glMatrixMode(GL_MODELVIEW);

        glTranslatef(0, 0.0f, -10.0f);
        glRotatef( -45.0f, 0, 1, 0);
        glRotatef( -45.0f, 0, 0, 1);

        DrawSphere( GfVec3f(1.0, 0.5, 0.5) );

        dt->WriteToFile("color", "testGlfDrawTarget_colorAOV_512x512." + colorImageFormat);

        dt->Unbind();
        TF_AXIOM(!dt->IsBound());
    }

    it = aovs.find("color");
    TF_AXIOM(it!=aovs.end());
    size_t initialContentsID = it->second->GetContentsID();
    TF_AXIOM(initialContentsID!=0);

    {
        dt->Bind();
        TF_AXIOM(dt->IsBound());

        dt->SetSize( GfVec2i(256,256) );
        TF_AXIOM(dt->IsValid());
        TF_AXIOM(dt->GetSize()==GfVec2i(256,256));

        glClearColor(0.5,0.5,0.5,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glViewport(0, 0, 256, 256);

        DrawSphere( GfVec3f(0.5, 1.0, 0.5) );

        dt->WriteToFile(
            "color", "testGlfDrawTarget_colorAOV_256x256." + colorImageFormat);

        dt->Unbind();
        TF_AXIOM(!dt->IsBound());
    }

    size_t secondContentsID = it->second->GetContentsID();
    TF_AXIOM(secondContentsID!=0);
    TF_AXIOM(initialContentsID!=secondContentsID);

    {
        dt->Bind();
        TF_AXIOM(dt->IsBound());

        dt->ClearAttachments( );
        TF_AXIOM(aovs.size()==0);
        TF_AXIOM(!dt->IsValid());

        dt->AddAttachment( "float_color", GL_RGBA, GL_FLOAT, GL_RGBA32F );
        dt->AddAttachment( "depth", GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_COMPONENT32F );
        TF_AXIOM(dt->IsValid());
        TF_AXIOM(aovs.size()==2);

        it = aovs.find("float_color");
        TF_AXIOM(it!=aovs.end());
        TF_AXIOM(it->second->GetGlTextureName()!=0);
        TF_AXIOM(it->second->GetFormat()==GL_RGBA);
        TF_AXIOM(it->second->GetType()==GL_FLOAT);

        glClearColor(0.5,0.5,0.5,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glViewport(0, 0, 256, 256);

        DrawSphere( GfVec3f(0.0, 0.5, 1.0) );

        dt->WriteToFile("float_color", "testGlfDrawTarget_floatColorAOV_256x256." + colorImageFormat);
        dt->WriteToFile("depth", "testGlfDrawTarget_floatDepthAOV_256x256.zfile");

        dt->Unbind();
        TF_AXIOM(!dt->IsBound());
    }

    size_t thirdContentsID = it->second->GetContentsID();
    TF_AXIOM(thirdContentsID!=0);
    TF_AXIOM(secondContentsID!=thirdContentsID);
}


#endif













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

PXR_NAMESPACE_CLOSE_SCOPE


#define INSTALL_LOCN "usd0727"

PXR_NAMESPACE_USING_DIRECTIVE

int main(int argc, char *argv[])
{
    //-------------------------------------------------------------------------
    // USD set up
    //-------------------------------------------------------------------------
    //Plug_InitConfig();
    std::vector<std::string> test;
    std::vector<std::string> paths;
    paths.emplace_back("/var/tmp/" INSTALL_LOCN "/lib/usd");
    paths.emplace_back("/var/tmp/" INSTALL_LOCN "/plugin/usd");
    std::vector<std::string> msgs;
    msgs.emplace_back("looking for plugs here: /var/tmp/" INSTALL_LOCN "/lib/usd");
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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    //glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    /*
    glfwWindowHint(GLFW_FLOAT_PIXEL_TYPE, GLFW_TRUE);
    glfwWindowHint(GLFW_RED_BITS, 16);
    glfwWindowHint(GLFW_GREEN_BITS, 16);
    glfwWindowHint(GLFW_BLUE_BITS, 16);
    glfwWindowHint(GLFW_ALPHA_BITS, 16);
    */
    
    window = glfwCreateWindow(640, 480, "USD Hydra demo", NULL, NULL);
    NSWindow *nswindow = glfwGetCocoaWindow(window);
    nswindow.contentView.layer = swapchain;
    nswindow.contentView.wantsLayer = YES;

    int bail = 20;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        usleep(1000);
        --bail;
        if (!bail)
            break;
    }

    if (false) {
        colorImageFormat = "exr"; // "png"
        
        glfwMakeContextCurrent(window);
        GarchGLApiLoad();
        GLint numExtensions = 0;
        glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
        TestGlfDrawTarget();
        return EXIT_SUCCESS;
    }
    
  
    
    
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
    //const std::string stillLife = "/Users/nporcino/dev/assets/textures/apollo.jpeg";
    //const std::string stillLife = "/Users/nporcino/dev/assets/textures/Incredibles.exr";
    const std::string stillLife = "/Users/nporcino/dev/assets/testCards/WoodPlanksDonut.exr";
    //const std::string stillLife = "/Users/nporcino/dev/assets/textures/StillLife.exr";
    //const std::string stillLife = "/Users/nporcino/dev/assets/textures/GoldenGate.exr";
    //const std::string stillLife = "/Users/nporcino/dev/assets/textures/out-dwaa.exr";
    HioImageSharedPtr _image;
    bool canReadExr = HioImage::IsSupportedImageFile(stillLife);
    std::unique_ptr<char[]> imageData;
    HioImage::StorageSpec _spec;
    /*
    bool
    HdStTextureUtils::ReadAndConvertImage(
        HioImageSharedPtr const &image,
        const bool flipped,
        const bool premultiplyAlpha,
        const HgiMipInfo &mipInfo,
        const size_t layer,
        void * const bufferStart)
     */
    if (canReadExr) {
        _image = HioImage::OpenForReading(stillLife,
                                          0, // int subimage,
                                          0, // int mip,
                                          HioImage::SourceColorSpace::Auto,
                                          false); //bool suppressErrors)
    }
    if (_image) {
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
            case HioFormatUNorm8srgb:
                std::cout << " format: HioFormatUNorm8srgb\n"; break;
            case HioFormatUNorm8Vec2srgb:
                std::cout << " format: HioFormatUNorm8Vec2srgb\n"; break;
            case HioFormatUNorm8Vec3srgb:
                std::cout << " format: HioFormatUNorm8Vec3srgb\n"; break;
            case HioFormatUNorm8Vec4srgb:
                std::cout << " format: HioFormatUNorm8Vec4srgb\n"; break;
            default:
                std::cout << " format: " << _image->GetFormat() << "\n"; break;
        }
        std::cout << " bytes per pixel: " << _image->GetBytesPerPixel() << "\n";
        std::cout << " mips: " << _image->GetNumMipLevels() << "\n";
        std::cout << (_image->IsColorSpaceSRGB() ? " srgb pixels\n" : " linear pixels\n");

        const bool premultiplyAlpha = false;
        auto hgiFormat = HdStTextureUtils::GetHgiFormat(
            _image->GetFormat(),
            premultiplyAlpha);

        const int layerCount = 1;
        const GfVec3i dimensions((int)_image->GetWidth(), (int)_image->GetHeight(), 1);
        const std::vector<HgiMipInfo> mipInfos =
            HgiGetMipInfos(hgiFormat, dimensions, layerCount);

        int cropTop = 0;
        int cropBottom = 0;
        float scale = 1.f;
        _spec.width  = (int)(_image->GetWidth() * scale);
        _spec.height = (int)((_image->GetHeight() - cropTop - cropBottom) * scale);
        _spec.format = HioFormatFloat16Vec4; // _image->GetFormat();
        _spec.flipped = false;

        size_t bufsize = _spec.width * _spec.height * HioGetDataSizeOfType(_spec.format) * HioGetComponentCount(_spec.format);
        imageData.reset(new char[bufsize]);
        _spec.data = imageData.get();
                
        if (_image->ReadCropped(cropTop, cropBottom, 0, 0, _spec)) {
            // successfully read the image!
            if (_spec.format == HioFormatUNorm8Vec3srgb) {
                _spec.format = HioFormatUNorm8Vec4srgb;     // force reading as rgba
                bufsize = _spec.width * _spec.height * HioGetDataSizeOfType(_spec.format) * HioGetComponentCount(_spec.format);
                char* newData = new char[bufsize];
                uint8_t* dst = (uint8_t*) newData;
                uint8_t* src = (uint8_t*) _spec.data;
                for (int i = 0; i < _spec.width * _spec.height * 3; i += 3) {
                    *dst++ = src[i];
                    *dst++ = src[i+1];
                    *dst++ = src[i+2];
                    *dst++ = 255;
                }
                imageData.reset(newData);
                _spec.data = imageData.get();
            }
        }
        const char* output = "/var/tmp/test_exr.exr";
        auto writeImage = HioImage::OpenForWriting(output);
        writeImage->Write(_spec);
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
        textureDesc.pixelsByteSize = HioGetDataSizeOfType(_spec.format) * HioGetComponentCount(_spec.format);
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

