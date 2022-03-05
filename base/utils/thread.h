#ifndef THREAD_H
#define THREAD_H

#include <QtConcurrent>

namespace Thread {

template<typename T>
T syncBlockingRunInThread(std::function<T()> func)
{
    if (!func) {
        return T();
    }
    QEventLoop loop;
    QFutureWatcher<T> watcher;
    QObject::connect(&watcher,
                     &QFutureWatcher<T>::finished,
                     &loop,
                     &QEventLoop::quit,
                     Qt::UniqueConnection);
    QFuture<T> future = QtConcurrent::run(QThreadPool::globalInstance(), [=] { return func(); });
    watcher.setFuture(future);
    loop.exec();
    if (future.isValid()) {
        return future.takeResult();
    }
}

} // namespace Thread

#endif // THREAD_H
