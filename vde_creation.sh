gksu "vdecmd -s /tmp/ledvde.mgmt shutdown"
gksu "vde_switch -x -t led0 -s /tmp/led0.ctl -m 777 -M /tmp/ledvde.mgmt -d -hub"

# we prefer to use ip over ifconfig (which is obsolete), but the script has to work when ip is not available as well
USINGIP=1
command -v ip >/dev/null 2>&1 || USINGIP=0

if [ $USINGIP -eq 1 ];
then
        gksu "ip addr add 10.42.0.1/24 dev led0"
else
        gksu "ifconfig led0 10.42.0.1 netmask 255.255.255.0"
fi
