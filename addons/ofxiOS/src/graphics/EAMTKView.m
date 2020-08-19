//
//  EAGLKView.h
//  iPhone+OF Static Library
//
//  Created by Dan Rosser on 7/3/18.
//

#include <TargetConditionals.h>

#import "EAMTKView.h"

#import "ES1Renderer.h"
#import "ES2Renderer.h"

@interface EAMTKView() {
    BOOL bInit;
}
@end

@implementation EAMTKView

@synthesize delegate;
// You must implement this method
+ (Class) layerClass
{
    return [CAMetalLayer class];
}


- (id)initWithFrame:(CGRect)frame
              andAA:(bool)msaaEnabled
          andRetina:(bool)retinaEnabled
     andRetinaScale:(CGFloat)retinaScale {

    if((self = [super initWithFrame:frame])) {
        
        
        bUseMSAA = msaaEnabled;
        bUseRetina = retinaEnabled;
        //------------------------------------------------------
        scaleFactorPref = retinaScale;
        
        if(bUseRetina == YES) {
            if(scaleFactorPref == 0.0) {
                scaleFactorPref = [[UIScreen mainScreen] nativeScale]; // no scale preference, default to max scale.
            } else {
                if(scaleFactorPref < 1.0) {
                    scaleFactorPref = 1.0; // invalid negative value, default scale to 1.
                } else if(scaleFactorPref > [[UIScreen mainScreen] nativeScale]) {
                    scaleFactorPref = [[UIScreen mainScreen] nativeScale];
                }
            }
        } else {
            scaleFactorPref = 1.0;
        }
        [self updateScaleFactor];
        if(msaaEnabled)
            self.sampleCount = 4;
        else
            self.sampleCount = 1;
        
#if TARGET_OS_IOS || (TARGET_OS_IPHONE && !TARGET_OS_TV)
        self.multipleTouchEnabled = true;
#endif
        self.opaque = true;
        
        //[self bindDrawable];
        
        bInit = YES;
    }
    
    return self;
}

- (void) destroy {
    if(!bInit) {
        return;
    }
    bInit = NO;
}

- (void) dealloc{
    [self destroy];
    [super dealloc];
}

- (void) setup {
    
}
- (void) update{
    
}

- (void) draw{

}

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event { }
- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event { }
- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event { }
- (void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event { }
#ifdef __IPHONE_9_1
- (void)touchesEstimatedPropertiesUpdated:(NSSet<UITouch *> *)touches { }
#endif


//-------------------------------------------------------------------
- (void)updateScaleFactor {
    MTKView *view = (MTKView *)self;
    
    scaleFactor = MIN(scaleFactorPref, [view contentScaleFactor]);
    if(scaleFactor != self.contentScaleFactor) {
        self.contentScaleFactor = scaleFactor;
    }
}

- (void) setMSAA:(bool)on
{
//    if(on)
//        self.drawableMultisample = GLKViewDrawableMultisample4X;
//    else
//        self.drawableMultisample = GLKViewDrawableMultisampleNone;
}

//------------------------------------------------------------------- notify.
- (void) notifyAnimationStarted {
    if([self.delegate respondsToSelector:@selector(glViewAnimationStarted)]) {
        [self.delegate mtkViewAnimationStarted];
    }
}

- (void) notifyAnimationStopped {
    if([self.delegate respondsToSelector:@selector(glViewAnimationStopped)]) {
        [self.delegate mtkViewAnimationStopped];
    }
}

- (void) notifyDraw {
    if([self.delegate respondsToSelector:@selector(glViewDraw)]) {
        [self.delegate mtkViewDraw];
    }
}

- (void) notifyResized {
    if([self.delegate respondsToSelector:@selector(glViewResized)]) {
        [self.delegate mtkViewResized];
    }
}


@end
