#pragma once
#include <string>
#include "disposable.h"
#include "audio_context_interface.h"
#include "buffer.hpp"
using namespace std;
namespace vocaloid{

    class AudioNode: public IDisposable{
    protected:
        IAudioContext *ctx_;
    public:
        string name_;

        explicit AudioNode(IAudioContext *ctx):ctx_(ctx){
            ctx_->AddNode(this);
        }

        ~AudioNode(){
            ctx_->RemoveNode(this);
        }

        void Process(Buffer*...){}

        void Connect(AudioNode *node){
            ctx_->Connect(this, node);
        }

        void Disconnect(AudioNode *node){
            ctx_->Disconnect(this, node);
        }

        void Dispose() override{}
    };
}