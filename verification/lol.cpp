#include <iostream>

int main()
{
  long x;
  for(long i = 0; i < 20000000; i += 64)
    {
      x = (i / 64) % 8192;
      if(x == 156)
	std::cout << "read("<< i << ");" << std::endl;

    }
}
