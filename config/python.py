""" python deps for this project """

import config.shared

install_requires: list[str] = [
    "cpplint",
]
build_requires: list[str] = config.shared.BUILD
test_requires: list[str] = config.shared.TEST
requires = install_requires + build_requires + test_requires
