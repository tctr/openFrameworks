//
//  ofxiOSMetalView.h
//  iPhone+OF Static Library
//
//  Created by Dan Rosser (147) on 24/6/20.
//
#pragma once
#include <TargetConditionals.h>
#include "ofAppBaseWindow.h"
#include "ofMetalBaseTypes.h"
#include "ofxiOSConstants.h"
#include "ofEvents.h"

class ofiOSMetalWindowSettings: public ofMetalWindowSettings{
public:
    ofiOSMetalWindowSettings()
    :enableRetina(true)
    ,retinaScale(0)
    ,enableDepth(false)
    ,enableAntiAliasing(false)
    ,numOfAntiAliasingSamples(0)
    ,enableHardwareOrientation(false)
    ,enableHardwareOrientationAnimation(false)
    ,enableSetupScreen(true)
    ,windowControllerType(ofxiOSWindowControllerType::METAL_KIT)
    ,colorType(ofxiOSRendererColorFormat::RGBA8888)
    ,depthType(ofxiOSRendererDepthFormat::DEPTH_NONE)
    ,stencilType(ofxiOSRendererStencilFormat::STENCIL_NONE)
    ,enableMultiTouch(false) {
        windowMode = OF_FULLSCREEN;
        setupOrientation = OF_ORIENTATION_DEFAULT;
     
    }
    
    ofiOSMetalWindowSettings(const ofWindowSettings & settings)
    :enableRetina(true)
    ,retinaScale(0)
    ,enableDepth(false)
    ,enableAntiAliasing(false)
    ,numOfAntiAliasingSamples(0)
    ,enableHardwareOrientation(false)
    ,enableHardwareOrientationAnimation(false)
    ,enableSetupScreen(true)
    ,windowControllerType(ofxiOSWindowControllerType::METAL_KIT)
    ,colorType(ofxiOSRendererColorFormat::RGBA8888)
    ,depthType(ofxiOSRendererDepthFormat::DEPTH_NONE)
    ,stencilType(ofxiOSRendererStencilFormat::STENCIL_NONE)
    ,enableMultiTouch(false) {

            enableRetina = true;
            retinaScale = 0;
            enableDepth = false;
            enableAntiAliasing = false;
            numOfAntiAliasingSamples = 0;
            enableHardwareOrientation = false;
            enableHardwareOrientationAnimation = false;
            enableSetupScreen = true;
            setupOrientation = OF_ORIENTATION_DEFAULT;
            windowControllerType = METAL_KIT;
            colorType = ofxiOSRendererColorFormat::RGBA8888;
            depthType = ofxiOSRendererDepthFormat::DEPTH_NONE;
            stencilType = ofxiOSRendererStencilFormat::STENCIL_NONE;
            enableMultiTouch = false;
    
    }


    virtual ~ofiOSMetalWindowSettings(){};
    
    bool enableRetina;
    float retinaScale;
    bool enableDepth;
    bool enableAntiAliasing;
    int numOfAntiAliasingSamples;
    bool enableHardwareOrientation;
    bool enableHardwareOrientationAnimation;
    bool enableSetupScreen;
    bool enableMultiTouch;
    ofxiOSWindowControllerType windowControllerType;
    ofxiOSRendererColorFormat colorType;
    ofxiOSRendererDepthFormat depthType;
    ofxiOSRendererStencilFormat stencilType;
    ofOrientation setupOrientation;
    
};


class ofAppiOSMetalWindow : public ofAppBaseMetalWindow {
public:

    static ofAppiOSMetalWindow * getInstance();
    
    ofAppiOSMetalWindow();
    ~ofAppiOSMetalWindow();
    
    static void loop();
    static bool doesLoop(){ return true; }
    static bool allowsMultiWindow(){ return false; }
    static bool needsPolling(){ return false; }
    static void pollEvents(){ }
    
    void setup(const ofWindowSettings & _settings);
    void setup(const ofiOSMetalWindowSettings & _settings);
    void setup();
    
    void run(ofBaseApp * appPtr);

    static void startAppWithDelegate(std::string appDelegateClassName);
    void update();
    void draw();
   
    void close();
    
    virtual void hideCursor();
    virtual void showCursor();
    
    virtual void setWindowPosition(int x, int y);
    virtual void setWindowShape(int w, int h);
        
    virtual glm::vec2 getWindowPosition();
    virtual glm::vec2 getWindowSize();
    virtual glm::vec2 getScreenSize();
    
#if TARGET_OS_IOS || (TARGET_OS_IPHONE && !TARGET_OS_TV)
    virtual void setOrientation(ofOrientation orientation);
    virtual ofOrientation getOrientation();
    virtual bool doesHWOrientation();
    //-------------------------------------------- ios config.
    bool enableHardwareOrientation();
    bool disableHardwareOrientation();
    
    bool enableOrientationAnimation();
    bool disableOrientationAnimation();
#endif
    
    virtual int getWidth();
    virtual int getHeight();
    
    ofiOSMetalWindowSettings & getSettings();
    ofCoreEvents & events();
    std::shared_ptr<ofBaseRenderer> & renderer();
    
    virtual void setWindowTitle(std::string title);
    
    virtual ofWindowMode getWindowMode();
    
    virtual void setFullscreen(bool fullscreen);
    virtual void toggleFullscreen();
    
    virtual void enableSetupScreen();
    virtual void disableSetupScreen();
    virtual bool isSetupScreenEnabled();
    
    virtual void setVerticalSync(bool enabled);
        
    bool isProgrammableRenderer();
    bool enableRetina(float retinaScale=0);
    bool disableRetina();
    bool isRetinaEnabled();
    bool isRetinaSupportedOnDevice();
    float getRetinaScale();
    
    bool enableDepthBuffer();
    bool disableDepthBuffer();
    bool isDepthBufferEnabled();
    
    bool enableAntiAliasing(int samples);
    bool disableAntiAliasing();
    bool isAntiAliasingEnabled();
    int getAntiAliasingSampleCount();
    
    void enableMultiTouch(bool isOn);
    bool isMultiTouch();
    ofxiOSWindowControllerType getWindowControllerType();
    ofxiOSRendererColorFormat getRendererColorType();
    ofxiOSRendererDepthFormat getRendererDepthType();
    ofxiOSRendererStencilFormat getRendererStencilType();
    
protected:
    
    ofCoreEvents coreEvents;
    std::shared_ptr<ofBaseRenderer> currentRenderer;
    ofiOSMetalWindowSettings settings;

    ofOrientation orientation;
    
    bool bRetinaSupportedOnDevice;
    bool bRetinaSupportedOnDeviceChecked;
    
    bool hasExited;
};
