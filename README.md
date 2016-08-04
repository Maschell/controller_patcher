# What is in this controller_patcher repository
These files are the magic behind tools like HID to VPAD and can used to use your USB HID Device on your WiiU console.

# How to create config files
Detailed information about creating config files and adding support for more controller can be found in the [wiki](https://github.com/Maschell/controller_patcher/wiki)

# Where can I find config files
Configfiles for all controllers are collection in [this repository](https://github.com/Maschell/controller_patch_configs)

#Dependencies for including it in other applications
This controller_patcher is build to be used in serveral projects. It don't uses some ultra fancy external libraries. </br>
To be able to use in other projects, you will need some external data though.</br>
- At first, you need a copy of the [dynamic_libs](https://github.com/Maschell/dynamic_libs) in your src/dynamic_libs folder.
- To able to log something, you need the right [logger functions](https://github.com/Maschell/hid_to_vpad/tree/master/src/utils).

# Example implementation
###How to "install" it
In order to add hid support to your application, you need add these lines add each start up.
```
  #include "controller_patcher/controller_patcher.h"
  #include "controller_patcher/config_reader.h"
  #include "controller_patcher/cp_retain_vars.h"
  int __entry_menu(int argc, char **argv){
    [...]
    
    init_config_controller(); //Inits the data
    init_button_remapping(); // inits the data for the buttons remapping
    
    if(gConfig_done == HID_INIT_DONE){ // Only read the SD content once (When we're in the Mii Maker and have the rights to do it)
      gConfig_done = HID_SDCARD_READ;       
      ConfigReader::getInstance(); //doing the magic automatically
      ConfigReader::destroyInstance(); //destroying the SD reader
    }
    
    [...]
  }
```

When you return back to the homebrew launcher, simply add the following line:

```
deinit_config_controller();
```
Take a look at the [HID to VPAD](https://github.com/Maschell/hid_to_vpad) sources for an actual example.


###How to use it:

Once you set up everything correctly, you can use the "setControllerDataFromHID" method to fill a VPADData pointer. Example:
```
#include "controller_patcher/controller_patcher.h"

VPADData buffer;
setControllerDataFromHID(&buffer,HID_ALL_CONNECTED_DEVICES);
```
This example will add the input from ALL connected HID devices into the created VPADData.

To remap the gamepad buttons. Use something like this:
```
#include "controller_patcher/cp_retain_vars.h"
#include "controller_patcher/controller_patcher.h"

    if(gButtonRemappingConfigDone && gConfig_done){
        buttonRemapping(buffer);
    }
```


The postion of the mouse cursor can be read from here:
```
#include "controller_patcher/cp_retain_vars.h"

X Value                gHID_Mouse.pad_data[0].data[0].X (0-1280)
Y Value                gHID_Mouse.pad_data[0].data[0].Y (0-720)
```

#Credits:
- Maschell
- FIX94 - huge thanks to him and his initally created gc-to-vpad. Was a motivation and base to start all this
