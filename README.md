# 说明
这是拿一个NDK模板改的应用，主要用于运行NDK开发的app。

## 使用方法

1. 使用ndk + C++开发一个.so或executable，都可以。确保里面含有android_main1这个函数。这个函数怎么开发后面讲。
2. 把这个.so或executable推送到`/data/data/com.example.native_activity/files`/下面。
这一步需要root权限。

```bash
adb push android_offscreen_test /data/data/com.example.native_activity/files/
```

3. 设置property，告诉native_app你的可执行文件的位置

```bash
adb shell setprop native_app_path /data/data/com.example.native_activity/files/android_offscreen_test
```

4. 启动native_app

```bash
adb shell am start com.example.native_activity/android.app.NativeActivity 
```

## 其他命令

关闭应用:

```bash
adb shell am force-stop com.example.native_activity 
```

查看日志：
```bash
adb shell logcat | grep "native_app"
```

## 总结

```bash

function na_stop()
{
    adb shell am force-stop com.example.native_activity
}

function na_start()
{
    file=$1
    
    readelf -sW $1 | grep "\bandroid_main1\b" || (echo "executable or library does not contain android_main1" && return)
    na_stop
    adb shell logcat -c
    adb push $file /data/data/com.example.native_activity/files/ 
    adb shell setprop native_app_path /data/data/com.example.native_activity/files/$file
    adb shell am start com.example.native_activity/android.app.NativeActivity
    adb shell logcat | grep "native_app"
}

```

## android_main1函数开发


以下是示例代码，注意要extern "C".

```cpp
#include <android/log.h>
#include <android/native_activity.h>
#include <android_native_app_glue.h>
#define ALOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "zhang", __VA_ARGS__))
#define ALOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "zhang", __VA_ARGS__))
extern "C" {

void android_main1(struct android_app *state) {
    ALOGI("event");
    while (true) {
        int ident;
        int events;
        struct android_poll_source *source = nullptr;

        while ((ident = ALooper_pollAll(0, nullptr, &events, (void **)&source)) >= 0) {

            ALOGI("event");
        }
    }
}
} // end extern "C"
```

```

---

Native Activity
===============
Native Activity is an Android sample that initializes a GLES 2.0 context and reads accelerometer data from C code using [Native Activity](http://developer.android.com/reference/android/app/NativeActivity.html).

This sample uses the new [Android Studio CMake plugin](http://tools.android.com/tech-docs/external-c-builds) with C++ support.

Pre-requisites
--------------
- Android Studio 2.2+ with [NDK](https://developer.android.com/ndk/) bundle.

Getting Started
---------------
1. [Download Android Studio](http://developer.android.com/sdk/index.html)
1. Launch Android Studio.
1. Open the sample directory.
1. Open *File/Project Structure...*
  - Click *Download* or *Select NDK location*.
1. Click *Tools/Android/Sync Project with Gradle Files*.
1. Click *Run/Run 'app'*.

Screenshots
-----------
![screenshot](screenshot.png)

Support
-------
If you've found an error in these samples, please [file an issue](https://github.com/googlesamples/android-ndk/issues/new).

Patches are encouraged, and may be submitted by [forking this project](https://github.com/googlesamples/android-ndk/fork) and
submitting a pull request through GitHub. Please see [CONTRIBUTING.md](../CONTRIBUTING.md) for more details.

- [Stack Overflow](http://stackoverflow.com/questions/tagged/android-ndk)
- [Android Tools Feedbacks](http://tools.android.com/feedback)

License
-------
Copyright 2015 Google, Inc.

Licensed to the Apache Software Foundation (ASF) under one or more contributor
license agreements.  See the NOTICE file distributed with this work for
additional information regarding copyright ownership.  The ASF licenses this
file to you under the Apache License, Version 2.0 (the "License"); you may not
use this file except in compliance with the License.  You may obtain a copy of
the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
License for the specific language governing permissions and limitations under
the License.
