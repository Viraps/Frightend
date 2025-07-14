#pragma once
#include <Windows.h>
#include <jni.h>
#include "../event/EventBus.h"

class InputHook {
private:
    static JavaVM* jvm;
    static JNIEnv* env;
    static jclass mouseClass;
    static jmethodID isButtonDownMethod;
    static bool initialized;
    
    // Previous states to detect changes
    static bool prevLeftButton;
    static bool prevRightButton;
    static bool prevMiddleButton;
    
    static bool Initialize();
    static void Cleanup();
    
public:
    static bool Init();
    static void Poll(EventBus& eventBus);
    static void Shutdown();
};