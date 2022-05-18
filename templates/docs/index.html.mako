<%!
    import config.project
    import user.personal
%><!DOCTYPE html>
<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
		<title>${config.project.name}</title>
		<link rel="shortcut icon" type="image/x-icon" href="../static/favicon.ico"/>
		<link rel="icon" type="image/x-icon" href="../static/favicon.ico"/>
		<meta name="Description" content="${config.project.long_description}"/>
		<meta name="Keywords" content="${user.personal.personal_fullname}, ${user.personal.personal_slug}, ${config.project.name}, ${', '.join(config.project.keywords)}"/>
	</head>
	<body>
		<h1>Welcome to the <i>${config.project.name}</i> web site</h1>
		<p>Some related <b>${config.project.name}</b> project links:</p>
		<ul>
			<li>
			The <b>${config.project.name}</b> github project is <a title="${config.project.name} github project" href="${config.project.website_source}">here</a>
			</li>
			<li>
			The <b>${config.project.name}</b> web site for the project is <a title="${config.project.name} web site" href="${config.project.website}">here</a>
			</li>
			<li>
			The <b>${config.project.name}</b> git clone link is <a title="${config.project.name} git clone link" href="${config.project.website_git}">here</a>

			</li>
			<li>
		</ul>
		<p>
			Copyright ${user.personal.personal_fullname}, ${config.project.copyright_years}
			<a href="mailto:${user.personal.personal_email}">${user.personal.personal_email}</a>
		</p>
	</body>
</html>
