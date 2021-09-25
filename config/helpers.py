import datetime


def get_copyright_years(project_year_started):
    # return ", ".join(map(str, range(int(project_year_started), datetime.datetime.now().year + 1)))
    general_current_year = datetime.datetime.now().year
    if str(general_current_year) == project_year_started:
        return general_current_year
    return f"{project_year_started}-{general_current_year}"


def get_google_analytics(project_google_analytics_tracking_id):
    return f"""<script type="text/javascript">
    (function(i,s,o,g,r,a,m){{i["GoogleAnalyticsObject"]=r;i[r]=i[r]||function(){{
    (i[r].q=i[r].q||[]).push(arguments)}},i[r].l=1*new Date();a=s.createElement(o),
    m=s.getElementsByTagName(o)[0];a.async=1;a.src=g;m.parentNode.insertBefore(a,m)
    }})(window,document,"script","https://www.google-analytics.com/analytics.js","ga");
    ga("create", "{project_google_analytics_tracking_id}", "auto");
    ga("send", "pageview");
    </script>"""


def get_paypal(project_paypal_donate_button_id):
    return f"""<form action="https://www.paypal.com/cgi-bin/webscr"
    method="post" target="_top">
    <input type="hidden" name="cmd" value="_s-xclick">
    <input type="hidden" name="hosted_button_id" value="{project_paypal_donate_button_id}">
    <input type="image" src="https://www.paypalobjects.com/en_US/IL/i/btn/btn_donateCC_LG.gif" name="submit"
    alt="PayPal - The safer, easier way to pay online!">
    <img alt="" border="0" src="https://www.paypalobjects.com/en_US/i/scr/pixel.gif" width="1" height="1">
    </form>"""
