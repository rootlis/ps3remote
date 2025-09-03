ps3remote
=========

Linux userspace driver for [SMK-Link VP3700 Bluetooth remote](http://www.smklink.com/pages/blu-link-universal-remote-for-playstation-3-vp3700-user-guide)

### Running the software
- Configure your Makefile manually
- Compile in the top directory with `make`
- `make install`
- Run `ps3remote` or `systemctl start ps3remote.service`

The driver will set up a new event device under `/dev/input/`, and will automatically connect to the remote when the remote is available. Press some number buttons. You should see them typed out as you would with a keyboard.

I've included a systemd service file. Modify it to point to your program, drop the modified version in `/etc/systemd/system/`, then run and/or enable it.
```shell
$ systemctl start ps3remote.service
$ systemctl enable ps3remote.service
Created symlink from /etc/systemd/system/multi-user.target.wants/ps3remote.service to /etc/systemd/system/ps3remote.service.
```

### Connecting the remote
- On the remote, hold Enter and Start for a few seconds until the blue light at the top turns on
- Set your Bluetooth adapter to scan
- Note your remote's address
- Set your adapter to trust the address
- Set your adapter to connect to the address

The remote light will blink once, and it should be connected. Press a button. If the red light lingers for a second after you release the button, something has gone wrong.

See my old [.bluetoothctl_history](../master/.bluetoothctl_history) for an example pairing session.

### Modifying the software
You can modify the keymap in [hidraw.c](../master/src/hidraw.c#L67) to set the buttons to whatever you want. Valid key values are in your `linux/input.h` header file. Make sure all your keys are enabled in [input.c](../master/src/uinput.c#L67).

You may also need to change the value of `REMOTE_HID_ID` in [remote.h](../master/src/remote.h#L11) to match your own device, because this is the value used to identify it from udev.
