#include "InputHook.h"
#include "../event/impl/MouseEvent.h"
#include "../event/impl/KeyEvent.h"
#include <cstdio>

JavaVM* InputHook::jvm = nullptr;
JNIEnv* InputHook::env = nullptr;
jclass InputHook::mouseClass = nullptr;
jmethodID InputHook::isButtonDownMethod = nullptr;
bool InputHook::initialized = false;
bool InputHook::prevLeftButton = false;
bool InputHook::prevRightButton = false;
bool InputHook::prevMiddleButton = false;

bool InputHook::Initialize() {
  
    jint result = JNI_GetCreatedJavaVMs(&jvm, 1, nullptr);
    if (result != JNI_OK || jvm == nullptr) {
        printf("Failed to get JVM\n");
        return false;
    }
    
    result = jvm->AttachCurrentThread((void**)&env, nullptr);
    if (result != JNI_OK || env == nullptr) {
        printf("Failed to attach to JVM thread\n");
        return false;
    }
    
    mouseClass = env->FindClass("org/lwjgl/input/Mouse");
    if (mouseClass == nullptr) {
        printf("Failed to find Mouse class\n");
        return false;
    }
    
    isButtonDownMethod = env->GetStaticMethodID(mouseClass, "isButtonDown", "(I)Z");
    if (isButtonDownMethod == nullptr) {
        printf("Failed to get isButtonDown method\n");
        return false;
    }
    
    return true;
}

bool InputHook::Init() {
    if (!initialized) {
        initialized = Initialize();
        if (initialized) {
            printf("InputHook initialized successfully\n");
        }
    }
    return initialized;
}

void InputHook::Poll(EventBus& eventBus) {
    if (!initialized) return;

    
    jint leftButtonArg = 0;
    jint rightButtonArg = 1;
    jint middleButtonArg = 2;
    
    bool currentLeftButton = env->CallStaticBooleanMethod(mouseClass, isButtonDownMethod, leftButtonArg);
    bool currentRightButton = env->CallStaticBooleanMethod(mouseClass, isButtonDownMethod, rightButtonArg);
    bool currentMiddleButton = env->CallStaticBooleanMethod(mouseClass, isButtonDownMethod, middleButtonArg);
    
    if (currentLeftButton != prevLeftButton) {
        MouseEvent mouseEvent(0, currentLeftButton);
        eventBus.post(mouseEvent);
        prevLeftButton = currentLeftButton;
    }
    
    if (currentRightButton != prevRightButton) {
        MouseEvent mouseEvent(1, currentRightButton);
        eventBus.post(mouseEvent);
        prevRightButton = currentRightButton;
    }
    
    if (currentMiddleButton != prevMiddleButton) {
        MouseEvent mouseEvent(2, currentMiddleButton);
        eventBus.post(mouseEvent);
        prevMiddleButton = currentMiddleButton;
    }
    
   
    static bool prevSpaceKey = false;
    bool currentSpaceKey = GetAsyncKeyState(VK_SPACE) & 0x8000;
    
    if (currentSpaceKey != prevSpaceKey) {
        KeyEvent keyEvent;
        keyEvent.keyCode = VK_SPACE;
        keyEvent.isDown = currentSpaceKey;
        eventBus.post(keyEvent);
        prevSpaceKey = currentSpaceKey;
    }
}

void InputHook::Cleanup() {
    if (jvm && env) {
        jvm->DetachCurrentThread();
    }
    jvm = nullptr;
    env = nullptr;
    mouseClass = nullptr;
    isButtonDownMethod = nullptr;
}

void InputHook::Shutdown() {
    if (initialized) {
        Cleanup();
        initialized = false;
        printf("InputHook shut down\n");
    }
}