<%!
    import config.project
    import user.personal
    import config.version
    import config.python
%>${config.project.project_name}
${'=' * len(config.project.project_name)}

version: ${config.version.version_str}

description: ${config.project.project_description}

build
-----
![build](https://github.com/veltzer/${config.project.project_name}/workflows/build/badge.svg)
* test_os: ${config.python.test_os}
* test_python: ${config.python.test_python}


${config.project.project_long_description}


	${user.personal.personal_origin}, ${config.project.project_copyright_years}
