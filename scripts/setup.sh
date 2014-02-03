#!/bin/bash
#
# name: setup.sh
# date: 02 Feb 14
# auth: Zach Hartwig
#
# desc: Setup Bash script for the ROOT-based ADAQAnalysis package. The
#       script sets up access for two different types of users:
#       'users' run preinstalled binaries from /usr/local/adaq;
#       'developers' require path settings relative to their top-level
#       SVN checkout directory of the ADAQ source code. The script
#       should be sourced from the user's bashrc file with the single
#       argument 'usr' or 'dev' as appropriate.

# The HOSTTYPE variable is used to determine whether 32 or 64 bit for
# ADAQ and CAEN libraries should be used
export HOSTTYPE=$(uname -m)

if [ ! "$#" -eq 1 ]; then
    echo -e "\nError! The ADAQAnalysis Setup.sh script only accepts single cmd line arg!"
    echo -e   "       'usr' to setup environment for installed production binaries in /usr/local/adaq"
    echo -e   "       'dev' to setup developer environment for source code binary builds\n"
fi


if [ "$1" ==  'usr' ]; then
    export ADAQUSER=user
    export PATH=/usr/local/adaq:$PATH
    export LD_LIBRARY_PATH=/usr/local/adaq/lib/$HOSTTYPE:$LD_LIBRARY_PATH

    echo -e "\nADAQAnalysis : Environment has been configured for ADAQAnalysis user!\n"

elif [ "$1" == 'dev' ]; then
    export ADAQUSER=developer
    export PATH=$PWD/../bin:$PATH
    export LD_LIBRARY_PATH=$ADAQHOME/lib/$HOSTTYPE:$LD_LIBRARY_PATH
    export PYTHONPATH=$ADAQHOME/lib/$HOSTTYPE:$PYTHONPATH

    echo -e "\nADAQAnalysis : Environment has been configured for ADAQAnalysis developer!\n"
fi
