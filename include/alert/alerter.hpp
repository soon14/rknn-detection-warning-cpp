#pragma once

#include "runner.hpp"

// 负责启动一个线程从帧队列里拉数据并用detector检测后放入resultQueue中
template<typename AlerterType, typename ResultFrameQueueType>
class Alerter: public Runner
{
public:
    Alerter(AlerterType &alerter, const ResultFrameQueueType &resultFrameQueue): 
        _alerter(alerter), _resultFrameQueue(resultFrameQueue) {}

protected:
    void Run(){
        auto [result, frame] = _resultFrameQueue.Get(_resultFrameQueueId);
        _alerter.Update(result, frame);
    }
private:
    AlerterType &_alerter;
    typename ResultFrameQueueType::DataID _resultFrameQueueId;
    const ResultFrameQueueType &_resultFrameQueue;
};