all: build

build:
	ndk-build NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=./Android.mk APP_ABI=arm64-v8a APP_PLATFORM=android-23

log: 
	adb logcat | grep -a droid_dd_dcow

pull: build 
	adb push pull_files.txt /data/local/tmp/files.txt
	adb push toolbox /data/local/tmp/toolbox
	adb push libs/arm64-v8a/farm /data/local/tmp/farm
	adb push till /data/local/tmp/till
	adb push libs/arm64-v8a/bridge /data/local/tmp/bridge
	adb shell chmod 0777 /data/local/tmp/bridge /data/local/tmp/till /data/local/tmp/farm /data/local/tmp/toolbox
	adb shell /data/local/tmp/farm
	adb pull /data/local/tmp/
	adb reboot

push: build
	adb push push_files.txt /data/local/tmp/files.txt 
	adb push push_images/* /data/local/tmp/
	adb push toolbox /data/local/tmp/toolbox
	adb push libs/arm64-v8a/farm /data/local/tmp/farm
	adb push till /data/local/tmp/till
	adb push libs/arm64-v8a/bridge /data/local/tmp/bridge
	adb shell chmod 0777 /data/local/tmp/bridge /data/local/tmp/till /data/local/tmp/farm /data/local/tmp/toolbox /data/local/tmp/*.img
	adb shell /data/local/tmp/farm

clean:
	rm -rf libs
	rm -rf obj

