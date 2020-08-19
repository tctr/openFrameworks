//
//  ofxiOSMTKView.h
//  iPhone+OF Static Library
//
//  Created by Dan Rosser (147) on 24/6/20.
//


#pragma once
#include <TargetConditionals.h>
#import <UIKit/UIKit.h>
#import <MetalKit/MetalKit.h>
#import <Metal/Metal.h>
#include <glm/glm.hpp>
#include "EAMTKView.h"


class ofxiOSApp;
class ofAppiOSWindow;

@interface ofxiOSMTKView : EAMTKView {

@protected
    NSMutableDictionary * activeTouches;
    glm::vec2 * screenSize;   // because glm::vec2 is forward declared,
    glm::vec2 * windowSize;   // these values have to be pointers.
    glm::vec2 * windowPos;
}

@property (readonly, nonatomic, getter=getScreenSize) glm::vec2 * screenSize;
@property (readonly, nonatomic, getter=getWindowSize) glm::vec2 * windowSize;
@property (readonly, nonatomic, getter=getWindowPosition) glm::vec2 * windowPos;

+ (ofxiOSMTKView *) getInstance;

- (id)initWithFrame:(CGRect)frame
             andApp:(ofxiOSApp *)app;
- (id)initWithFrame:(CGRect)frame
             andApp:(ofxiOSApp *)app
         sharegroup:(EAGLSharegroup *)sharegroup;
- (void)setup;
- (void)update;
- (void)draw;
- (void)setMSAA:(bool)on;
- (void)updateDimensions;
- (void)destroy;
- (CGPoint)orientateTouchPoint:(CGPoint)touchPoint;
- (void)resetTouches;
- (UIImage*)getSnapshot;

@end
