#! /bin/sh -f
# Written by Furey.
# With additions from Tony.
# With changes from Kahn.
# Ported to 'sh' by Loki for those who don't have 'csh'.
# Thanks to Alan and Grodyn for thier help on the redirecting stuff - Loki
# With changes from Zen to look more like startup.

# Set the port number.
port=4001
if [ "$1" != "" ]; then port=$1; fi

# Set limits. (cannot be used in a 'sh' script - Loki)
# nohup
ulimit -s 1024
if [ -r SHUTDOWN.TXT ]; then rm -f SHUTDOWN.TXT; fi

while [ 1 ]
do
    # If you want to have logs in a different directory,
    #   change the 'logfile=' line to reflect the directory name.
    # (It isn't used but I let it in anyway)
    index=1000
    while [ 1 ]
    do
        logfile=../log/$index.log
        if [ ! -r $logfile ]; then break; fi
        index=`expr $index + 1`
    done

    # Run magma.
    # Thanks to Alan and Grodyn for their help on the redirecting stuff - Loki
    ../src/magma $port >$logfile 2>&1

    # Restart, giving old connections a chance to die.
    if [ -r SHUTDOWN.TXT ]; then
        rm -f SHUTDOWN.TXT
    fi

    sleep 10
done
