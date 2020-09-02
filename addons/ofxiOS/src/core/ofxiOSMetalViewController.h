//
//  ofxiOSMetalViewController.h
//  iPhone+OF Static Library
//
//  Created by Dan Rosser (147) on 24/6/20.
//

#ifndef ofxiOSMetalViewController_h
#define ofxiOSMetalViewController_h

#pragma once

#include <TargetConditionals.h>
#if TARGET_OS_IOS || (TARGET_OS_IPHONE && !TARGET_OS_TV)

#import <UIKit/UIKit.h>
#import <MetalKit/MetalKit.h>
#import "ofxMetalViewController.h"

class ofxiOSApp;
@class ofxiOSMetalView;

@interface ofxiOSMetalViewController : ofxMetalViewController

@property (nonatomic, retain) ofxiOSMetalView * metalView;
@property (nonatomic, retain) MTKView * view;

- (id)initWithFrame:(CGRect)frame app:(ofxiOSApp *)app;

- (UIInterfaceOrientation)currentInterfaceOrientation;
- (void)setCurrentInterfaceOrientation:(UIInterfaceOrientation) orient;
- (void)rotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
                            animated:(BOOL)animated;
- (BOOL)isReadyToRotate;
- (void)setPreferredFPS:(int)fps;
- (void)setMSAA:(bool)value;


@end

#endif


#endif /* ofxiOSGLKViewController_h */
