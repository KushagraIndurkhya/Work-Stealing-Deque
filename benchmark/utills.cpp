#include "./../src/B_DEQ.cpp"
#include "./../src/UNB_DEQ.cpp"
#include "./../src/WSQ.cpp"
#include<thread>
#include<functional>

using namespace std;
using namespace wsq;

#include<chrono>
/**
 * It returns the number of microseconds since the epoch
 * 
 * @return The number of microseconds since the epoch.
 */
long getEpoch()
{
  using namespace std::chrono;
  microseconds ms = duration_cast<microseconds>(
      system_clock::now().time_since_epoch());

  return ms.count();
}

// Runs a WorkStealingThread
void thread_func(WorkStealingThread *wst)
{
  wst->run();
  printf("Exiting Thread_Func %d \n", wst->get_thread_id());
}

/**
 * It creates an array of `n` threads, each of which has a `k`-sized work queue. The work queue is
 * either bounded or unbounded, depending on the value of `isBounded`. The work queue is filled with
 * `Runnable` objects generated by the `gen_runnable` function. The threads are then started, and the
 * function waits for them to finish
 * 
 * @param n number of threads
 * @param k The maximum number of tasks that can be stored in a queue.
 * @param gen_runnable A function that generates a Runnable object.
 * @param isBounded Whether to use a bounded or unbounded deque.
 * @param enableStealing If true, the threads will steal from each other. If false, they will only
 * steal from the bottom of their own queue.
 */

void create_and_join_threads(int n, int k, function<Runnable()> gen_runnable, bool isBounded, bool enableStealing)
{
  thread threads[n];
  DEQueue *queues[n];
  srand(time(0));

  for (int i = 0; i < n; i++)
  {
    if (isBounded)
      queues[i] = new BDEQueue(k);
    else
      queues[i] = new UnboundedDEQueue(k);
    int len = k;
    for (int j = 0; j < len; j++)
      queues[i]->pushBottom(gen_runnable());
  }
  for (int i = 0; i < n; i++)
  {
    WorkStealingThread *wst = new WorkStealingThread(i, queues, n, enableStealing);
    threads[i] = thread(thread_func, wst);
  }

  for (int i = 0; i < n; i++)
  {
    threads[i].join();
  }
}