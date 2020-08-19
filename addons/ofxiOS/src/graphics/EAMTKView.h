//
//  EAGLKView.h
//  iPhone+OF Static Library
//
//  Created by Dan Rosser on 7/3/18.
//


#pragma once

#import <UIKit/UIKit.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import "ESRenderer.h"

@protocol EAMTKViewDelegate <NSObject>
@optional
- (void)mtkViewAnimationStarted;
- (void)mtkViewAnimationStopped;
- (void)mtkViewDraw;
- (void)mtkViewResized;
@end

@interface EAMTKView : MTKView
{

@protected

    CGFloat scaleFactor;
    CGFloat scaleFactorPref;

    BOOL bUseDepth;
    BOOL bUseMSAA;
    BOOL bUseRetina;
    NSInteger msaaSamples;
}

@property (nonatomic, assign) id delegate;

- (id)initWithFrame:(CGRect)frame
              andAA:(bool)msaaEnabled
          andRetina:(bool)retinaEnabled
     andRetinaScale:(CGFloat)retinaScale;

- (void)setup;
- (void)update;
- (void)draw;
- (void)destroy;

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event;
- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event;
- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event;
- (void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event;
#ifdef __IPHONE_9_1
- (void)touchesEstimatedPropertiesUpdated:(NSSet<UITouch *> *)touches NS_AVAILABLE_IOS(9_1);
#endif
- (void)setMSAA:(bool)on;
- (void)notifyAnimationStarted;
- (void)notifyAnimationStopped;
- (void)notifyDraw;
- (void)notifyResized;

@end
