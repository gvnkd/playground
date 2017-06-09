@0xc1fb4f9ae0b77ec1;

interface WzNet {
    interface Subscription {}

    interface Publisher(T) {
        subscribe @0 (subscriber: Subscriber(T)) -> (subscription: Subscription);
    }

    interface Subscriber(T) {
        pushMessage @0 (message: T) -> ();
    }

    wzVersion @0 () -> (value: Text); # string version of Warzone
    peVersion @1 () -> (value: Text); # string version of PluginsEngine
    subscribe @2 [T] (subscriber: Subscriber(T)) -> (subscription: Subscription);
    #pushMessage @3 [T] (message: T) -> ();
}
