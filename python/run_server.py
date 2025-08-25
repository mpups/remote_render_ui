import os

# On Windows we need to set the DLL paths:
if hasattr(os, 'add_dll_directory'):
    os.add_dll_directory(r'C:\msys64\ucrt64\bin')
    os.add_dll_directory(r'C:\msys64\mingw64\bin')
    os.add_dll_directory(r'C:\Users\mpup\workspace\remote_render_ui\external\install\bin')
    os.add_dll_directory(r'C:\Users\mpup\workspace\remote_render_ui\build\external\nanogui')

import gui_server
import numpy as np
import time
import argparse

parser = argparse.ArgumentParser(description='Run the GUI server with configurable port')
parser.add_argument('--port', type=int, default=5000, help='Port to run the server on (default: 5000)')
args = parser.parse_args()

server = gui_server.InterfaceServer(args.port)
server.start()
while not server.wait_until_ready(500):
    # Wait forever: this loop keeps the Python code responsive to ctrl-C
    None

gui_server.set_log_level('off')

state = gui_server.State()
state.value = 4.0

width = 1024
height = 1024

server.initialise_video_stream(width, height)
server.sync_client_ui()
server.sync_state(state)

# Create a zeroed RGB image array
test_image = np.zeros((height, width, 3), dtype=np.uint8)

# Initialize frame offset for scrolling effect
frame_offset = 0
# Create a meshgrid for x and y coordinates
y_coords, x_coords = np.mgrid[0:test_image.shape[0], 0:test_image.shape[1]]

# Initialize step counter for progress updates
step = 0
total_steps = 100

while not server.get_state().stop:
    # Create a scrolling colour gradient:
    shifted_x = (x_coords + frame_offset) % test_image.shape[1]
    test_image[:, :, 0] = shifted_x % 255                # Blue
    test_image[:, :, 1] = (shifted_x + y_coords) % 255   # Green
    test_image[:, :, 2] = y_coords % 255                 # Red

    # Increment the offset for the next frame:
    frame_offset = (frame_offset + 1) % test_image.shape[1]

    # Send the updated image:
    server.send_image(test_image, convert_to_bgr=False)

    # Send some fake progress updates:
    step = (step + 1) % total_steps
    server.update_progress(step, total_steps)

    # Check state and show changes:
    if server.state_changed():
        state = server.consume_state()
        print(f"Value: {state.value}")

    # Sleep 5ms to avoid consuming too much CPU:
    time.sleep(0.005)

server.stop()