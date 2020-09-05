#!/bin/bash

scriptPath=$(realpath $0)
scriptDir=$(dirname $scriptPath)/
scene=$scriptDir/add-mac-layer.xml
dtd=$scriptDir/UWSimScene.dtd

uwdata=$(rospack find uwsim)/data/scenes/
rm -f $dtd
ln -s $uwdata/UWSimScene.dtd $dtd


scenepath=$(realpath $scene)

NS_LOG="AquaSimMac=all|prefix_time:AquaSimSFama=all|prefix_time:AquaSimAloha=all|prefix_time" rosrun uwsim uwsim --configfile $scenepath --dataPath $(rospack find uwsim)/data/scenes/ 2>&1  

