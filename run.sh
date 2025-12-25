#!/bin/sh

BIN="./build/advanced-nas-server"

if [ ! -f "$BIN" ]; then
    echo "❌ Binary not found: $BIN"
    echo "👉 Build the project first"
    exit 1
fi

if [ ! -x "$BIN" ]; then
    echo "⚠️ No execute permission, fixing..."
    chmod +x "$BIN" || {
        echo "❌ Failed to add execute permission"
        exit 1
    }
fi

exec "$BIN"
