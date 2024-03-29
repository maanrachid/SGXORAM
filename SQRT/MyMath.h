#ifndef MyMath_h
#define MyMath_h




int log(int n){
  int temp=1;
  int base =2;
  int counter=0;
  while (temp<n){
    temp=temp*base;
    counter++;
  }
  return counter;
}

int Sqrt(int x)
{
    // Base cases
    if (x == 0 || x == 1)
       return x;
 
    // Staring from 1, try all numbers until
    // i*i is greater than or equal to x.
    int i = 1, result = 1;
    while (result < x)
    {
       if (result == x)
          return result;
       i++;
       result = i*i;
    }
    return i-1;
}



#endif
