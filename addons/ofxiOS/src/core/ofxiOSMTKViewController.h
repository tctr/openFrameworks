//
//  ofxiOSMTKViewController.h
//  iPhone+OF Static Library
//
//  Created by Dan Rosser (147) on 24/6/20.
//

#ifndef ofxiOSMTKViewController_h
#define ofxiOSMTKViewController_h

#pragma once

#include <TargetConditionals.h>
#if TARGET_OS_IOS || (TARGET_OS_IPHONE && !TARGET_OS_TV)

#import <UIKit/UIKit.h>
#import <MetalKit/MetalKit.h>

class ofxiOSApp;
@class ofxiOSMTKView;

@interface ofxiOSMTKViewController : UIViewController

@property (nonatomic, retain) ofxiOSMTKView * mtkview;
@property (nonatomic, retain) MTKView * view;

- (id)initWithFrame:(CGRect)frame app:(ofxiOSApp *)app;
- (id)initWithFrame:(CGRect)frame app:(ofxiOSApp *)app sharegroup:(EAGLSharegroup *)sharegroup;

- (UIInterfaceOrientation)currentInterfaceOrientation;
- (void)setCurrentInterfaceOrientation:(UIInterfaceOrientation) orient;
- (void)rotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
                            animated:(BOOL)animated;
- (BOOL)isReadyToRotate;
- (void)setPreferredFPS:(int)fps;
- (void)setMSAA:(bool)value;
- (EAGLSharegroup *)getSharegroup;

@end

#endif


#endif /* ofxiOSGLKViewController_h */
