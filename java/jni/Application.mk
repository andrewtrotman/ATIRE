#APP_OPTIM := release
APP_OPTIM := debug
APP_CFLAGS := -DDEBUG
APP_CPPFLAGS += -frtti # -fexceptions

APP_STL := stlport_static
APP_ABI := armeabi
LOCAL_ARM_MODE := thumb
