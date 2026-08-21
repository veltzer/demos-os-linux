-- shared stuff
-- config for python module
PCONFIG = {
    "pyclassifiers",
}
-- build for python module
PBUILD = {
    "pyclassifiers",
    "pydmt",
    "pymakehelper",
    "pycmdtools",
}
-- test for python module
PTEST = {
    "pylint",
    "pytest",
    "mypy",
    "ruff",
    -- "pytest-cov",
}
-- deps for python module
PDEPS = {
    "ripgrep",
}
-- config for non python module
CONFIG = {
}
-- build for non python module
BUILD = {
    "pyclassifiers",
    "pydmt",
    "pymakehelper",
    "pycmdtools",
}
-- test for non python module
TEST = {
    "pylint",
    "pytest",
    "mypy",
    "ruff",
}
-- deps for non python module
DEPS = {
}
