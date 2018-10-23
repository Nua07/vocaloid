#pragma once
#include <iostream>
#include "synthesizer.h"
#include "vocaloid/maths/interpolate.hpp"
namespace vocaloid{

    struct GainValue{
        uint64_t timestamp;
        float value;
        INTERPOLATOR_TYPE interpolator;
    };

    class Gain: public Synthesizer{
    private:
        uint64_t offset_;
        uint32_t sample_rate_;
        uint16_t bits_;
        vector<GainValue> value_list;
        int64_t FindIndex(uint64_t timestamp, bool &accurate){
            int64_t start = 0, last = value_list.size() - 1, middle = 0;
            int64_t delta = 0;
            accurate = false;
            if(last < 0)return 0;
            while(start <= last){
                middle = (last + start) / 2;
                delta =  timestamp - value_list[middle].timestamp;
                if(abs(delta) < 0.000001){
                    accurate = true;
                    break;
                }else if(delta > 0){
                    start = middle + 1;
                }else{
                    last = middle - 1;
                }
            }
            if(delta > 0){
                middle++;
            }
            return middle;
        }

        uint64_t CalculatePlayedTime(uint64_t offset){
            return uint64_t((float)(offset) / (bits_/8.0f) / float(sample_rate_) * 1000.0f);
        }

    public:
        float value;

        explicit Gain(float v = 1.0f){
            sample_rate_ = 44100;
            bits_ = 16;
            value = v;
            offset_ = 0;
        }

        void Initialize(uint32_t sample_rate = 44100,
                        uint16_t bits = 16){
            sample_rate_ = sample_rate;
            bits_ = bits;
        }

        void SetValueAtTime(float v, uint64_t start_time){
            bool accurate = false;
            auto index = FindIndex(start_time, accurate);
            value_list.insert(value_list.begin() + index, {start_time, v, INTERPOLATOR_TYPE::NONE});
        }

        void LinearRampToValueAtTime(float v, uint64_t end_time){
            bool accurate = false;
            auto index = FindIndex(end_time, accurate);
            value_list.insert(value_list.begin() + index, {end_time, v, INTERPOLATOR_TYPE::LINEAR});
        }

        float GetValueAtTime(uint64_t time){
            bool accurate = false;
            int64_t index = FindIndex(time, accurate);
            if(index - 1 < 0)return value;
            if(index >= value_list.size()){
                return value_list[value_list.size() - 1].value;
            }
            float v = value;
            if(accurate){
                return value_list[index].value;
            }else{
                int64_t pre = index - 1;
                auto preV = value_list[pre].value;
                auto curV = value_list[index].value;
                auto duration = value_list[index].timestamp - value_list[pre].timestamp;
                auto interpolator = value_list[index].interpolator;
                if(interpolator == INTERPOLATOR_TYPE::NONE)return preV;
                else if(interpolator == INTERPOLATOR_TYPE::LINEAR)
                    v = preV + (curV - preV) * float(time - value_list[pre].timestamp)/duration;
            }
            return v;
        }

        void SetOffset(uint64_t offset){
            offset_ = offset;
        }

        void Offset(uint64_t len){
            offset_ += len;
        }

        uint64_t Process(vector<float> in, uint64_t len, vector<float> &out) override {
            uint64_t o = offset_;
            for(auto i = 0;i < len;i++){
                auto gain_v = GetValueAtTime(CalculatePlayedTime(o++));
                out[i] = in[i] * gain_v;
            }
            return len;
        }

        uint64_t GetOffset(){
            return offset_;
        }
    };
}