unsigned readUnsigned()
{
  unsigned x;
  while (true)
  {
    cin >> x;
    if (x % 2 != 0)
    {
      break;
    }
  }
  return x;
}

bool f()
{
  unsigned b;
  b = 43;
  cout << b; 
}

unsigned main ( unsigned x, bool y, unsigned z)
{
  unsigned a;
  unsigned i;
  unsigned oszto;
  bool vanoszto;
  a = readUnsigned();
  vanoszto = false;
  i = 2;
  f();
  while( !vanoszto && i < a )
  {
    if( a % i == 0 )
    {
      vanoszto = true;
      oszto = i;
    }
    i = i+1;
  }
  if( vanoszto )
  {
    cout << vanoszto;
    cout << oszto;
  }
  else
  {
    cout << vanoszto;
  }
}