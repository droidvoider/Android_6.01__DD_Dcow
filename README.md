**STERN DEVELOPER WARNING: PROCESS WILL NOT REINITIATE UNTIL AFTER IT HAS RAN**
**STERN DEVELOPER WARNING: YOU MUST REBOOT BEFORE YOU MAY READ/WRITE AGAIN**


**STERN DEVELOPER WARNING: WRITING SOMETHING MEANS REBOOTING WITH IT!!!!!!**
**STERN DEVELOPER WARNING: WRITING SOMETHING MEANS REBOOTING WITH IT!!!!!!**
**STERN DEVELOPER WARNING: WRITING SOMETHING MEANS REBOOTING WITH IT!!!!!!**

**WARNING THIS IS LITERALLY THE MOST DANGEROUS ANDROID TOOLS I'VE EVER SEEN!!!**
This tool writes to the partitions on 64bit Android 6.01 dirtycow vulnerable devices. It
does so without verifying anything. It will write whatever you tell it to write without
even blinking!! So if you write your recipe for chicken soup to your BOTA0 partition
your device is hisory..

------------------------------------------------------------------------------------------------
------------------- Android 64bit dirtycow image manipulation project --------------------------
------------------------------------------------------------------------------------------------
This educational tool is for replacing images on 64-bit Android 6.01 devices with locked
bootloaders. This process is dangerous even when using the proper tools such as Odin with
the proper factory firmware!

This tool was released 03/29/2017 with very little testing. It is aimed at developers and
isn't meant for anyone who doesn't understand Android completely. If you replace your
bootloader with an incompatible/unsigned version it is possible the hardware will disallow
booting. Developers test these things in a safe way do not test theories on your daily device.
YOU WILL BE VERY VERY SORRY NEW COMER, my name is droidvoider for a reason I ruin a lot of droid.
_________________________________________________________________________________________________


**BASIC INSTRUCTIONS:**
This tool uses a text file to determine what to copy and where. An example of both push/pull files
are included along with this tool.
(you can take the binaries and mimic the process in Makefile for Windows. I will eventually do this)

1. Download the github repo.
2. Open pull_files.txt see the if=/dev/block path then adb shell to your device to confirm path.
Note: For example: ```bash
adb shell then cd /dev/block/platform/15570000.ufs/by-name/ then ls -la
Sub notes: directory taken from pull_files.txt ---> (if=/dev/block/platform/15570000.ufs/by-name/BOTA0)
3. Confirm the Allow MTP message on your device BEFORE continuing or not at all.., before is best
```bash command from ubuntu with ndk installed
4. make pull or make push respectively

**See Makefile for basic process of how things work.
**VERY VERY IMPORTANT NOTE:**
Additionally please notice that you can set the block size for transfers.. You can NOT use bs=10m for 10 megabytes.. It must be written in bytes
(the example is approximately bs=8m, again never use m values with my binaries it will fail)




## Showing the status
```bash
make log
```

## Pulling recover image
first edit pull_files.txt to add RECOVERY path
```bash
make pull
```

## Pushing recovery image (flashing)
first edit push_files.txt to add RECOVERY path
Place the image you want to flash in the root directory push_images with the name matching the name in push_files.txt.
This is a really dangerous function. Only do this if you know what you are doing.
```bash
make push
```

## Pulling  multi images
first edit pull_files.txt to add any/every path
```bash
make pull
```
## Pushing multi image (flashing)
first edit pull_files.txt to add any/every path
Place the image you want to flash in the push_images/ folder in the directory with the binary.
(error?? check names, paths.. see my examples)
This is a really dangerous function. Only do this if you know what you are doing.
```bash
make push
```


## Notes
If the log gets stuck here
```
farm-root: [*] waiting for process to finish
```
try to get system-server to run a toolbox command. (turning lockscreen on and off seems to work for galaxy s7 active and galaxy Note 5). 

## TODO
* fix for armeabi? doesn't seem to work for this architecture right now.

## Help/Sources
* https://github.com/timwr/CVE-2016-5195 for dirtycow exploit.
* https://github.com/jcadduono/android_external_dirtycow for some SELinux context ideas

