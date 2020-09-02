//
//  ofxiOSMetalView.mm
//  iPhone+OF Static Library
//
//  Created by Dan Rosser (147) on 24/6/20.
//

#include "ofxiOSMetalView.h"
#import <Metal/Metal.h>
#include "ofxiOSApp.h"
#include "ofAppiOSMetalWindow.h"
#include "MetalRenderer.h"
#include "MetalRenderer.h"
#include "ofMetalProgrammableRenderer.h"
#include <TargetConditionals.h>
#import <GameController/GameController.h>

static ofxiOSMetalView * _instanceRef = nil;

@interface ofxiOSMetalView() {
    BOOL bInit;
    shared_ptr<ofAppiOSMetalWindow> window;
    shared_ptr<ofxiOSApp> app;
    CADisplayLink *_displayLink;
    NSThread *_renderThread;
    BOOL _continueRunLoop;
    BOOL bSetup;
    
}
@end

@implementation ofxiOSMetalView

@synthesize screenSize;
@synthesize windowSize;
@synthesize windowPos;

+ (ofxiOSMetalView *) getInstance {
    return _instanceRef;
}

+ (Class) layerClass
{
    return [CAMetalLayer class];
}

- (id)initWithFrame:(CGRect)frame andApp:(ofxiOSApp *)appPtr {
    
    window = dynamic_pointer_cast<ofAppiOSMetalWindow>(ofGetMainLoop()->getCurrentWindow());
    
    if(window.get() == NULL) {
        ofLog(OF_LOG_FATAL_ERROR, "ofxiOSEAGLView::initWithFrame - window is NULL");
        return nil;
    }
    

    
    self = [self initWithFrame:frame
                         andAA:window->isAntiAliasingEnabled()
                     andRetina:window->isRetinaEnabled()
                andRetinaScale:window->getRetinaScale()];
    
    bSetup = NO;
    if(self) {
        
        _instanceRef = self;
        
        app = shared_ptr<ofxiOSApp>(appPtr);
        activeTouches = [[NSMutableDictionary alloc] init];
                
        screenSize = new glm::vec2();
        windowSize = new glm::vec2();
        windowPos = new glm::vec2();
        ofSetOrientation(window->getOrientation());
        [self updateDimensions];
        
        bInit = YES;
    }
    
    return self;
}

- (void)setup {
    if(window.get() == NULL) {
        ofLog(OF_LOG_FATAL_ERROR, "ofxiOSEAGLView setup. Failed setup. window is NULL");
        return;
    }
    
    if(app.get() != ofGetAppPtr()) { // check if already running.
        
        ofSetMainLoop(shared_ptr<ofMainLoop>(NULL)); // destroy old main loop.
        auto mainLoop = std::make_shared<ofMainLoop>(); // make new main loop.
        ofSetMainLoop(mainLoop);
        
        ofiOSMetalWindowSettings windowSettings = window->getSettings();
        window = NULL;

        window = dynamic_pointer_cast<ofAppiOSMetalWindow>(ofCreateWindow(windowSettings));

        ofRunApp(app);
    }
    
    if(window->isProgrammableRenderer() == true) {
        static_cast<ofMetalProgrammableRenderer*>(window->renderer().get())->setup(window->getSettings());
   
    
    ofxiOSAlerts.addListener(app.get());
    
    ofDisableTextureEdgeHack();
    
    window->events().notifySetup();
    bSetup = YES;
    window->renderer()->clear();
}

- (void)destroy {
    if(!bInit) {
        return;
    }

    window->events().notifyExit();
    
    ofxiOSAlerts.removeListener(app.get());

    ofGetMainLoop()->exit();
    
    app = NULL;
    window = NULL;
    
    [activeTouches release];
    
    delete screenSize;
    screenSize = NULL;
    delete windowSize;
    windowSize = NULL;
    delete windowPos;
    windowPos = NULL;
    
    _instanceRef = nil;
    
    bInit = NO;
    
    [super destroy];
}

- (void)dealloc {
    [self destroy];
    [super dealloc];
}

- (void)layoutSubviews {
    [super layoutSubviews];
    [self resizeDrawable:self.window.screen.nativeScale];
    [self updateDimensions];
    
    [super notifyResized];
    window->events().notifyWindowResized(ofGetWidth(), ofGetHeight());
}

- (void)updateDimensions {
    *windowPos = glm::vec2(self.frame.origin.x * scaleFactor, self.frame.origin.y * scaleFactor);
    *windowSize = glm::vec2(self.bounds.size.width * scaleFactor, self.bounds.size.height * scaleFactor);
    
    UIScreen * currentScreen = self.window.screen;  // current screen is the screen that GLView is attached to.
    if(!currentScreen) {                            // if GLView is not attached, assume to be main device screen.
        currentScreen = [UIScreen mainScreen];
    }
    *screenSize = glm::vec2(currentScreen.bounds.size.width * scaleFactor, currentScreen.bounds.size.height * scaleFactor);
}

- (void) setMSAA:(bool)on
{
    [self setMSAA:on sampleCount:4];
}

- (void)setPreferredFPS:(int)fps {
    _displayLink.preferredFramesPerSecond = fps;
}

- (void) setMSAA:(bool)on sampleCount:(int)sampeles
{
    if(on)
        self.sampleCount = sampeles;
    else
        self.sampleCount = 1;
    _displayLink.anti
}

- (void)notifyResized {
    // blank this.
    // we want to notifyResized at the end of layoutSubviews.
}

- (void)update {
    if(bSetup == NO) return;
    
    window->events().notifyUpdate();
}


- (void)draw {
    if(bSetup == NO) return;
    
    window->renderer()->startRender();
    
    if(window->isSetupScreenEnabled()) {
        window->renderer()->setupScreen();
    }
    
    //------------------------------------------ draw.
    
    window->events().notifyDraw();
    
    //------------------------------------------
    
    window->renderer()->finishRender();
    
    [super notifyDraw];   // alerts delegate that a new frame has been drawn.
}


- (void)notifyDraw {
    // blank this.
    // we want to notifyDraw at the end of drawView.
}

//------------------------------------------------------
- (CGPoint)orientateTouchPoint:(CGPoint)touchPoint {
    
    if(ofAppiOSWindow::getInstance()->doesHWOrientation()) {
        return touchPoint;
    }
    
    ofOrientation orientation = ofGetOrientation();
    CGPoint touchPointOriented = CGPointZero;
    
    switch(orientation) {
        case OF_ORIENTATION_180:
            touchPointOriented.x = ofGetWidth() - touchPoint.x;
            touchPointOriented.y = ofGetHeight() - touchPoint.y;
            break;
            
        case OF_ORIENTATION_90_RIGHT:
            touchPointOriented.x = touchPoint.y;
            touchPointOriented.y = ofGetHeight() - touchPoint.x;
            break;
            
        case OF_ORIENTATION_90_LEFT:
            touchPointOriented.x = ofGetWidth() - touchPoint.y;
            touchPointOriented.y = touchPoint.x;
            break;
            
        case OF_ORIENTATION_DEFAULT:
        default:
            touchPointOriented = touchPoint;
            break;
    }
    return touchPointOriented;
}

//------------------------------------------------------

-(void) resetTouches {

    [activeTouches removeAllObjects];
}

- (void)touchesBegan:(NSSet *)touches 
           withEvent:(UIEvent *)event{
    
    if(!bInit || !bSetup) {
        // if the glView is destroyed which also includes the OF app,
        // we no longer need to pass on these touch events.
        return; 
    }
    
    for(UITouch *touch in touches) {
        int touchIndex = 0;
        while([[activeTouches allValues] containsObject:[NSNumber numberWithInt:touchIndex]]){
            touchIndex++;
        }
        
        [activeTouches setObject:[NSNumber numberWithInt:touchIndex] forKey:[NSValue valueWithPointer:touch]];
        
        CGPoint touchPoint = [touch locationInView:self];
        
        touchPoint.x *= scaleFactor; // this has to be done because retina still returns points in 320x240 but with high percision
        touchPoint.y *= scaleFactor;
        touchPoint = [self orientateTouchPoint:touchPoint];
        
        if( touchIndex==0 ){
            window->events().notifyMousePressed(touchPoint.x, touchPoint.y, 0);
        }
        
        ofTouchEventArgs touchArgs;
        touchArgs.numTouches = [[event touchesForView:self] count];
        touchArgs.x = touchPoint.x;
        touchArgs.y = touchPoint.y;
        touchArgs.id = touchIndex;
        if([touch tapCount] == 2){
            touchArgs.type = ofTouchEventArgs::doubleTap;
            ofNotifyEvent(window->events().touchDoubleTap,touchArgs);   // send doubletap
        }
        touchArgs.type = ofTouchEventArgs::down;
        ofNotifyEvent(window->events().touchDown,touchArgs);    // but also send tap (upto app programmer to ignore this if doubletap came that frame)
    }   
}

//------------------------------------------------------
- (void)touchesMoved:(NSSet *)touches 
           withEvent:(UIEvent *)event{
    
    if(!bInit || !bSetup) {
        // if the glView is destroyed which also includes the OF app,
        // we no longer need to pass on these touch events.
        return; 
    }
    
    for(UITouch *touch in touches){
        int touchIndex = [[activeTouches objectForKey:[NSValue valueWithPointer:touch]] intValue];
        
        CGPoint touchPoint = [touch locationInView:self];
        
        touchPoint.x *= scaleFactor; // this has to be done because retina still returns points in 320x240 but with high percision
        touchPoint.y *= scaleFactor;
        touchPoint = [self orientateTouchPoint:touchPoint];
        
        if( touchIndex==0 ){
            window->events().notifyMouseDragged(touchPoint.x, touchPoint.y, 0);
        }       
        ofTouchEventArgs touchArgs;
        touchArgs.numTouches = [[event touchesForView:self] count];
        touchArgs.x = touchPoint.x;
        touchArgs.y = touchPoint.y;
        touchArgs.id = touchIndex;
        touchArgs.type = ofTouchEventArgs::move;
        ofNotifyEvent(window->events().touchMoved, touchArgs);
    }
}

//------------------------------------------------------
- (void)touchesEnded:(NSSet *)touches 
           withEvent:(UIEvent *)event{
    
    if(!bInit || !bSetup) {
        // if the glView is destroyed which also includes the OF app,
        // we no longer need to pass on these touch events.
        return; 
    }
    
    for(UITouch *touch in touches){
        int touchIndex = [[activeTouches objectForKey:[NSValue valueWithPointer:touch]] intValue];
        
        [activeTouches removeObjectForKey:[NSValue valueWithPointer:touch]];
        
        CGPoint touchPoint = [touch locationInView:self];
        
        touchPoint.x *= scaleFactor; // this has to be done because retina still returns points in 320x240 but with high percision
        touchPoint.y *= scaleFactor;
        touchPoint = [self orientateTouchPoint:touchPoint];
        
        if( touchIndex==0 ){
            window->events().notifyMouseReleased(touchPoint.x, touchPoint.y, 0);
        }
        
        ofTouchEventArgs touchArgs;
        touchArgs.numTouches = [[event touchesForView:self] count] - [touches count];
        touchArgs.x = touchPoint.x;
        touchArgs.y = touchPoint.y;
        touchArgs.id = touchIndex;
        touchArgs.type = ofTouchEventArgs::up;
        ofNotifyEvent(window->events().touchUp, touchArgs);
    }
}

//------------------------------------------------------
- (void)touchesCancelled:(NSSet *)touches 
               withEvent:(UIEvent *)event{
    
    if(!bInit || !bSetup) {
        // if the glView is destroyed which also includes the OF app,
        // we no longer need to pass on these touch events.
        return; 
    }
    
    for(UITouch *touch in touches){
        int touchIndex = [[activeTouches objectForKey:[NSValue valueWithPointer:touch]] intValue];
        
        CGPoint touchPoint = [touch locationInView:self];
        
        touchPoint.x *= scaleFactor; // this has to be done because retina still returns points in 320x240 but with high percision
        touchPoint.y *= scaleFactor;
        touchPoint = [self orientateTouchPoint:touchPoint];
        
        ofTouchEventArgs touchArgs;
        touchArgs.numTouches = [[event touchesForView:self] count];
        touchArgs.x = touchPoint.x;
        touchArgs.y = touchPoint.y;
        touchArgs.id = touchIndex;
        touchArgs.type = ofTouchEventArgs::cancel;
        ofNotifyEvent(window->events().touchCancelled, touchArgs);
    }
    
    [self touchesEnded:touches withEvent:event];
}

- (UIImage*)getSnapshot {
    return nil;
}

- (void)setPaused:(BOOL)paused
{
    super.paused = paused;

    _displayLink.paused = paused;
}

- (void)didMoveToWindow
{
    [super didMoveToWindow];
    
    if(self.window == nil)
        {
            // If moving off of a window destroy the display link.
            [_displayLink invalidate];
            _displayLink = nil;
            return;
        }

        [self setupCADisplayLinkForScreen:self.window.screen];


        // CADisplayLink callbacks are associated with an 'NSRunLoop'. The currentRunLoop is the
        // the main run loop (since 'didMoveToWindow' is always executed from the main thread.
        [_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
    
        [self resizeDrawable:self.window.screen.nativeScale];

}

- (void)setupCADisplayLinkForScreen:(UIScreen*)screen
{
    [self stopRenderLoop];

    _displayLink = [screen displayLinkWithTarget:self selector:@selector(render)];

    _displayLink.paused = self.paused;

    _displayLink.preferredFramesPerSecond = 60;
}

- (void)didEnterBackground:(NSNotification*)notification
{
    self.paused = YES;
}

- (void)willEnterForeground:(NSNotification*)notification
{
    self.paused = NO;
}

- (void)stopRenderLoop
{
    [_displayLink invalidate];
}

- (void)setContentScaleFactor:(CGFloat)contentScaleFactor
{
    [super setContentScaleFactor:contentScaleFactor];
    [self resizeDrawable:self.window.screen.nativeScale];
}

- (void)setFrame:(CGRect)frame
{
    [super setFrame:frame];
    [self resizeDrawable:self.window.screen.nativeScale];
}

- (void)setBounds:(CGRect)bounds
{
    [super setBounds:bounds];
    [self resizeDrawable:self.window.screen.nativeScale];
}


@end
