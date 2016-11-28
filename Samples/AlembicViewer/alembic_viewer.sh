#!/usr/bin/env bash

unix_to_windows_path()
{
  echo "$@" | sed 's/^\/\(.\)\//\1:\\/' | sed 's/\//\\/g'
}

echo "========================"
echo "Launching Alembic Viewer"
echo "========================"
echo ""

source ../../../environment.sh

ALEMBICVIEWERPYPATH=$FABRIC_DIR/Samples/Python

FABRIC_OS=$(uname -s)
if [[ "$FABRIC_OS" == *W32* ]] || [[ "$FABRIC_OS" == *W64* ]]; then
  PYTHONPATH="$(unix_to_windows_path $ALEMBICVIEWERPYPATH);$PYTHONPATH"
else
  PYTHONPATH=$ALEMBICVIEWERPYPATH:$PYTHONPATH
fi
export PYTHONPATH

python $FABRIC_DIR/Samples/Python/AlembicViewer/alembic_viewer.py
