#!/bin/bash

# Default values
DEFAULT_KERNEL_VERSION="v5.15"
DEFAULT_GIT_REPOSITORY="git@github.com:torvalds/linux.git"
DEFAULT_ARCH="arm64"

# Display usage message
usage() {
    echo "Usage: $0 -v <kernel_version> -r <git_repository> [-a <arch>] -o <output_dir>"
    echo "  -v <kernel_version>: Version of the Linux kernel to compile (default: $DEFAULT_KERNEL_VERSION)"
    echo "  -r <git_repository>: Git repository URL of the Linux kernel (default: $DEFAULT_GIT_REPOSITORY)"
    echo "  -a <arch>: Target architecture (e.g., arm64, arm, x86_64, etc.) Default: $DEFAULT_ARCH"
    echo "  -o <output_dir>: Output directory to move generated files"
    exit 1
}

# Set default values
KERNEL_VERSION="$DEFAULT_KERNEL_VERSION"
GIT_REPOSITORY="$DEFAULT_GIT_REPOSITORY"
ARCH="$DEFAULT_ARCH"

# Check if the specified directory exists, change to it or create it
TARGET_DIR="$HOME/development/inesc"
if [ -d "$TARGET_DIR" ]; then
    cd "$TARGET_DIR" || {
        echo "Failed to change to the directory $TARGET_DIR. Exiting..."
        exit 1
    }
else
    mkdir -p "$TARGET_DIR" || {
        echo "Failed to create the directory $TARGET_DIR. Exiting..."
        exit 1
    }
    cd "$TARGET_DIR" || {
        echo "Failed to change to the directory $TARGET_DIR. Exiting..."
        exit 1
    }
fi

# Display the current working directory
echo "Executing the script in: $(pwd)"

# Parse command-line options
while getopts ":v:r:a:o:" opt; do
    case $opt in
        v)
            KERNEL_VERSION="$OPTARG"
            ;;
        r)
            GIT_REPOSITORY="$OPTARG"
            ;;
        a)
            ARCH="$OPTARG"
            ;;
        o)
            OUTPUT_DIR="$OPTARG"
            ;;
        \?)
            echo "Invalid option: -$OPTARG"
            usage
            ;;
        :)
            echo "Option -$OPTARG requires an argument."
            usage
            ;;
    esac
done

# Check if the kernel directory already exists
KERNEL_DIR="linux"
if [ -d "$KERNEL_DIR" ]; then
    # Check if it is a Git repository
    if [ -d "$KERNEL_DIR/.git" ]; then
        # If it's a Git repository, try to checkout the specified kernel version
        cd "$KERNEL_DIR"
        git fetch origin
        git checkout "$KERNEL_VERSION" || {
            echo "Failed to checkout kernel version $KERNEL_VERSION. Please check if the version exists."
            exit 1
        }
        cd ..
    else
        # If it's not a Git repository, exit and notify the user
        echo "Directory $KERNEL_DIR already exists and is not a Git repository. Please remove or choose a different directory."
        exit 1
    fi
else
    # Clone the Linux kernel source from the specified Git repository
    git clone --branch "$KERNEL_VERSION" "$GIT_REPOSITORY" "$KERNEL_DIR"
fi

# Check if the output directory exists inside the kernel source directory, create it if not
if [ ! -d "$KERNEL_DIR/$OUTPUT_DIR" ]; then
    mkdir -p "$KERNEL_DIR/$OUTPUT_DIR"
fi

# Proceed with compiling the kernel
cd "$KERNEL_DIR"

# Display the current working directory before compiling the kernel
echo "Compiling the kernel in: $(pwd)"

# Configure the kernel for the specified architecture
make ARCH="$ARCH" defconfig

# Compile the kernel
make ARCH="$ARCH" CROSS_COMPILE=aarch64-linux-gnu- -j$(nproc)

# Generate additional files
make ARCH="$ARCH" CROSS_COMPILE=aarch64-linux-gnu- Image.defconfig
make ARCH="$ARCH" CROSS_COMPILE=aarch64-linux-gnu- vmlinux.defconfig
make ARCH="$ARCH" CROSS_COMPILE=aarch64-linux-gnu- foundation-v8-gicv3.dtb
make ARCH="$ARCH" CROSS_COMPILE=aarch64-linux-gnu- foundation-v8.dtb

# Placeholder comment - Add commands to generate initrd.arm64.img if needed
# Example: Replace the following line with the actual command to generate initrd.arm64.img
# mkinitramfs -o "$OUTPUT_DIR/initrd.arm64.img" <additional options>

# Move generated files to the output directory
mv -t "$OUTPUT_DIR" arch/"$ARCH"/boot/Image arch/"$ARCH"/boot/Image.defconfig vmlinux.defconfig arch/"$ARCH"/boot/foundation-v8-gicv3.dtb arch/"$ARCH"/boot/foundation-v8.dtb
# Add command to move initrd.arm64.img if needed

# Display the path where the files were generated
echo "Compilation completed. Generated files moved to: $(pwd)/$OUTPUT_DIR"
