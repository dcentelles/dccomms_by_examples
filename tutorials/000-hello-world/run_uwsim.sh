#!/bin/bash

scriptPath=$(realpath $0)
scriptDir=$(dirname $scriptPath)/
scene=$scriptDir/hello_world.xml
dtd=UWSimScene.dtd

uwdata=$(rospack find uwsim)/data/scenes/
if [ ! -f $dtd ]
then
	ln -s $uwdata/$dtd $scriptDir/$dtd
fi


scenepath=$(realpath $scene)

rosrun uwsim uwsim --configfile $scenepath --dataPath $(rospack find uwsim)/data/scenes/ 2>&1  

