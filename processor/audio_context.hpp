#pragma once
#include <stdint.h>
#include <vector>
#include <map>
#include "ticker.hpp"
#include "audio_context_state.h"
#include "emitter.hpp"
#include "audio_node.hpp"
#include "audio_source_node.hpp"
#include "audio_destination_node.hpp"
#include "thread_pool.hpp"
#include "audio_graph.hpp"
#include "audio_router.hpp"
using namespace std;
namespace vocaloid{
    class AudioContext: public Emitter{
    protected:
        Ticker *ticker_;
        AudioContextState state_;
        uint32_t sample_rate_;
        AudioGraph *graph_;
        AudioRouter *router_;
    public:

        int max_thread_count_ = 4;

        explicit AudioContext(uint32_t sample_rate = 44100):sample_rate_(sample_rate){
            state_ = AudioContextState::FREE;
            ticker_ = new Ticker();
            graph_ = new AudioGraph();
            router_ = new AudioRouter();
        }

        void AddNode(AudioNode* node) {
            graph_->AddNode(node);
        }

        void RemoveNode(AudioNode* node) {
            graph_->RemoveNode(node);
        }

        void Connect(AudioNode* from, AudioNode* to) {
            graph_->Connect(from, to);
        }

        void Disconnect(AudioNode* from, AudioNode *to) {
            graph_->Disconnect(from, to);
        }

        void Setup() {
            router_->Setup(graph_, max_thread_count_);
            state_ = AudioContextState::PREPARE;
        }

        void Run() {
            state_ = AudioContextState::RUN;
            router_->Run();
        }

        void Stop() {
            state_ = AudioContextState::STOP;
            router_->Stop();
        }

        AudioGraph* GetGraph() {
            return graph_;
        }

        Ticker* GetTicker() {
            return ticker_;
        }

        AudioContextState GetState() {
            return state_;
        }

        uint32_t GetSampleRate() {
            return sample_rate_;
        }

        void Dispose() override{
            Emitter::Dispose();
        }
    };
}