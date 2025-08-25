import os
import sys

def setup_dll_paths():
    """Setup DLL search paths from environment variables."""
    # Add build directory to Python path
    build_dir = os.path.join(os.path.dirname(__file__), '..', 'build')
    sys.path.insert(0, os.path.abspath(build_dir))

    # Get DLL paths from environment variable or use defaults
    dll_paths_env = os.environ.get('PYTHON_DLL_PATHS', '')

    if dll_paths_env:
        dll_paths = dll_paths_env.split(os.pathsep)
    else:
        # Default paths if environment variable not set
        dll_paths = [
            r'C:\msys64\ucrt64\bin',
            r'C:\msys64\mingw64\bin',
            r'C:\Users\mpup\workspace\remote_render_ui\external\install\bin'
        ]

    # Add DLL directories (Windows-specific)
    if hasattr(os, 'add_dll_directory'):
        for path in dll_paths:
            if os.path.exists(path):
                os.add_dll_directory(path)