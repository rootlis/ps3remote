ps3remote
=========

Linux userspace driver for SMK-Link VP3700 Bluetooth remote

### Connecting the remote
- On the remote, hold Enter and Start for a few seconds until the blue light at the top turns on
- Set your Bluetooth adapter to scan
- Note your remote's address
- Set your adapter to trust the address
- Set your adapter to connect to the address

The remote light will blink once, and it should be connected. Press a button. If the red light lingers for a second after you release the button, something has gone wrong.

### Running the software
- Compile in the `src` directory with `make`
- Run `./ps3remote`

The driver will set up a new event device under `/dev/input/`, and will automatically connect to the remote when it is available. Press some number buttons. You should see them typed out as you would with a keyboard.

I've also included a systemd service file. Modify it to point to your program. Drop it in `/etc/systemd/system/`.
```
$ systemd start ps3remote.service
$ systemd enable ps3remote.service
Created symlink from /etc/systemd/system/multi-user.target.wants/ps3remote.service to /etc/systemd/system/ps3remote.service.
```

### Modifying the software
You can modify the keymap in [hidraw.c](../blob/master/src/hidraw.c) to set the buttons to whatever you want. Valid key values are in your `linux/input.h` header file.

You may also need to change the value of `REMOTE_HID_ID` in [remote.h](../blob/master/src/remote.h) to match your own device, because this is the value used to identify it from udev.
