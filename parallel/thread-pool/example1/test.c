#include "thread-pool.h"

int main (int argc, char **argv)
{
    int i;
    tp_work *worker = NULL;
    tp_work_desc *job = NULL;
    tp_thread_pool *test_pool = NULL;

    test_pool = creat_thread_pool(1, 10);

    /*线程池中最多三个活动线程*/
    test_pool->init(test_pool);

    /*连续向池中投入10个任务*/
    for (i = 0; i < 10; i++)
    {
        /* workingnum[i] = i; */
        /* test_pool->add_thread(test_pool); */
        test_pool->process_job(test_pool, worker, job);
    }

    /*等待所有任务完成*/
    sleep (50);

    /*销毁线程池*/
    test_pool->close(test_pool);

    return 0;
}
