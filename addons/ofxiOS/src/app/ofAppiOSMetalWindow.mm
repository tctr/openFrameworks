//
//  ofxiOSMetalView.h
//  iPhone+OF Static Library
//
//  Created by Dan Rosser (147) on 24/6/20.
//
#include <TargetConditionals.h>
#include "ofAppiOSMetalWindow.h"
#include "ofMetalProgrammableRenderer.h"
//#include "ofGLRenderer.h"
//#include "ofGLRenderer.h"
//#include "ofGLProgrammableRenderer.h"
#include "ofMetalProgrammableRenderer.h"
#if TARGET_OS_IOS || (TARGET_OS_IPHONE && !TARGET_OS_TV)
    #include "ofxiOSAppDelegate.h"
    #include "ofxiOSViewController.h"
    const std::string appDelegateName = "ofxiOSAppDelegate";
#elif TARGET_OS_TV
    #include "ofxtvOSAppDelegate.h"
    #include "ofxtvOSViewController.h"
    const std::string appDelegateName = "ofxtvOSAppDelegate";
#endif
#include "ofxiOSMetalView.h"
#include "ofxiOSGLKView.h"
#include "ofxiOSEAGLView.h"

//----------------------------------------------------------------------------------- instance.
static ofAppiOSMetalWindow * _instance = NULL;
ofAppiOSMetalWindow * ofAppiOSMetalWindow::getInstance() {
	return _instance;
}

//----------------------------------------------------------------------------------- constructor / destructor.
ofAppiOSMetalWindow::ofAppiOSMetalWindow() : hasExited(false) {
	if(_instance == NULL) {
        _instance = this;
    } else {
        ofLog(OF_LOG_ERROR, "ofAppiOSMetalWindow instantiated more than once");
    }
    bRetinaSupportedOnDevice = false;
    bRetinaSupportedOnDeviceChecked = false;
}

ofAppiOSMetalWindow::~ofAppiOSMetalWindow() {
    close();
	_instance = NULL;
}

void ofAppiOSMetalWindow::close() {
    if(hasExited == false){
        hasExited = true;
        events().notifyExit();
        events().disable();
    }
}

void ofAppiOSMetalWindow::setup(const ofWindowSettings & _settings) {
    const ofiOSMetalWindowSettings * metalSettings = dynamic_cast<const ofiOSMetalWindowSettings*>(&_settings);
    if(metalSettings){
        setup(*metalSettings);
    } else{
       
    }
}

void ofAppiOSMetalWindow::setup(const ofMetalWindowSettings & _settings) {
    const ofiOSMetalWindowSettings * metalSettings = dynamic_cast<const ofiOSMetalWindowSettings*>(&_settings);
    if(settings.setupOrientation == OF_ORIENTATION_UNKNOWN) {
        settings.setupOrientation = OF_ORIENTATION_DEFAULT;
    }
    setOrientation(settings.setupOrientation);
    
    currentRenderer = std::shared_ptr<ofBaseRenderer>(new ofMetalProgrammableRenderer(this));
    
    
    hasExited = false;
}


void ofAppiOSMetalWindow::setup() {
	
	
	if(settings.setupOrientation == OF_ORIENTATION_UNKNOWN) {
		settings.setupOrientation = OF_ORIENTATION_DEFAULT;
	}
	setOrientation(settings.setupOrientation);
	
    currentRenderer = std::shared_ptr<ofBaseRenderer>(new ofMetalProgrammableRenderer(this));
	
	
	hasExited = false;
}


void ofAppiOSMetalWindow::loop() {
    startAppWithDelegate(appDelegateName);
}

void ofAppiOSMetalWindow::run(ofBaseApp * appPtr){
    startAppWithDelegate(appDelegateName);
}

void ofAppiOSMetalWindow::startAppWithDelegate(std::string appDelegateClassName) {
    static bool bAppCreated = false;
    if(bAppCreated == true) {
        return;
    }
    bAppCreated = true;
    
    @autoreleasepool {
        UIApplicationMain(0, nil, nil, [NSString stringWithUTF8String:appDelegateClassName.c_str()]);
    }
}

void ofAppiOSMetalWindow::update() {
    
}
void ofAppiOSMetalWindow::draw() {
    
}


//----------------------------------------------------------------------------------- cursor.
void ofAppiOSMetalWindow::hideCursor() {
    // not supported on iOS.
}

void ofAppiOSMetalWindow::showCursor() {
    // not supported on iOS.
}

//----------------------------------------------------------------------------------- window / screen properties.
void ofAppiOSMetalWindow::setWindowPosition(int x, int y) {
	// not supported on iOS.
}

void ofAppiOSMetalWindow::setWindowShape(int w, int h) {
	// not supported on iOS.
}

glm::vec2	ofAppiOSMetalWindow::getWindowPosition() {
	if(settings.windowControllerType == METAL_KIT)
        return *[[ofxiOSMetalView getInstance] getWindowPosition];
    else if(settings.windowControllerType == GL_KIT)
		return *[[ofxiOSGLKView getInstance] getWindowPosition];
	else
		return *[[ofxiOSEAGLView getInstance] getWindowPosition];
}

glm::vec2	ofAppiOSMetalWindow::getWindowSize() {
        return *[[ofxiOSMetalView getInstance] getWindowSize];
}

glm::vec2	ofAppiOSMetalWindow::getScreenSize() {

        return *[[ofxiOSMetalView getInstance] getWindowSize];

}

int ofAppiOSMetalWindow::getWidth(){
	if(settings.enableHardwareOrientation == true ||
       orientation == OF_ORIENTATION_DEFAULT ||
       orientation == OF_ORIENTATION_180) {
		return (int)getWindowSize().x;
	}
	return (int)getWindowSize().y;
}

int ofAppiOSMetalWindow::getHeight(){
	if(settings.enableHardwareOrientation == true ||
       orientation == OF_ORIENTATION_DEFAULT ||
       orientation == OF_ORIENTATION_180) {
		return (int)getWindowSize().y;
	}
	return (int)getWindowSize().x;
}

ofWindowMode ofAppiOSMetalWindow::getWindowMode() {
	return settings.windowMode;
}

#if TARGET_OS_IOS || (TARGET_OS_IPHONE && !TARGET_OS_TV)
//----------------------------------------------------------------------------------- orientation.
void ofAppiOSMetalWindow::setOrientation(ofOrientation toOrientation) {
    if(orientation == toOrientation) {
        return;
    }
    bool bOrientationPortraitOne = (orientation == OF_ORIENTATION_DEFAULT) || (orientation == OF_ORIENTATION_180);
    bool bOrientationPortraitTwo = (toOrientation == OF_ORIENTATION_DEFAULT) || (toOrientation == OF_ORIENTATION_180);
    bool bResized = bOrientationPortraitOne != bOrientationPortraitTwo;

    orientation = toOrientation;
#if TARGET_OS_IOS || (TARGET_OS_IPHONE && !TARGET_OS_TV)
    UIInterfaceOrientation interfaceOrientation = UIInterfaceOrientationPortrait;
    switch (orientation) {
        case OF_ORIENTATION_DEFAULT:
            interfaceOrientation = UIInterfaceOrientationPortrait;
            break;
        case OF_ORIENTATION_180:
            interfaceOrientation = UIInterfaceOrientationPortraitUpsideDown;
            break;
        case OF_ORIENTATION_90_RIGHT:
            interfaceOrientation = UIInterfaceOrientationLandscapeLeft;
            break;
        case OF_ORIENTATION_90_LEFT:
            interfaceOrientation = UIInterfaceOrientationLandscapeRight;
            break;
    }


    id<UIApplicationDelegate> appDelegate = [UIApplication sharedApplication].delegate;
    if([appDelegate respondsToSelector:@selector(glViewController)] == NO) {
        // check app delegate has glViewController,
        // otherwise calling glViewController will cause a crash.
        return;
    }
//    UIViewController * uiViewController = ((ofxiOSAppDelegate *)appDelegate).uiViewController;
//	if([uiViewController isKindOfClass:[ofxiOSViewController class]] == YES) {
//		ofxiOSViewController * glViewController = (ofxiOSMetalViewController*)uiViewController;
//		if(glViewController) {
//			ofxiOSMetalView * glView = mtkViewController.glView;
//			if(settings.enableHardwareOrientation == true) {
//				[glViewController rotateToInterfaceOrientation:interfaceOrientation animated:settings.enableHardwareOrientationAnimation];
//			} else {
//				[[UIApplication sharedApplication] setStatusBarOrientation:interfaceOrientation animated:settings.enableHardwareOrientationAnimation];
//				if(bResized == true) {
//					[glView layoutSubviews]; // calling layoutSubviews so window resize notification is fired.
//				}
//			}
//		}
//	}
#endif
}

ofOrientation ofAppiOSMetalWindow::getOrientation() {
	return orientation;
}

bool ofAppiOSMetalWindow::doesHWOrientation() {
    return settings.enableHardwareOrientation;
}

//-----------------------------------------------------------------------------------
bool ofAppiOSMetalWindow::enableHardwareOrientation() {
    return (settings.enableHardwareOrientation = true);
}

bool ofAppiOSMetalWindow::disableHardwareOrientation() {
    return (settings.enableHardwareOrientation = false);
}

bool ofAppiOSMetalWindow::enableOrientationAnimation() {
    return (settings.enableHardwareOrientationAnimation = true);
}

bool ofAppiOSMetalWindow::disableOrientationAnimation() {
    return (settings.enableHardwareOrientationAnimation = false);
}

#endif

//-----------------------------------------------------------------------------------
void ofAppiOSMetalWindow::setWindowTitle(std::string title) {
    // not supported on iOS.
}

void ofAppiOSMetalWindow::setFullscreen(bool fullscreen) {
#if TARGET_OS_IOS || (TARGET_OS_IPHONE && !TARGET_OS_TV)
    [[UIApplication sharedApplication] setStatusBarHidden:fullscreen withAnimation:UIStatusBarAnimationSlide];
#endif
    if(fullscreen) {
        settings.windowMode = OF_FULLSCREEN;
    } else {
        settings.windowMode = OF_WINDOW;
    }
}

void ofAppiOSMetalWindow::toggleFullscreen() {
    if(settings.windowMode == OF_FULLSCREEN) {
        setFullscreen(false);
    } else {
        setFullscreen(true);
    }
}

bool ofAppiOSMetalWindow::isProgrammableRenderer() {
    return true;
}

//-----------------------------------------------------------------------------------
void ofAppiOSMetalWindow::enableSetupScreen() {
	settings.enableSetupScreen = true;
};

void ofAppiOSMetalWindow::disableSetupScreen() {
	settings.enableSetupScreen = false;
};

bool ofAppiOSMetalWindow::isSetupScreenEnabled() {
    return settings.enableSetupScreen;
}

void ofAppiOSMetalWindow::setVerticalSync(bool enabled) {
    // not supported on iOS.
}

//----------------------------------------------------------------------------------- retina.
bool ofAppiOSMetalWindow::enableRetina(float retinaScale) {
    if(isRetinaSupportedOnDevice()) {
        settings.enableRetina = true;
        settings.retinaScale = retinaScale;
    }
    return settings.enableRetina;
}

bool ofAppiOSMetalWindow::disableRetina() {
    return (settings.enableRetina = false);
}

bool ofAppiOSMetalWindow::isRetinaEnabled() {
    return settings.enableRetina;
}

bool ofAppiOSMetalWindow::isRetinaSupportedOnDevice() {
    if(bRetinaSupportedOnDeviceChecked) {
        return bRetinaSupportedOnDevice;
    }
    
    bRetinaSupportedOnDeviceChecked = true;
    
    @autoreleasepool {
        if([[UIScreen mainScreen] respondsToSelector:@selector(scale)]){
            if ([[UIScreen mainScreen] scale] > 1){
                bRetinaSupportedOnDevice = true;
            }
        }
    }
    
    return bRetinaSupportedOnDevice;
}

float ofAppiOSMetalWindow::getRetinaScale() {
    return settings.retinaScale;
}

//----------------------------------------------------------------------------------- depth buffer.
bool ofAppiOSMetalWindow::enableDepthBuffer() {
	settings.depthType = ofxiOSRendererDepthFormat::DEPTH_24;
    return (settings.enableDepth = true);
}

bool ofAppiOSMetalWindow::disableDepthBuffer() {
	settings.depthType = ofxiOSRendererDepthFormat::DEPTH_NONE;
    return (settings.enableDepth = false);
}

bool ofAppiOSMetalWindow::isDepthBufferEnabled() {
    return settings.enableDepth;
}

//----------------------------------------------------------------------------------- anti aliasing.
bool ofAppiOSMetalWindow::enableAntiAliasing(int samples) {
	settings.numOfAntiAliasingSamples = samples;
    return (settings.enableAntiAliasing = true);
}

void ofAppiOSMetalWindow::enableMultiTouch(bool isOn) {
	settings.enableMultiTouch = isOn;
#if TARGET_OS_IOS || (TARGET_OS_IPHONE && !TARGET_OS_TV)
    if(settings.windowControllerType == METAL_KIT) {
        if([ofxiOSMetalView getInstance]) {
            [[ofxiOSMetalView getInstance] setMultipleTouchEnabled:isOn];
        }
    }
#endif
}

bool ofAppiOSMetalWindow::isMultiTouch() {
	return settings.enableMultiTouch;
}

bool ofAppiOSMetalWindow::disableAntiAliasing() {
    return (settings.enableAntiAliasing = false);
}

bool ofAppiOSMetalWindow::isAntiAliasingEnabled() {
    return settings.enableAntiAliasing;
}

int	ofAppiOSMetalWindow::getAntiAliasingSampleCount() {
    return settings.numOfAntiAliasingSamples;
}

ofxiOSWindowControllerType ofAppiOSMetalWindow::getWindowControllerType() {
	return settings.windowControllerType;
}

ofxiOSRendererColorFormat ofAppiOSMetalWindow::getRendererColorType() {
	return settings.colorType;
}

ofxiOSRendererDepthFormat ofAppiOSMetalWindow::getRendererDepthType() {
	return settings.depthType;
}

ofxiOSRendererStencilFormat ofAppiOSMetalWindow::getRendererStencilType() {
	return settings.stencilType;
}

//-----------------------------------------------------------------------------------
ofCoreEvents & ofAppiOSMetalWindow::events(){
    return coreEvents;
}

//-----------------------------------------------------------------------------------
std::shared_ptr<ofBaseRenderer> & ofAppiOSMetalWindow::renderer(){
    return currentRenderer;
}

ofiOSMetalWindowSettings & ofAppiOSMetalWindow::getSettings() {
    return settings;
}
