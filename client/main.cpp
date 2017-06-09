#include "../common/capnp/wzNet.capnp.h"
#include <iostream>
#include <math.h>
#include <capnp/ez-rpc.h>
#include <kj/debug.h>

class SubscriberImpl final: public WzNet::Subscriber<capnp::Text>::Server {
  kj::Promise<void> pushMessage(WzNet::Subscriber<capnp::Text>::PushMessageParams::Reader context) {
    auto msg = context.getMessage();
    std::cout << "received message: " << msg.cStr() << std::endl;
    return kj::READY_NOW;
  }
};

/*
class CallbackImpl final : public WzNet::Callback::Server {
public:
    kj::Promise<void> event(WzNet::Callback::EventParams::Reader params){
//        KJ_REQUIRE(params.size() == 1, "wrong number of parameters.");
        auto payload = params.getPayload();
        std::cout << "event received: " << payload.cStr() << std::endl;
//        context.getResults().setValue(pow(params[0], params[1]));
        return kj::READY_NOW;
    }
};
*/
int main(int argc, char *argv[])
{
    if (argc != 2) {
        std::cerr << "usage: " << argv[0] << " HOST:PORT\n"
                                             "Connects to the wzNet server at the given address and "
                                             "does some RPCs." << std::endl;
        return 1;
    }

    capnp::EzRpcClient client(argv[1]);
    auto& waitScope = client.getWaitScope();
    WzNet::Client cl = client.getMain<WzNet>();
/*
    {
        std::cout << "register event callback..." << std::endl;
        auto req = cl.subscribeRequest<capnp::Text>();
        req.setSubscriber(kj::heap<SubscriberImpl>());
        auto promise = req.send();
        auto resp = promise.wait(waitScope);
    }
*/
    {
    std::cout << "listening for callbacks..." << std::endl;
    std::cout.flush();
    }

    {
	std::cout << "requesting wzVersion..." << std::endl;
	std::cout.flush();

	auto request = cl.wzVersionRequest();

	auto evalPromise = request.send();
	auto response = evalPromise.wait(waitScope);
	auto ver = response.getValue();

	std::cout << "wzVer: " << ver.cStr() << std::endl;
    }
    kj::NEVER_DONE.wait(waitScope);
    return 0;
}
