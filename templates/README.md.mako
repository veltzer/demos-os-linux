<%!
    import config.project
    import user.personal
    import config.git
    import config.python
%>${config.project.project_name}
${'=' * len(config.project.project_name)}

version: ${config.git.last_tag}

build
-----
![build](https://github.com/veltzer/${config.project.project_name}/workflows/build/badge.svg)
* test_container: ${config.python.test_container}
* test_python: ${config.python.test_python}

${config.project.project_long_description}

${config.project.project_description}

	${user.personal.personal_origin}, ${config.project.project_copyright_years}
