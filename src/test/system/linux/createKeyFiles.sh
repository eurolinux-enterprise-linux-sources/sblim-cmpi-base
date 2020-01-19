#!/bin/sh


DOMAIN=`dnsdomainname`;

#***********************************************************************#
#                          ComputerSystem                               #
#-----------------------------------------------------------------------#

# CreationClassName

echo 'Linux_ComputerSystem' > ComputerSystem.keys

# Name

if [[ -n $DOMAIN ]]; then
    echo `hostname` | grep $DOMAIN >/dev/null\
	&& echo `hostname` >> ComputerSystem.keys \
	|| echo `hostname`.$DOMAIN >> ComputerSystem.keys
else 
    echo `hostname` >> ComputerSystem.keys
fi

#-----------------------------------------------------------------------#



#***********************************************************************#
#                         OperatingSystem                               #
#-----------------------------------------------------------------------#

# CreationClassName

echo 'Linux_OperatingSystem' > OperatingSystem.keys

# Name

if [[ -n $DOMAIN ]]; then
    echo `hostname` | grep $DOMAIN >/dev/null\
	&& echo `hostname` >> OperatingSystem.keys \
	|| echo `hostname`.$DOMAIN >> OperatingSystem.keys
else 
    echo `hostname` >> OperatingSystem.keys
fi

#-----------------------------------------------------------------------#
