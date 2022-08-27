// Name: Andrew Chou
// Class: CSS430
// ------------------------------------------Philosopher.cpp----------------------------------------------
// This Philosopher.cpp illustrate the philosopher problem where there are 5 philosophers in total, 2.5
//    pairs of chopsticks (5 piece of chopsticks). The philosopher must pick a pair of chopstick or if 
//    another side is not available, the side being picked up previously must be released back to the table.
//    Table 0, illustrate the way philosopher just randomly pick the left and right chopsticks without
//        checking any availability of it
//    Table 1, illustrate the way only one philosopher can use the table at a time. By that, when one 
//        philosopher is using the table, no one else can use
//    Table 2, illustrate the normal philosopher problem where the philosopher can use the table 
//        concurrently, and picking up and put down chopstick activity is being monitored, so no violation
//        is performed.
//
// -------------------------------------------------------------------------------------------------------

#include <pthread.h> // pthread
#include <stdio.h>
#include <stdlib.h>   // atoi( )
#include <unistd.h>   // sleep( )
#include <sys/time.h> // gettimeofday
#include <iostream>   // cout

#define PHILOSOPHERS 5
#define MEALS 3

using namespace std;

class Table2 {
public:
  Table2( ) {
    pthread_mutex_init(&lock, NULL);
    // HW3A TODO: initialize by yourself
  }
  void pickup( int i ) {
    // HW3A TODO: implement by yourself by referring to the textbook Figure 5.18.
    pthread_mutex_lock(&lock);
    state[i] = HUNGRY;
    test(i);
    if(state[i] != EATING){
      pthread_cond_wait(&self[i], &lock); 
    } 
    cout << "philosopher[" << i << "] picked up chopsticks" << endl;
    pthread_mutex_unlock(&lock);
  }

  void putdown( int i ) {
    // HW3A TODO: implement by yourself by referring to the textbook Figure 5.18.
    pthread_mutex_lock(&lock);
    state[i] = THINKING;
    test((i + 4) % 5);
    test((i + 1) % 5);
    cout << "philosopher[" << i << "] put down chopsticks" << endl;
    pthread_mutex_unlock(&lock);
  }

private:
  enum { THINKING, HUNGRY, EATING } state[PHILOSOPHERS];
  pthread_mutex_t lock;
  pthread_cond_t self[PHILOSOPHERS];

  void test( int i ) {
    // HW3A TODO: implement by yourself by referring to the textbook Figure 5.18.
    if((state[(i + 4) % 5] != EATING) && (state[i] == HUNGRY) && (state[(i + 1) % 5] != EATING)){
      state[i] = EATING;
      pthread_cond_signal(&self[i]);
    }
  }
};

class Table1 {
public:
  Table1( ) {
    // HW3A TODO: initialize the mutex lock
    pthread_mutex_init(&lock, NULL);
  }
  void pickup( int i ) {
    // HW3A TODO: lock the mutex
    pthread_mutex_lock(&lock);
    cout << "philosopher[" << i << "] picked up chopsticks" << endl;
  }

  void putdown( int i ) {
    cout << "philosopher[" << i << "] put down chopsticks" << endl;
    // HW3A TODO: unlock the mutex
    pthread_mutex_unlock(&lock);
  }

private:
  // HW3A TODO: define a mutex lock
  pthread_mutex_t lock;
};

class Table0 {
public:
  void pickup( int i ) {
    cout << "philosopher[" << i << "] picked up chopsticks" << endl;
  }

  void putdown( int i ) {
    cout << "philosopher[" << i << "] put down chopsticks" << endl;
  }
};

static Table2 table2;
static Table1 table1;
static Table0 table0;

static int table_id = 0;

void *philosopher( void *arg ) {
  int id = *(int *)arg;
  
  for ( int i = 0; i < MEALS; i++ ) {
    switch( table_id ) {
    case 0:
      table0.pickup( id );
      sleep( 1 );
      table0.putdown( id );
      break;
    case 1:
      table1.pickup( id );
      sleep( 1 );
      table1.putdown( id );
      break;
    case 2:
      table2.pickup( id );
      sleep( 1 );
      table2.putdown( id );
    break;
    }
  }
  return NULL;
}

int main( int argc, char** argv ) {
  pthread_t threads[PHILOSOPHERS];
  pthread_attr_t attr;
  int id[PHILOSOPHERS];
  table_id = atoi( argv[1] );

  pthread_attr_init(&attr);
  
  struct timeval start_time, end_time;
  gettimeofday( &start_time, NULL );
  for ( int i = 0; i < PHILOSOPHERS; i++ ) {
    id[i] = i;
    pthread_create( &threads[i], &attr, philosopher, (void *)&id[i] );
  }

  for ( int i = 0; i < PHILOSOPHERS; i++ )
    pthread_join( threads[i], NULL );
  gettimeofday( &end_time, NULL );

  sleep( 1 );
  cout << "time = " << ( end_time.tv_sec - start_time.tv_sec ) * 1000000 + ( end_time.tv_usec - start_time.tv_usec ) << endl;

  return 0;
}
