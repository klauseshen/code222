// ProdCons.cpp: Producer-Consumer Example in SystemC
// EECS 222 solution A3

#include <systemc.h>
#include <stdio.h>

class IP : virtual public sc_interface
{
public:
  virtual void Send(char) = 0;
};

class IC : virtual public sc_interface
{
public:
  virtual char Receive(void) = 0;
};

class C : public sc_channel, public IP, public IC
{
public:
  C(sc_module_name name) : sc_channel(name)
  {}

  void Send(char X)
  {
    Data = X;
    Req.notify(SC_ZERO_TIME);
    wait(Ack);
  }

  char Receive(void)
  {
    char Y;
    wait(Req);
    Y = Data;
    Ack.notify(SC_ZERO_TIME);
    return Y;
  }

private:
  sc_event Req;
  char Data;
  sc_event Ack;
};

class Prod: public sc_module 
{
public:
  sc_port<IP> Port;

  SC_HAS_PROCESS(Prod);

  Prod(sc_module_name name): sc_module(name)
  {
    Msg = "Apples and Bananas";
    SC_THREAD(main);
  }

  void main(void)
  {
    printf("Producer starts.\n");
    const char *p = Msg;
    do
    { cout << "Producer sends '" << *p << "'." << endl;
      Port->Send(*p);
      p++;
    } while(*p != 0);
    Port->Send(*p);
    cout << "Producer done." << endl;
  }

private:
  const char *Msg;
};

class Cons : public sc_module
{
public:
  sc_port<IC> Port;

  SC_HAS_PROCESS(Cons);

  Cons(sc_module_name name) : sc_module(name)
  {
    SC_THREAD(main);
  }

  void main(void)
  {
    cout << "Consumer starts." << endl;
    while(true)
    { Y = Port->Receive();
      if (Y == 0)
	  break;
      cout << "Consumer received '" << Y << "'." << endl;
    }
    cout << "Consumer done." << endl;
  }

private:
  char Y;
};

class Top : public sc_module
{
public:
  C *c;
  Prod *prod;
  Cons *cons;

  Top(sc_module_name name) : sc_module(name)
  {
    c = new C("c");

    prod = new Prod("prod");
    prod->Port(*c);

    cons = new Cons("cons");
    cons->Port(*c);
  }
};

int sc_main(int argc, char *argv[])
{
  Top top("top");

  cout << "Top starts." << endl;
  sc_start();
  cout << "Top done." << endl;
  return 0;
}

// EOF
