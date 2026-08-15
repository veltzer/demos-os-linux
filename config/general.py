""" general config """

import datetime
import os
import socket
import subprocess

general_current_folder = os.path.basename(os.getcwd())
general_current_year = datetime.datetime.now(datetime.timezone.utc).year
general_homedir = os.path.expanduser("~")
# d.general_hostname=subprocess.check_output(["hostname"]).decode().rstrip()
general_hostname = socket.gethostname()
general_domain_name = subprocess.check_output(["hostname", "--domain"]).decode().rstrip()
