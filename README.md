# What is in this controller_patcher repository
These files are the magic behind tools like HID to VPAD and can used to use your USB HID Device on your WiiU console.

# How to create config files
Detailed information about creating config files and adding support for more controller can be found in the [wiki](https://github.com/Maschell/controller_patcher/wiki)

# Where can I find config files
Configfiles for all controllers are collection in [this repository](https://github.com/Maschell/controller_patch_configs)

# Dependencies for including it in other applications
This controller_patcher is build to be used in serveral projects. It don't uses some ultra fancy external libraries. </br>
To be able to use in other projects, you will need some external data though.</br>
- At first, you need a copy of the [dynamic_libs](https://github.com/Maschell/dynamic_libs) in your src/dynamic_libs folder.
- To able to log something, you need the right [logger functions](https://github.com/Maschell/hid_to_vpad/tree/master/src/utils).

# Example implementation
### How to "install" it
TODO!

# Credits:
- Maschell  
- FIX94 - huge thanks to him and his initally created gc-to-vpad. Was a motivation and base to start all this
