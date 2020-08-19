//  MetalRenderer.h
//  iPhone+OF Static Library
//
//  Created by Dan Rosser (147) on 25/6/20.
//

#pragma once

#import <MetalKit/MetalKit.h>

@interface MetalRenderer : NSObject<MTKViewDelegate>
{
    @private
    EAGLContext *context;

    // The pixel dimensions of the CAEAGLLayer
    int backingWidth;
    int backingHeight;

    // The OpenGL ES names for the framebuffer and renderbuffer used to render to this view
//    GLuint defaultFramebuffer, colorRenderbuffer, depthRenderbuffer;
//    GLuint msaaFrameBuffer, msaaColorRenderBuffer;

    //settings
    bool msaaEnabled;
    int msaaSamples;
    bool depthEnabled;
    bool retinaEnabled;
    bool bResize;
}


- (void)startRender;
- (void)finishRender;
- (BOOL)resizeFromLayer:(CAEAGLLayer *)layer;

- (nonnull instancetype)initWithMetalKitView:(nonnull MTKView *)mtkView;

- (NSInteger)getWidth;
- (NSInteger)getHeight;

@end
