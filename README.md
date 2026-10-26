# SlateVC++
A blazingly fast SlateVC Client

# Setup
### Windows
```sh
.\setup.bat
```
### Linux
```sh
chmod +x setup.sh
./setup.sh
```
## Configuring CMAKE and compiling
If you are working with an IDE like CLion, try to reload the CMakeLists. 
The IDE should now detect the presets.

### configuring manually
If not or if you are working from the terminal you can manually configure the project:

```sh
# List all available presets:
cmake --list-presets

# configure the preset you want:
cmake --preset debug
```

### compiling manually
```sh
# to compile when working from terminal:
cmake --build --preset debug

# multithreaded build:
cmake --build --preset debug -j 8

# cleanbuild:
cmake --build --preset debug --target clean
```

## Running in NIXOS
To compile and work with the project in NixOS you just need to run `nix develop`
in the console and optionally if you want to use your IDE e.g. CLion start it from the same environment:
```sh
nix develop
clion &
```

### disclaimer
I don't really use NixOS but wanted to provide a flake anyway. Since I didn't wanted to spend 20 hours on something I don't use I vibecoded the flake.nix (and reviewed it). This is pretty much the only thing that I vibecoded in the entire project.


# ideas
- Background customization:
  - When selecting picture, render via opengl and use Texture shader. 
  - User can set custom shader.
