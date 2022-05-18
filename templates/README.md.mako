<%!
    import config.project
    import user.personal
    import config.version
    import config.python
%>${config.project.name}
${'=' * len(config.project.name)}

version: ${config.version.version_str}

description: ${config.project.description_short}

build
-----
![build](https://github.com/veltzer/${config.project.name}/workflows/build/badge.svg)
* test_os: ${config.python.test_os}
* test_python: ${config.python.test_python}


${config.project.description_long}


	${user.personal.personal_origin}, ${config.project.copyright_years_long}
