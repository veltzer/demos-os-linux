-- general config

-- run a command and return the first line of its output
local function capture(command)
    local handle = io.popen(command)
    local result = handle:read("*l")
    handle:close()
    return result
end

GENERAL_CURRENT_FOLDER = capture("pwd"):match("([^/]+)$")
GENERAL_CURRENT_YEAR = tonumber(os.date("!%Y"))
GENERAL_HOMEDIR = os.getenv("HOME")
-- d.general_hostname=subprocess.check_output(["hostname"]).decode().rstrip()
GENERAL_HOSTNAME = capture("hostname")
GENERAL_DOMAIN_NAME = capture("hostname --domain")
