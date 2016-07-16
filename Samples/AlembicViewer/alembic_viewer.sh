echo "========================"
echo "Launching Alembic Viewer"
echo "========================"
echo ""

BASEDIR=$(dirname "$0")
FABRIC_DIR=$BASEDIR/../../..

source $FABRIC_DIR/environment.sh

PYTHONPATH=$BASEDIR/../:$PYTHONPATH
export PYTHONPATH

python $BASEDIR/alembic_viewer.py
