#include "alert/trigger/trigger.h"

#include <json/json.h>
#include <cpp-base64/base64.h>
#include "draw/draw.hpp"
#include "alert/trigger/enter.hpp"
#include "alert/trigger/leave.hpp"
#include "alert/trigger/pass.hpp"
#include "alert/trigger/stay.hpp"
#include "alert/trigger/count.hpp"
 
Trigger::Trigger(const std::string &event, const std::string &object, const std::string &region):
    _event(event), _object(object), _region(region)
{
    spdlog::debug("事件[{}]已添加，关联对象为[{}]，对应区域为：[{}]", event, object, region.empty() ? "整个画面" : region);
}
    
// 调用这个接口报警
void Trigger::Alert(const std::vector<STracker> &objs, std::shared_ptr<const cv::Mat> image)
{
    if(!_alertClient){
        spdlog::warn("没有设置报警Url，无法发送报警，事件为[{}]，区域为[{}]", _event, _region);
        return;
    }
    Json::Value alert;
    alert["object"] = _object;
    alert["event"] = _event;
    alert["region"] = _region.empty() ? Json::Value(Json::nullValue) : _region;
    Json::Value information;
    if(objs.empty())
        information = Json::Value(Json::nullValue);
    else{
        for(auto &obj : objs){
            Json::Value boxValue(Json::arrayValue);
            auto &box = obj->GetObject().box;
            boxValue.append(box.x);
            boxValue.append(box.y);
            boxValue.append(box.x + box.w);
            boxValue.append(box.y + box.h);
            information[obj->GetID()] = boxValue;
        }
    }
    alert["information"] = information;
    // 绘制报警对象
    cv::Mat dstImage = *image;
    if(_drawBox && !objs.empty()){
        auto cpImage = dstImage.clone();
        for(auto &obj : objs)
            DrawObject(cpImage, obj->GetObject());
        dstImage = cpImage;
    }
    // 对图片编码
    std::vector<uchar> buf;
    cv::imencode(".jpg", dstImage, buf);
    Json::Value imageValue;
    imageValue["ext"] = "jpg";
    imageValue["base64"] = base64_encode(buf.data(), buf.size());
    alert["image"] = imageValue;

    Json::FastWriter writer;
    _alertClient->Post(_alertPath, writer.write(alert), "application/json");
}

void Trigger::SetDrawBox(bool drawBox)
{
    _drawBox = drawBox;
}

void Trigger::SetAlertUrl(const std::string &alertUrl)
{
    auto protocolEndPos = alertUrl.find("://");
    if (protocolEndPos == std::string::npos) {
        spdlog::critical("设置报警Url失败，缺少协议部分：{}", alertUrl);
        throw std::invalid_argument("设置报警Url失败，缺少协议");
    }

    auto pathStartPos = alertUrl.find('/', protocolEndPos + 3);
    if (pathStartPos == std::string::npos) {
        spdlog::critical("设置报警Url失败，缺少路径部分：{}", alertUrl);
        throw std::invalid_argument("设置报警Url失败，缺少路径");
    }

    _alertClient = std::make_unique<httplib::Client>(alertUrl.substr(0, pathStartPos));
    _alertPath = alertUrl.substr(pathStartPos);
}

STrigger Trigger::ParseTrigger(const std::string &triggerInfo)
{
    Json::Reader reader;
	Json::Value root;
    
    if(!reader.parse(triggerInfo, root)){
        spdlog::critical("解析事件触发器Json字符串失败，事件配置为：[{}]", triggerInfo);
        throw std::invalid_argument("事件触发器构造失败");
    }
    auto condition = root["condition"].asString();
    auto event = root["event"].asString();
    auto object = root["object"].asString();
    std::string region;

    if(auto regionValue = root["region"];!regionValue.isNull())
        region = regionValue.asString();
        
    auto argsValue = root["args"];

    if(condition == "出现")
        return STrigger(new EnterTrigger(event, object, region));
    else if (condition == "离开")
        return STrigger(new LeaveTrigger(event, object, region));
    else if (condition.find("经过") != std::string::npos)
        return STrigger(new PassTrigger(event, object, region, condition));
    
    spdlog::critical("生成事件触发器失败，未知的触发条件[{}]（事件名称为[{}]，对象为[{}]）", condition, event, object);
    throw std::invalid_argument("构造事件触发器失败");
    // event, object, args, region = alert_config['event'], alert_config['object'], alert_config['args'] or [], alert_config.get('region')

    // if condition == '出现':
    //     return EnterTrigger(event, object, region)
    // elif condition == '离开':
    //     return LeaveTrigger(event, object, region)
    // elif condition == '滞留':
    //     return StayTrigger(event, object, region, *args)
    // elif condition.endswith('经过'):
    //     return PassTrigger(event, object, region, condition)
    // elif '等于' in condition \
    //     or '小于' in condition \
    //     or '大于' in condition :
    //     return CountTrigger(event, object, region, condition, *args)
    // raise RuntimeError(f'未知报警条件：{condition}')
}