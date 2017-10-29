# What is in this controller_patcher repository
These files are the magic behind tools like HID to VPAD and can used to use your USB HID Device on your WiiU console.

# How to create config files
Detailed information about creating config files and adding support for more controller can be found in the [wiki](https://github.com/Maschell/controller_patcher/wiki)

# Where can I find config files
Configfiles for all controllers are collection in [this repository](https://github.com/Maschell/controller_patch_configs)

## Logging usage
To able to use the logging change the "DO_LOGGING" parameter in the Makefile.

# Compiling
You need to install all dependencies first!

Install this static library into your portlibs folder via: 

```
make && make install
```

Link the application with

```
-lutils -ldynamiclibs -lcontrollerpatcher
```

# Dependencies
- Application needs to be loaded from the [homebrew_launcher](https://github.com/dimok789/homebrew_launcher)
- [libutils](https://github.com/Maschell/libutils) for common functions.
- [dynamic_libs](https://github.com/Maschell/dynamic_libs/tree/lib) for access to the functions.

# Example implementation

### How to "install" it
TODO!

```
ControllerPatcher::Init(NULL); //No custom configuration
ControllerPatcher::disableControllerMapping();
ControllerPatcher::startNetworkServer();
```

```
ControllerPatcher::DeInit();
ControllerPatcher::stopNetworkServer();
```

# Credits:
- Maschell  
- FIX94 - huge thanks to him and his initally created gc-to-vpad. Was a motivation and base to start all this
