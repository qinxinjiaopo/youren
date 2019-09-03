#include <iostream>
#include "util/YR_ThreadPool.h"

using namespace youren;
using namespace std;

int main()
{
    YR_ThreadPool tpool;

            // 4个线程
            tpool.init(4);

            // 启动线程池,tpool.start();
            tpool.start();

            // 执行任务的线程id集合
            std::set<pthread_t> threadIds{};

            for (int i = 0; i < 100000; i++)
            {
                tpool.exec([i, &threadIds]
                {
                    cout << "index = " << i << ", current thread id : " << pthread_self() << endl;
                    threadIds.insert(pthread_self());
                });
            }

            // 等待线程结束

            cout << "waitForAllDone..." << endl;
            bool b = tpool.waitForAllDone(-1); // -1 表示等待执行完
            cout << "waitForAllDone..." << b << ":" << tpool.getJobNum() << endl;
            // 停止线程,析够的时候也会自动停止线程
            // 线程结束时,会自动释放私有数据
            tpool.stop();

            // 打印出执行任务的线程id
            cout << "执行任务的线程id : " << endl;
            for(const auto& iter: threadIds)
            {
                cout << iter << endl;
            }
    return 0;
}

