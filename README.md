# Crazy Curves Plugin for After Effects

## Overview
Crazy Curves is an After Effects plugin that provides advanced curve adjustment capabilities for precise control over color grading and tonal adjustments in your compositions.

## Features
- Intuitive curve adjustment interface
- Independent control over RGB channels
- Master RGB curve for overall adjustments
- Real-time preview of changes
- Support for multiple layers
- 32-bit color depth support

## Requirements
- Adobe After Effects CC 2018 or later
- After Effects SDK (download from Adobe's website)
- CMake 3.12 or later
- C++ compiler supporting C++17

## Building from Source

### Prerequisites
1. Download and install the Adobe After Effects SDK
2. Install CMake
3. Install a compatible C++ compiler (Visual Studio 2019 recommended for Windows)

### Build Steps
1. Clone the repository:
```bash
git clone https://github.com/Retr0680/Crazy-Curves.git
cd Crazy-Curves
```

2. Create a build directory and configure CMake:
```bash
mkdir build
cd build
cmake .. -DAE_SDK_PATH=/path/to/your/ae/sdk
```

3. Build the plugin:
```bash
cmake --build . --config Release
```

## Installation
1. Copy the built plugin (.aex file) to your After Effects plug-ins directory:
   - Windows: `C:\Program Files\Adobe\Adobe After Effects [version]\Support Files\Plug-ins`
   - macOS: `/Applications/Adobe After Effects [version]/Plug-ins`

2. Restart After Effects

## Usage
1. Open After Effects
2. Apply the "Crazy Curves" effect to your layer
3. Adjust the curves:
   - Use the master RGB curve for overall adjustments
   - Use individual channel curves for precise color control
   - Click and drag on the curve to add control points
   - Double-click a point to remove it

## Development
The plugin is built using the After Effects SDK and follows Adobe's plugin architecture. Key files:
- `src/CrazyCurves.cpp`: Main plugin implementation
- `src/CrazyCurves.h`: Header file with declarations
- `CMakeLists.txt`: CMake build configuration

## License
This project is licensed under the MIT License. See the LICENSE file for details.

## Contributing
1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## Support
For issues, questions, or contributions, please open an issue on the GitHub repository.