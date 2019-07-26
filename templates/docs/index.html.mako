<%!
    import config.project
    import user.personal
%><!DOCTYPE html>
<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
		<title>${config.project.project_name}</title>
		<link rel="shortcut icon" type="image/x-icon" href="../static/favicon.ico"/>
		<link rel="icon" type="image/x-icon" href="../static/favicon.ico"/>
		<meta name="Description" content="${config.project.project_long_description}"/>
		<meta name="Keywords" content="${user.personal.personal_fullname}, ${user.personal.personal_slug}, ${config.project.project_name}, ${', '.join(config.project.project_keywords)}"/>
		${config.project.project_google_analytics_snipplet}
	</head>
	<body>
		<h1>Welcome to the <i>${config.project.project_name}</i> web site</h1>
		<p>Some related <b>${config.project.project_name}</b> project links:</p>
		<ul>
			<li>
			The <b>${config.project.project_name}</b> github project is <a title="${config.project.project_name} github project" href="${config.project.project_website_source}">here</a>
			</li>
			<li>
			The <b>${config.project.project_name}</b> web site for the project is <a title="${config.project.project_name} web site" href="${config.project.project_website}">here</a>
			</li>
			<li>
			The <b>${config.project.project_name}</b> git clone link is <a title="${config.project.project_name} git clone link" href="${config.project.project_website_git}">here</a>

			</li>
			<li>
		</ul>
		<p>
			Copyright ${user.personal.personal_fullname}, ${config.project.project_copyright_years}
			<a href="mailto:${user.personal.personal_email}">${user.personal.personal_email}</a>
		</p>
	</body>
</html>
