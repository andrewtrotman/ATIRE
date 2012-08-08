/*
	ANT_CGI_HEADER()
	----------------
*/
void ANT_CGI_header(char *query)
{
char *source;

puts("<html>");
puts("<head>");
printf("<title>ATIRE - %s</title>\n", query);
puts("</head>");
puts("<body bgcolor=FFFFE0>");
//puts("<a href=env.exe>DEBUG</a>");
puts("<form name=ANTquery action=ant_cgi.exe method=get>");
puts("<table width=100%><tr><td>");
puts("<a href=\"");
source = getenv("HTTP_REFERER");
puts(source == NULL ? "http://localhost/" : source);
puts("\">");
puts("<b style=\"font-size:24;font-family:chiller\">&lt;Back</b>");
puts("</td>");
puts("<td align=right style=\"vertical-align:middle\">");
puts("<img src=\"../logo.png\" width=\"80px\" style=\"vertical-align:middle\" />");
printf("<input type=\"text\" size=\"80\" maxlength=\"128\" name=\"query\" value=\"%s\">", query);
puts("<input type=\"submit\" value=\"Search\">");
puts("</td>");
puts("</tr>");
puts("</table>");
puts("</form>");

puts("<script type=\"text/javascript\" language=\"JavaScript\">");
puts("document.forms['ANTquery'].elements['query'].focus();");
puts("</script>");

puts("<hr>");
}


/*
	ANT_CGI_HEADER()
	----------------
*/
void ANT_CGI_header(void)
{
char *source;

puts("<html>");
puts("<body bgcolor=FFFFE0>");
puts("<head>");
puts("<title>ATIRE</title>");
puts("</head>");
//puts("<a href=env.exe>DEBUG</a>");
//puts("<form name=ANTquery action=ant_cgi.exe method=get>");
puts("<table width=100%><tr><td>");
puts("<a href=\"");
source = getenv("HTTP_REFERER");
puts(source == NULL ? "http://localhost/" : source);
puts("\">");
puts("<b style=\"font-size:24;font-family:chiller\">&lt;Back</b>");
puts("</td>");
puts("<td align=right style=\"vertical-align:middle\">");
puts("<img src=\"../logo.png\" width=\"80px\" style=\"vertical-align:middle\" />");
//printf("<input type=\"text\" size=\"80\" maxlength=\"128\" name=\"query\" value=\"%s\">", query);
//puts("<input type=\"submit\" value=\"Search\">");
puts("</td>");
puts("</tr>");
puts("</table>");
//puts("</form>");

puts("<script type=\"text/javascript\" language=\"JavaScript\">");
puts("document.forms['ANTquery'].elements['query'].focus();");
puts("</script>");

puts("<hr>");
}
