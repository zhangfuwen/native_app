/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

//BEGIN_INCLUDE(all)
#include <initializer_list>
#include <memory>
#include <cstdlib>
#include <cstring>
#include <jni.h>
#include <cerrno>
#include <cassert>

#include <EGL/egl.h>
#include <GLES/gl.h>

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>

#include <dlfcn.h>

#include <string>
#include <cstdio>
#include <cstdlib>
#include <errno.h>

std::string property_get(const char* key, const char* defaultVal)
{
    std::string ret;
    std::string cmd = "getprop ";
    cmd += key;
    char out[1024];
    FILE* file = popen(cmd.c_str(), "r");
    if(!file) {
        strcpy(out, defaultVal);
        return out;
    }
    fread(out, 1024, 1, file);
    if(ferror(file)) {
        strcpy(out, defaultVal);
        return out;
    }
    pclose(file);
    return out;
}

// trim from start (in place)
static inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
    return s;
}

// trim from end (in place)
static inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
    return s;
}

// trim from both ends (in place)
[[maybe_unused]] inline std::string &trim(std::string &s) {
    ltrim(s);
    rtrim(s);
    return s;
}

// trim from start (copying)
[[maybe_unused]] inline std::string ltrim_copy(std::string s) {
    ltrim(s);
    return s;
}

// trim from end (copying)
[[maybe_unused]] inline std::string rtrim_copy(std::string s) {
    rtrim(s);
    return s;
}

// trim from both ends (copying)
[[maybe_unused]] inline std::string trim_copy(std::string s) {
    trim(s);
    return s;
}

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native_app", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native_app", __VA_ARGS__))

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(struct android_app* state) {
    auto path = property_get("native_app_path", "/data/data/com.example.native_activity/files/android_offscreen_test");
    trim(path);
    LOGI("native app path %s", path.c_str());

    void * handle =  dlopen(path.c_str(), RTLD_NOW);
    if(handle == nullptr) {
        LOGI("failed to dlopen %s, %s", path.c_str(), strerror(errno));
        return;
    }

    void (*f)(struct android_app *state);
    f = (decltype(f))dlsym(handle, "android_main_override");
    auto err = dlerror();
    if(err != nullptr) {
        LOGI("failed to dlsym, %s, %s", strerror(errno), err);
        return;
    }

    if(f == nullptr) {
        LOGI("symbol android_main_override not found");
        return;
    }
    LOGI("ok");
    state->onAppCmd = nullptr;
    state->onInputEvent = [](android_app *app, AInputEvent * event) {
        return 0;
    };
    f(state);
}
//END_INCLUDE(all)
