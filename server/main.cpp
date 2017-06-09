#include "../common/capnp/wzNet.capnp.h"
#include <iostream>
#include <capnp/ez-rpc.h>
#include <kj/debug.h>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <kj/async-io.h>
#include <kj/async.h>

std::mutex mEvents;
std::condition_variable cEvents;
std::queue<std::string> qEvents;

std::mutex mActions;
std::condition_variable cActions;
std::queue<std::string> qActions;

std::vector<WzNet::Subscriber<capnp::AnyPointer>::Client> subscribers;
/*
class EventP final: public kj::EventPort {
    bool wait(){
        return true;
    }

    bool poll(){
        return true;
    }
};

void events(){
    std::cout << "start thread" << std::endl;

    auto ioContext = kj::setupAsyncIo();
    kj::Promise<int> textPromise = kj::Promise<void>(kj::READY_NOW).then([]() { return 234; });
    int r = textPromise.wait(ioContext.waitScope);
    std::cout << r << std::endl;
}

int main(int argc, const char* argv[]) {
    std::cout << "start" << std::endl;

    auto ioContext = kj::setupAsyncIo();
    auto uep = ioContext.unixEventPort;
    kj::Promise<int> textPromise = kj::Promise<void>(kj::READY_NOW).then([]() { return 123; });
    int r = textPromise.wait(ioContext.waitScope);
    std::cout << r << std::endl;

    std::thread t1(events);
    t1.join();
//    EventP eventPort;
//    kj::EventLoop eventLoop(eventPort);
    //auto ws = kj::WaitScope(eventLoop);
//    kj::Promise<int> textPromise = kj::Promise<void>(kj::READY_NOW).then([]() { return 123; });
    //kj::String text = textPromise.wait();
//    print(text);
//    auto r = textPromise.wait();
    //eventLoop.run();
    std::cout << "done" << std::endl;
    return 0;
}
*/


class PublisherImpl final: public WzNet::Publisher<capnp::Text>::Server {
    kj::Promise<void> subscribe(SubscribeContext context) {
        std::cout << "subscribe request received" << std::endl;
        return kj::READY_NOW;
    }
};

class WzNetImpl final: public WzNet::Server {
    kj::Promise<void> wzVersion(WzVersionContext context) {
        context.getResults().setValue("omg");
        return kj::READY_NOW;
    }

    kj::Promise<void> subscribe(SubscribeContext context) {
        std::cout << "subscribe called..." << std::endl;
//        auto& s = context.getParams().getSubscriber();
//        subscribers.push_back(s);
        subscribers.push_back(context.getParams().getSubscriber());
//        subscribers.insert(s);
        return kj::READY_NOW;
    }
public:
    void tick() {
        for(WzNet::Subscriber<capnp::AnyPointer>::Client &s : subscribers){
            auto req = s.pushMessageRequest();
            //req.setMessage("hearthbeat");
            req.send();
            //req.send();
        }
    }

    /*
    kj::Promise<void> Publisher<capnp::Text>::subscribe(SubscribeContext context) {
        return kj::READY_NOW;
    }
    */
};

void tick () {
    auto ioContext = kj::setupAsyncIo();
    while(true) {
        std::cout << "tick" << std::endl;
        for(WzNet::Subscriber<capnp::AnyPointer>::Client &s : subscribers){
            std::cout << "trying to callback a client" << std::endl;
            auto req = s.pushMessageRequest();
            //req.setMessage("hearthbeat");
            req.send();
            //req.send();
        }
        kj::READY_NOW;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}


int main(int argc, const char* argv[]) {
  if (argc != 2) {
    std::cerr << "usage: " << argv[0] << " ADDRESS[:PORT]\n"
        "Runs the server bound to the given address/port.\n"
        "ADDRESS may be '*' to bind to all local addresses.\n"
        ":PORT may be omitted to choose a port automatically." << std::endl;
    return 1;
  }

  // Set up a server.
  capnp::EzRpcServer server(kj::heap<WzNetImpl>(), argv[1]);

//  auto serverH = kj::heap<WzNetImpl>();
//  WzNetImpl& serverRef = *serverH;
//  capnp::EzRpcServer server(kj::mv(serverH), argv[1]);
//  capnp::EzRpcServer server(kj::heap<PublisherImpl>(), argv[1]);

  // Write the port number to stdout, in case it was chosen automatically.
  auto& waitScope = server.getWaitScope();
  uint port = server.getPort().wait(waitScope);

  if (port == 0) {
    // The address format "unix:/path/to/socket" opens a unix domain socket,
    // in which case the port will be zero.
    std::cout << "Listening on Unix socket..." << std::endl;
  } else {
    std::cout << "Listening on port " << port << "..." << std::endl;
  }

  //std::thread hb (tick, std::ref(serverRef));
  std::thread hb (tick);
  //hb.detach();

  // Run forever, accepting connections and handling requests.
  kj::NEVER_DONE.wait(waitScope);
}

