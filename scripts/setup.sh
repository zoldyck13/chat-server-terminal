#!/bin/bash


found_pkg(){
    pkg-config --exists "$1"
}


detect_distro(){
    . /etc/os-release
    echo "$ID"
}

check_ftxui() {
    if pkg-config --exists ftxui 2>/dev/null; then
        echo "ftxui found (pkg-config)"
    elif [ -d "/usr/local/include/ftxui" ]; then
        echo "ftxui found (manual install)"
    else
        echo "Installing ftxui from GitHub..."
        git clone https://github.com/ArthurSonzogni/FTXUI.git
        cd FTXUI
        mkdir build && cd build
        cmake ..
        make -j$(nproc)
        sudo make install
        cd ../..
        rm -rf FTXUI
    fi
}

DISTRO=$(detect_distro)


install_pkg(){
    case "$DISTRO" in
        arch)
            sudo pacman -S --needed --noconfirm "$1"
            ;;
        ubuntu|debian)
            sudo apt install -y "$1"
            ;;
        fedora)
            sudo dnf install -y "$1"
            ;;
        *)
            echo "Unsupported distro"
            ;;
    esac
}


PROG=("git" "make" "cmake" "gcc" "sqlite3" )

for item in "${PROG[@]}"; do
    if command -v $item &>/dev/null; then
        echo "$item already install"
    else
        echo "Installing program $item..."
        install_pkg $item
    fi
done



check_ftxui

echo "All dependencies are ready!"