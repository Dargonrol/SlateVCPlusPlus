{
  description = "SlateVC Client Development Environment";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = { self, nixpkgs }: let
    system = "x86_64-linux";
    pkgs = import nixpkgs { inherit system; };

    fhs = pkgs.buildFHSEnv {
      name = "slatevc-dev-env";
      targetPkgs = pkgs: with pkgs; [
        git
        cmake
        ninja
        pkg-config
        gcc
        gdb
        binutils
        python3
        conan

        libGL
        libglvnd
        libx11
        libx11.dev
        libxcursor
        libxcursor.dev
        libxrandr
        libxrandr.dev
        libxinerama
        libxinerama.dev
        libxi
        libxi.dev
        libxext
        libxext.dev
        libxxf86vm
        libxxf86vm.dev
        libxkbcommon
        libxkbcommon.dev
        libxrender
        libxrender.dev
        libxfixes
        libxfixes.dev
        libxcb
        xcbutil
        xcbutilcursor
        xcbutilwm
        xcbutilkeysyms
        xcbutilimage
        xcbutilrenderutil
        libfontenc
        libICE
        libSM
        libXt
        libXmu
        libuuid
        libXScrnSaver
        libXv
        libXtst
        libXcomposite
        libXdamage
        libXaw
        libXau
        libXdmcp
        libxkbfile
        libXpm
        xorgproto
      ];
      profile = ''
        mkdir -p /tmp/pkgconfig
        mkdir -p /tmp/lib

        cat << 'EOF' > /tmp/bin/pkg-config
#!/usr/bin/env bash
REAL_PKG_CONFIG="/usr/bin/pkg-config"
if [ ! -x "$REAL_PKG_CONFIG" ]; then
  REAL_PKG_CONFIG="/bin/pkg-config"
fi

for arg in "$@"; do
  if [[ "$arg" != -* ]] && [[ "$arg" != *=* ]]; then
    PC_FILE="/tmp/pkgconfig/$arg.pc"
    if [ ! -f "$PC_FILE" ]; then
      mkdir -p /tmp/pkgconfig
      echo "Name: $arg" > "$PC_FILE"
      echo "Description: Auto stub for $arg" >> "$PC_FILE"
      echo "Version: 1.0" >> "$PC_FILE"
      echo "Libs: -L/tmp/lib -L/usr/lib -L/usr/lib64 -l$arg" >> "$PC_FILE"
      echo "Cflags: -I/usr/include" >> "$PC_FILE"
    fi
  fi
done

export PKG_CONFIG_PATH="/tmp/pkgconfig:/usr/lib/pkgconfig:/usr/share/pkgconfig:''${PKG_CONFIG_PATH:-}"
exec "$REAL_PKG_CONFIG" "$@"
EOF
        chmod +x /tmp/bin/pkg-config

        for lib in /usr/lib/lib*.so* /usr/lib64/lib*.so*; do
          if [ -f "$lib" ] || [ -L "$lib" ]; then
            base=$(basename "$lib")
            lower=$(echo "$base" | tr '[:upper:]' '[:lower:]')
            ln -sf "$lib" "/tmp/lib/$lower"
            
            if [[ "$lower" =~ (\.so\.[0-9]+) ]]; then
              unversioned=$(echo "$lower" | sed -E 's/\.so\..*/\.so/')
              if [ ! -e "/tmp/lib/$unversioned" ]; then
                ln -sf "$lib" "/tmp/lib/$unversioned"
              fi
            fi
          fi
        done

        for ghost in xcb-event xcb-aux xcb-atom xcb-renderutil; do
          if [ ! -e "/tmp/lib/lib$ghost.so" ]; then
            ln -sf /usr/lib/libxcb.so /tmp/lib/lib$ghost.so 2>/dev/null || ln -sf /usr/lib64/libxcb.so /tmp/lib/lib$ghost.so 2>/dev/null || true
          fi
        done

        if [ ! -e "/tmp/lib/libxscrnsaver.so" ]; then
          ln -sf /usr/lib/libXss.so /tmp/lib/libxscrnsaver.so 2>/dev/null || ln -sf /usr/lib64/libXss.so /tmp/lib/libxscrnsaver.so 2>/dev/null || true
        fi

        if [ ! -e "/tmp/lib/libxres.so" ]; then
          ln -sf /usr/lib/libXext.so /tmp/lib/libxres.so 2>/dev/null || ln -sf /usr/lib64/libXext.so /tmp/lib/libxres.so 2>/dev/null || true
        fi

        if [ ! -e "/tmp/lib/libxpm.so" ]; then
          ln -sf /usr/lib/libXpm.so /tmp/lib/libxpm.so 2>/dev/null || ln -sf /usr/lib64/libXpm.so /tmp/lib/libxpm.so 2>/dev/null || true
        fi

        export PATH="/tmp/bin:$PATH"
        export PKG_CONFIG_PATH="/tmp/pkgconfig:/usr/lib/pkgconfig:/usr/share/pkgconfig"
        export LIBRARY_PATH="/tmp/lib:/usr/lib:/usr/lib64"

        echo "=== Entered SlateVC FHS Environment ==="
        if [ -f "./setup.sh" ]; then
          bash ./setup.sh
        fi

        if [ ! -f "build/Debug/CMakeCache.txt" ]; then
          echo "--> Configuring CMake Debug build..."
          cmake --preset debug
          cmake --preset release
        fi
      '';
    };
  in {
    devShells.${system}.default = fhs.env;
  };
}
