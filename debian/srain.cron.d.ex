#
# Regular cron jobs for the srain package
#
0 4	* * *	root	[ -x /usr/bin/srain_maintenance ] && /usr/bin/srain_maintenance
