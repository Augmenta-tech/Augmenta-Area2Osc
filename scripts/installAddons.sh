#!/bin/sh

# Script that installs needed addons
# Note : This script only works on OSX and linux, for Windows, please install the addons manually

# Errors out
set -e

[ "$1" = "-h" ] || [ "$1" = "--help" ] || [ $# -gt 0 ] && { echo "Usage : $0 [ --version ]" && exit 1; }

# Get correct paths
ORIG_PATH=`pwd`
cd `dirname $0`
SCRIPT_PATH=`pwd`
cd $ORIG_PATH

case `uname` in

  Darwin)
	# Install ofxSyphon
	ADDONS=ofxSyphon
	cd $SCRIPT_PATH/../../../addons/ &&
	if [ ! -d $ADDONS ]; then
        	git clone https://github.com/astellato/ofxSyphon.git &&
        	cd $ADDONS &&
        	git checkout master
	fi
	;;

  Linux)
	echo "Skipping ofxSyphon because you are on linux"
	;;
esac

# Install ofxStateMachine
ADDONS=ofxStateMachine
cd $SCRIPT_PATH/../../../addons/ &&
if [ ! -d $ADDONS ]; then
git clone https://github.com/Lyptik/ofxStateMachine.git &&
cd $ADDONS &&
git checkout master
fi

cd $ORIG_PATH &&
echo "Done!"
