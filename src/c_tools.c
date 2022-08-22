#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "c_tools.h"

const char *secureMe(char input[])
{
  int i, x=0;
  char buffer[2048];
  
  for(i=0;input[i]!='\0'&&i<2048;i++)
  {
    switch(i)
    {
        case 2044 ... 2047:
            buffer[i] = '.';
            break;
        default:
            switch(input[i])
            {
            case 32:
            case 48 ... 57:
            case 65 ... 90:
            case 97 ... 122:
                buffer[i+x] = input[i];
                break;
            default:
                buffer[i+x] = '\\';
                buffer[i+1+x] = input[i];
                x++;
                break;
            }
            break;
    }
  }

  char *output = malloc(strlen(buffer));
  snprintf(output, strlen(buffer)+1, "%s", buffer);

  return output;
}