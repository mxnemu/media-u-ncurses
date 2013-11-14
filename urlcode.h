#pragma once

/*
  Thanks to Fred Bulback 
  url: http://www.geekhideout.com/urlcode.shtml
 */

char from_hex(char ch);
char to_hex(char code);
char *url_encode(char *str);
char *url_decode(char *str);
