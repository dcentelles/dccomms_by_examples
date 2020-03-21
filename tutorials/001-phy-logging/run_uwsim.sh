#!/bin/bash

scriptPath=$(realpath $0)
scriptDir=$(dirname $scriptPath)/
scene=$scriptDir/phy-logging.xml
dtd=$scriptDir/UWSimScene.dtd

uwdata=$(rospack find uwsim)/data/scenes/
rm -f $dtd
ln -s $uwdata/UWSimScene.dtd $dtd


scenepath=$(realpath $scene)

rosrun uwsim uwsim --configfile $scenepath --dataPath $(rospack find uwsim)/data/scenes/ 2>&1  

