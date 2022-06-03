#! /bin/sh
#
# INIT script for Selenika Peer Server
# Version: 1.1.1 03-June-2022 sobolentcev@gmail.com
#
### BEGIN INIT INFO
# Provides:          Selenika Peer Server
# Required-Start:    $local_fs $remote_fs
# Required-Stop:     $local_fs $remote_fs
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: Selenika Peer Server Service
# Description:       Selenika Peer Server Service
### END INIT INFO

. /lib/lsb/init-functions

PATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin
DAEMON=/usr/share/selenika/peer-server/start.sh
EXEC=/usr/share/selenika/peer-server/selenika-peer-server
NAME=selenika-peer-server

LOGFILE=/var/log/selenika-peer-server.log
DESC=peer-server

DAEMON_OPTS="$PEER_SERVER_OPTS"

if [ ! -x $DAEMON ] ;then
  echo "Daemon not executable: $DAEMON"
  exit 1
fi

set -e
    
stop() {
    echo "Stops exec: $EXEC" 
    # Stops the daemon/service
    #
    # Return
    #   0 if daemon has been stopped
    #   1 if daemon was already stopped
    #   2 if daemon could not be stopped
    #   other if a failure occurred
    start-stop-daemon --stop --exec $EXEC
    RETVAL="$?"
    sleep 1
    return "$RETVAL"
}

start() {
    echo -n "Starting $DESC: "
    DAEMON_START_CMD="exec $DAEMON $DAEMON_OPTS"

    start-stop-daemon --start --name $NAME \
        --exec /bin/bash -- -c "$AUTHBIND_CMD $DAEMON_START_CMD"
    echo "$NAME started."
}

reload() {
    echo 'Not yet implemented.'
}

status() {
    status_of_proc -p $EXEC name "$NAME" && exit 0 || exit $?
}

case "$1" in
  start)
    start
    ;;
  stop)
    stop
    ;;
  restart)
    stop
    start
    ;;
  reload)
    reload
    ;;
  force-reload)
    reload
    ;;
  status)
    status
    ;;
  *)
    N=/etc/init.d/$NAME
    echo "Usage: $N {start|stop|restart|reload|status}" >&2
    exit 1
    ;;
esac

exit 0
