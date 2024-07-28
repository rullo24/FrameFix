# Prerequisites
- O/S: Linux w/ the X11 Windowing System
- X11 Dev Libraries
```bash
sudo apt-get install libx11-dev
```

# Running the Program
```bash
./FrameFix <width> <height> <y_pos_from_top>
```

## Example
```bash
./FrameFix 960 500 25
```

# Features
- Resize Focused Window: Adjust the size of the active window with specified dimensions.
- Automatic Focused Window Detection: Automatically identifies the currently focused window
- Custom Dimensions: Specify the desired width and height for the resize operation.
- Efficient and Fast: Minimal system resource usage with quick execution.
