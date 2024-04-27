#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <opencv2/opencv.hpp>
#include "tracking/tracker.hpp"
#include "alert/trigger/trigger.h"

// 提供单区域报警触发器管理
class Region
{
public:
    Region();
    Region(const std::string &name, const std::vector<cv::Point2f> &region);

    // 添加trigger
    void AddTrigger(size_t classID, STrigger trigger);

    // 若该区域是否有Trigger
    bool Empty()const;
    
    // 更新
    void Update(std::shared_ptr<TrackerWorld> trackerWorld, std::shared_ptr<cv::Mat> image);

private:
    std::string _name;  // 这个区域的名称
    std::vector<cv::Point2f> _region;  // 这个区域的位置
    std::unordered_map<size_t, std::vector<STrigger>> _triggers;  // 这个区域的报警触发器们
    TrackerWorld _objects;  // 这个区域的物体们
    
    // 判断某个物体是否进入该区域了并处理
    void Enter(STracker tracker, std::shared_ptr<cv::Mat> image, TrackerSet &trackerSet);

    // 判断某个物体是否离开该区域了并处理
    void Leave(STracker tracker, std::shared_ptr<cv::Mat> image, TrackerSet &trackerSet);
};