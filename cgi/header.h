/*
	ANT_CGI_HEADER()
	----------------
*/
void ANT_CGI_header(char *query)
{
char *source;

puts("<html>");
puts("<body bgcolor=FFFFE0>");
//puts("<a href=env.exe>DEBUG</a>");
puts("<form name=ANTquery action=ant_cgi.exe method=get>");
puts("<table width=100%><tr><td>");
puts("<a href=\"");
source = getenv("HTTP_REFERER");
puts(source == NULL ? "http://localhost/" : source);
puts("\">");
puts("<b style=\"font-size:24;font-family:chiller\"><÷<b style=\"color:800000\">B</b><b style=\"color=008000\">A</b><b style=\"color:000080\">C</b><b style=\"color:808000\">K</b></b>");
puts("</td>");
puts("<td align=right>");
puts("<b style=\"font-size:24;font-family:chiller\"><b style=\"color:800000\">A</b><b style=\"color=008000\">N</b><b style=\"color:000080\">T</b></b>");
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
//puts("<a href=env.exe>DEBUG</a>");
//puts("<form name=ANTquery action=ant_cgi.exe method=get>");
puts("<table width=100%><tr><td>");
puts("<a href=\"");
source = getenv("HTTP_REFERER");
puts(source == NULL ? "http://localhost/" : source);
puts("\">");
puts("<b style=\"font-size:24;font-family:chiller\"><÷<b style=\"color:800000\">B</b><b style=\"color=008000\">A</b><b style=\"color:000080\">C</b><b style=\"color:808000\">K</b></b>");
puts("</td>");
puts("<td align=right>");
puts("<a href=\"/\"><b style=\"font-size:24;font-family:chiller\"><b style=\"color:800000\">A</b><b style=\"color=008000\">N</b><b style=\"color:000080\">T</b></b></a>");
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
