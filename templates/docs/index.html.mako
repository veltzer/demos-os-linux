<%!
    import pydmt.helpers.project
    import pydmt.helpers.urls
    import pydmt.helpers.signature
    import config.project
    import user.personal
%><!DOCTYPE html>
<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
		<title>${pydmt.helpers.project.get_name()}</title>
		<link rel="shortcut icon" type="image/x-icon" href="../static/favicon.ico"/>
		<link rel="icon" type="image/x-icon" href="../static/favicon.ico"/>
		<meta name="Description" content="${config.project.description_short}"/>
		<meta name="Keywords" content="${user.personal.fullname}, ${user.personal.slug}, ${pydmt.helpers.project.get_name()}, ${', '.join(config.project.keywords)}"/>
	</head>
	<body>
		<h1>Welcome to the <i>${pydmt.helpers.project.get_name()}</i> web site</h1>
		<p>Some related <b>${pydmt.helpers.project.get_name()}</b> project links:</p>
		<ul>
			<li>
			The <b>${pydmt.helpers.project.get_name()}</b> github project is <a title="${pydmt.helpers.project.get_name()} github project" href="${pydmt.helpers.urls.get_website_source()}">here</a>
			</li>
			<li>
			The <b>${pydmt.helpers.project.get_name()}</b> web site for the project is <a title="${pydmt.helpers.project.get_name()} web site" href="${pydmt.helpers.urls.get_website()}">here</a>
			</li>
			<li>
			The <b>${pydmt.helpers.project.get_name()}</b> git clone link is <a title="${pydmt.helpers.project.get_name()} git clone link" href="${pydmt.helpers.urls.get_website_git()}">here</a>

			</li>
			<li>
		</ul>
		<p>
			Copyright ${user.personal.fullname} © ${pydmt.helpers.signature.get_copyright_years_long()}
			<a href="mailto:${user.personal.email}">${user.personal.email}</a>
		</p>
	</body>
</html>
